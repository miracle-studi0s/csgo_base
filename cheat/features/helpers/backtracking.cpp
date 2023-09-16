#include "../../callbacks.hpp"
#include "../../main_definitions.hpp"
#include "../../security/security.hpp"
#include "../../sdk/sdk.hpp"
#include "threaded_animstate.hpp"
#include "game_information.hpp"
#include "animation_fix.hpp"
#include "backtracking.hpp"

bool c_backtracking::is_tick_valid(c_cs_player* player, animation_record_t* record) {
	if (__game_info->cl_lagcomp0)
		return true;

	if (record->information.has(RECORD_TICKBASE_DRIFT))
		return false;

	float correct = std::clamp(__game_info->lerp_time + __game_info->ping, 0.f, __game_info->max_unlag);

	const auto server_tickcount = __interfaces->global_vars->tickcount + TIME_TO_TICKS(__game_info->ping);
	const auto dead_time = static_cast<int>(TICKS_TO_TIME(server_tickcount) - __game_info->max_unlag);

	float delta_time = std::fabs(correct - (__game_info->predicted_time - record->sim_time));
	return delta_time < 0.2f && (record->sim_time >= static_cast<float>(dead_time));
}

void c_backtracking::build_roll_matrix(c_cs_player* player, animation_side_t* side, int side_index, float& fresh_tick, vec3_t& fresh_angles, clamp_bones_info_t& clamp_info) {
	RESTORE(player->eye_angles().z);

	math::memcpy_sse(side->roll_matrix, side->matrix, sizeof(side->roll_matrix));
	player->eye_angles().z = 50.f * -SIDE_LEFT;

	auto& builder = side->bone_builder;
	builder.clamp_bones_in_bbox(player, side->roll_matrix, builder.mask, fresh_tick, fresh_angles, clamp_info);
}

void c_backtracking::clamp_matrix(c_cs_player* player, animation_side_t* side, float& fresh_tick, vec3_t& fresh_angles, clamp_bones_info_t& clamp_info) {
	auto& builder = side->bone_builder;
	builder.clamp_bones_in_bbox(player, builder.matrix, builder.mask, fresh_tick, fresh_angles, clamp_info);
}

float c_backtracking::get_lerp_time() {
	static auto cl_updaterate = __convars->get(XOR("cl_updaterate"));
	static auto cl_interp_ratio = __convars->get(XOR("cl_interp_ratio"));
	static auto cl_interp = __convars->get(XOR("cl_interp"));

	return std::max(cl_interp->get_float(), cl_interp_ratio->get_float() / cl_updaterate->get_float());
}

animation_record_t* c_backtracking::get_first_record(c_cs_player* player) {
	auto info = __animation_fix->get_animation_info(player->index());
	if (!info->updated(player))
		return nullptr;

	auto& first_record = info->animation_records.front();
	if (first_record.choke_amount > 2 && first_record.information.has(RECORD_SHOULD_EXTRAPOLATE) || __game_info->cl_lagcomp0)
		return &first_record;

	auto record = std::find_if(info->animation_records.begin(), info->animation_records.end(), [&](animation_record_t& current) {
		return current.information.has(RECORD_VALID);
		});

	if (record != info->animation_records.end())
		return &*record;

	return nullptr;
}

animation_record_t* c_backtracking::get_last_record(c_cs_player* player) {
	auto info = __animation_fix->get_animation_info(player->index());
	if (!info->updated(player))
		return nullptr;

	auto& first_record = info->animation_records.front();
	if (first_record.choke_amount > 2 && first_record.information.has(RECORD_SHOULD_EXTRAPOLATE) || __game_info->cl_lagcomp0)
		return nullptr;

	auto record = std::find_if(info->animation_records.rbegin(), info->animation_records.rend(), [&](animation_record_t& current) {
		return current.information.has(RECORD_VALID);
		});

	if (record != info->animation_records.rend())
		return &*record;

	return nullptr;
}

void c_backtracking::set_record(c_cs_player* player, animation_record_t* record, matrix3x4_t* matrix) {
	player->origin() = record->origin;
	player->set_abs_origin(record->origin);

	player->bb_maxs() = record->maxs;
	player->bb_mins() = record->mins;

	player->invalidate_bone_cache();
	player->set_bone_cache(matrix);
}

void c_backtracking::collect(c_cs_player* player, animation_info_t* info, animation_record_t* record) {
	if (!__game_info->cl_lagcomp0) {
		record->information.remove(RECORD_TICKBASE_DRIFT | RECORD_SHOULD_EXTRAPOLATE);

		if (info->shifted_simulation_time > player->sim_time())
			record->information.force(RECORD_TICKBASE_DRIFT);
		else if (player->sim_time() <= player->old_sim_time())
			record->information.force(RECORD_TICKBASE_DRIFT);
		else 
			info->shifted_simulation_time = player->sim_time();

		auto prev_record = info->prev_record;
		if (prev_record && record->choke_amount > 3 && (record->origin - prev_record->origin).length_sqr() > 4096.f)
			record->information.force(RECORD_SHOULD_EXTRAPOLATE);
	}
	else 
		record->information.remove(RECORD_TICKBASE_DRIFT | RECORD_SHOULD_EXTRAPOLATE);
}

void c_backtracking::update_player_lagcomp(c_cs_player* player, float& fresh_tick) {
	if (!player->is_alive())
		return;

	auto info = __animation_fix->get_animation_info(player->index());
	if (!info)
		return;

	if (!info->updated(player))
		return;

	auto& fresh_angles = player->eye_angles();

	// HACK: due to ClampBonesInBbox use vars that are not lag compensated
	// we should update main matrix position here with fresh values
	for (auto& i : info->animation_records) {
		if (is_tick_valid(player, &i))
			i.information.force(RECORD_VALID);
		else
			i.information.remove(RECORD_VALID);

		clamp_bones_info_t clamp_info{};
		clamp_info.store(&i);

		// update orig matrix
		clamp_matrix(player, &i.side_original, fresh_tick, fresh_angles, clamp_info);

		// update left matrix & left roll matrix
		clamp_matrix(player, &i.side_left, fresh_tick, fresh_angles, clamp_info);
		build_roll_matrix(player, &i.side_left, SIDE_LEFT, fresh_tick, fresh_angles, clamp_info);

		// update right matrix & right roll matrix
		clamp_matrix(player, &i.side_right, fresh_tick, fresh_angles, clamp_info);
		build_roll_matrix(player, &i.side_right, SIDE_RIGHT, fresh_tick, fresh_angles, clamp_info);

		// update zero matrix
		clamp_matrix(player, &i.side_zero, fresh_tick, fresh_angles, clamp_info);
	}
}

#ifdef _DEBUG
void thread_update_lagcomp(std::unique_ptr<c_backtracking>& ptr, c_cs_player* player, float& fresh_tick) {
	ptr->update_player_lagcomp(player, fresh_tick);
}
#else
void thread_update_lagcomp(c_backtracking* ptr, c_cs_player* player, float& fresh_tick) {
	ptr->update_player_lagcomp(player, fresh_tick);
}
#endif

void c_backtracking::predict(c_user_cmd* cmd) {
	if (!__game_info->connected)
		return;

	if (!__local_player)
		return;

	auto& fresh_tick = __game_info->predicted_time;

	__entlistener->for_each_player([&](c_cs_player* player) {
#ifdef _DEBUG
		__thread_pool->add_task(thread_update_lagcomp, std::ref(__backtracking), player, std::ref(fresh_tick));
#else
		__thread_pool->add_task(thread_update_lagcomp, __backtracking, player, std::ref(fresh_tick));
#endif
	});

	__thread_pool->wait_all();
}