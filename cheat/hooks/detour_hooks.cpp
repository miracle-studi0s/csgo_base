#include "hooks.hpp"
#include "../sdk/sdk.hpp"
#include "../features/features.hpp"

#define ADD_DETOUR(hook_name, hook_interface, offset) \
	add_hook(hook_name, FIND_OFFSET(__modules->hook_interface, offset).cast<void*>()) 

#define ADD_DETOUR_RELATIVE(hook_name, hook_interface, offset) \
	add_hook(hook_name, FIND_OFFSET(__modules->hook_interface, offset).relative(XORN(1)).cast<void*>())

void __fastcall get_exposure_range(float* min, float* max) {
	static auto original = __hooks->get_original(&get_exposure_range);

	*min = 1.f;
	*max = 1.f;

	original(min, max);
}

void __fastcall calc_view_model_bob(void* ecx, void* edx, vec3_t& position) {
	static auto original = __hooks->get_original(&calc_view_model_bob);
	original(ecx, edx, position);
}

void __fastcall physics_simulate(c_cs_player* ecx, void* edx) {
	static auto original = __hooks->get_original(&physics_simulate);
	int& simulation_tick = *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(ecx) + 0x2AC);

	if (!ecx || !ecx->is_alive()
		|| __interfaces->global_vars->tickcount == simulation_tick
		|| __interfaces->engine->is_hltv()
		|| ecx->flags().has(FL_ATCONTROLS)
		|| ecx != __local_player)
	{
		original(ecx, edx);
		return;
	}

	auto& ctx = ecx->cmd_context();

	original(ecx, edx);
}

void __fastcall update_postscreen_effects(void* ecx, void* edx) {
	static auto original = __hooks->get_original(&update_postscreen_effects);
	original(ecx, edx);
}

bool __fastcall draw_fog(void* ecx, void* edx) {
	static auto original = __hooks->get_original(&draw_fog);
	return original(ecx, edx);
}

int __fastcall send_datagram(c_net_channel* ecx, void* edx, void* datagram) {
	static auto original = __hooks->get_original(&send_datagram);
	return original(ecx, edx, datagram);
}

bool __fastcall send_net_msg(i_net_channel_info* ecx, void* edx, c_net_message& msg, bool force_reliable, bool voice) {
	static auto original = __hooks->get_original(&send_net_msg);
	return original(ecx, edx, msg, force_reliable, voice);
}

bool __fastcall using_static_prop_debug(void* ecx, void* edx) {
	static auto original = __hooks->get_original(&using_static_prop_debug);
	return original(ecx, edx);
}

bool __fastcall msg_voice_data(void* ecx, void* edx, c_svc_msg_voice_data* message) {
	static auto original = __hooks->get_original(&msg_voice_data);
	return original(ecx, edx, message);
}

void __vectorcall read_packets(bool final_tick) {
	static auto original = __hooks->get_original(&read_packets);
	original(final_tick);
}

void __vectorcall cl_move(float accumulated_extra_samples, bool final_tick) {
	static auto original = __hooks->get_original(&cl_move);
	original(accumulated_extra_samples, final_tick);
}

void __fastcall process_packet(c_net_channel* ecx, void* edx, void* packet, bool header) {
	static auto original = __hooks->get_original(&process_packet);
	original(ecx, edx, packet, header);

	// remove event delay and execute it after it recieved
	// prevent delays or other issues
	
	// TO-DO: remove hardcoded offset
	auto events = *reinterpret_cast<c_event_info**>(reinterpret_cast<std::uintptr_t>(__interfaces->client_state) + XORN(0x4E6C));
	if (events) {
		auto iter = events;
		while (iter) {
			if (iter->class_id)
				iter->delay = 0.f;

			iter = iter->next;
		}
	}

	__interfaces->engine->fire_events();
}

void __fastcall get_color_modulation(i_material* ecx, void* edx, float* r, float* g, float* b) {
	static auto original = __hooks->get_original(&get_color_modulation);
	original(ecx, edx, r, g, b);
}

int process_interpolated_list() {
	static auto original = __hooks->get_original(&process_interpolated_list);

	static auto allow_extrapolation = *FIND_OFFSET(__modules->client, OFFSET_ALLOW_EXTRAPOLATION).add(XORN(1)).cast<bool**>();
	if (allow_extrapolation)
		*allow_extrapolation = false;

	return original();
}

void __fastcall add_view_model_bob(void* ecx, void* edx, c_base_entity* model, vec3_t& pos, vec3_t& angles) {
	static auto original = __hooks->get_original(&add_view_model_bob);
	original(ecx, edx, model, pos, angles);
}

void __fastcall calc_viewmodel_view(void* ecx, void* edx, c_cs_player* owner, vec3_t& eye_pos, vec3_t& eye_angles) {
	static auto original = __hooks->get_original(&calc_viewmodel_view);
	original(ecx, edx, owner, eye_pos, eye_angles);
}

void* __fastcall model_renderable_animating(void* ecx, void* edx) {
	static auto original = __hooks->get_original(&model_renderable_animating);

	auto player = (c_cs_player*)((std::uintptr_t)ecx - 4);
	if (!player || player->get_client_class()->class_id != CCSRagdoll)
		return original(ecx, edx);

	// remove ragdolls from fast path list (allows to make chams in DME)
	return nullptr;
}

void __fastcall build_transformations(c_cs_player* ecx, void* edx, c_studio_hdr* hdr, int a3, int a4, int a5, int a6, int a7) {
	static auto original = __hooks->get_original(&build_transformations);
	if (!ecx || !ecx->is_player() || !ecx->is_alive())
		return original(ecx, edx, hdr, a3, a4, a5, a6, a7);

	RESTORE(ecx->jiggle_bones_enabled());
	RESTORE(ecx->use_new_animstate());

#ifndef LEGACY
	RESTORE(*(int*)((std::uintptr_t)ecx + 0x26B0)); // mask ptr

	// don't call clamp bones in bbox when it's not neccesary
	*(int*)((std::uintptr_t)ecx + 0x26B0) = 0;
#endif

	// don't allow game to procees attachments shake (jiggle)
	ecx->jiggle_bones_enabled() = false;

	// don't allow game to lerp bones through body snapshots
	ecx->use_new_animstate() = false;

	original(ecx, edx, hdr, a3, a4, a5, a6, a7);
}

void __fastcall do_extra_bone_processing(c_cs_player* ecx, void* edx, c_studio_hdr* hdr, vec3_t* pos, vec3_t* q, const matrix3x4_t& mat, std::uint8_t* bone_computed, void* context) {
	return;
}

void __fastcall add_renderable(void* ecx, void* edx, c_renderable* renderable, bool render_with_viewmodels, int type, int model_type, int split_screen_enables) {
	static auto original = __hooks->get_original(&add_renderable);

	auto renderable_addr = reinterpret_cast<std::uintptr_t>(renderable);
	if (!renderable_addr || renderable_addr == 0x4)
		return original(ecx, edx, renderable, render_with_viewmodels, type, model_type, split_screen_enables);

	auto entity = reinterpret_cast<c_base_entity*>(renderable_addr - 0x4);
	int index = *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(entity) + 0x64);

	if (index < 1 || index > 64)
		return original(ecx, edx, renderable, render_with_viewmodels, type, model_type, split_screen_enables);

	// set advanced transparency type for fixing z order (chams renders behind props)
	if (index == __interfaces->engine->get_local_player())
		type = 1;
	else
		type = 2;

	original(ecx, edx, renderable, render_with_viewmodels, type, model_type, split_screen_enables);
}

void __fastcall update_client_side_animation(c_cs_player* ecx, void* edx) {
	static auto original = __hooks->get_original(&update_client_side_animation);

	if (ecx->is_alive() && !ecx->is_teammate(false)) {
		c_feature::exec_update_client_side_animation(ecx);
	}
	else
		return original(ecx, edx);
}

void __fastcall calc_view(c_cs_player* ecx, void* edx, vec3_t& eye_origin, vec3_t& eye_angles, float& near_, float& far_, float& fov) {
	static auto original = __hooks->get_original(&calc_view);
	original(ecx, edx, eye_origin, eye_angles, near_, far_, fov);
}

vec3_t* __fastcall weapon_shootpos(c_cs_player* ecx, void* edx, vec3_t& eye) {
	static auto original = __hooks->get_original(&weapon_shootpos);
	return original(ecx, edx, eye);
}

void __fastcall modify_eye_position(c_animation_state* ecx, void* edx, vec3_t& pos) {
	static auto original = __hooks->get_original(&modify_eye_position);
	original(ecx, edx, pos);
}

bool __fastcall setup_bones(void* ecx, void* edx, matrix3x4_t* bone_to_world, int max_bones, int mask, float time) {
	static auto original = __hooks->get_original(&setup_bones);

	auto player = reinterpret_cast<c_cs_player*>(reinterpret_cast<std::uintptr_t>(ecx) - 4);

	if (__interfaces->client_state->delta_tick == -1 || !player || !player->is_player() || !player->is_alive() || player->is_teammate(false))
		return original(ecx, edx, bone_to_world, max_bones, mask, time);

	if (player != __local_player) {
		auto animation_info = __animation_fix->get_animation_info(player->index());
		if (animation_info->old_spawn_time != player->spawn_time() || !animation_info->came_from_dormant)
			return original(ecx, edx, bone_to_world, max_bones, mask, time);
	}

	if (bone_to_world)
		math::memcpy_sse(bone_to_world, player->bone_cache().base(), sizeof(matrix3x4_t) * player->bone_cache().count());

	return true;
}

bool __fastcall want_reticle_shown(void* ecx, void* edx) {
	static auto original = __hooks->get_original(&want_reticle_shown);
	return original(ecx, edx);
}

void __fastcall engine_paint(void* ecx, void* edx, const int mode) {
	static auto original = __hooks->get_original(&engine_paint);
	original(ecx, edx, mode);
}

void __fastcall interpolate_server_entities(void* ecx, void* edx) {
	static auto original = __hooks->get_original(&interpolate_server_entities);
	original(ecx, edx);
}

#ifndef LEGACY
bool __fastcall trace_filter_to_head_collision(void* ecx, void* edx, int collision_group, int contents_mask) {
	static auto original = __hooks->get_original(&trace_filter_to_head_collision);
	static auto player_movement = XORN(COLLISION_GROUP_PLAYER_MOVEMENT);

	return original(ecx, edx, collision_group, contents_mask);
}

void __fastcall on_bbox_change_callback(c_cs_player* ecx, void* edx, vec3_t* old_mins, vec3_t* new_mins, vec3_t* old_maxs, vec3_t* new_maxs) {
	auto& base_origin = ecx == __local_player ? ecx->get_abs_origin() : ecx->origin();

	ecx->collision_change_origin() = base_origin.z + old_maxs->z;
	ecx->collision_change_time() = TICKS_TO_TIME(__game_info->predicted_time);
}

void __fastcall check_moving_ground(void* ecx, void* edx, c_cs_player* player, double frametime) {
	static auto original = __hooks->get_original(&check_moving_ground);
	original(ecx, edx, player, frametime);
}

void __fastcall reset_latched(c_cs_player* ecx, void* edx) {
	static auto original = __hooks->get_original(&reset_latched);
	return original(ecx, edx);
}

void __fastcall clamp_bones_in_bbox(c_cs_player* ecx, void* edx, matrix3x4_t* bones, int mask) {
	static auto original = __hooks->get_original(&clamp_bones_in_bbox);
	return original(ecx, edx, bones, mask);
}

// force game to use a default m_vecVelocity instead of m_vecAbsVelocity
// that also corrected in animatino fix
void __fastcall estimate_abs_velocity(c_base_entity* ecx, void* edx, vec3_t& vel) {
	static auto original = __hooks->get_original(&estimate_abs_velocity);

	if (ecx && ecx->is_player()) {
		auto player = reinterpret_cast<c_cs_player*>(ecx);
		vel = player->velocity();
	}
	else
		return original(ecx, edx, vel);
}

// server simulation time recieves wrong and not rounded to 100 ticks (as it should)
// the best fix will be compare if networked animations were updated, wrong simtime recieved & you try to update at the same server tick
void __fastcall post_data_update(c_networkable* ecx, void* edx, int update_type) {
	static auto original = __hooks->get_original(&post_data_update);

	auto entity = ecx->get_n_unknown_entity()->get_base_entity();
	if (!entity || !entity->is_player())
		return original(ecx, edx, update_type);

	auto player = reinterpret_cast<c_cs_player*>(entity);
	if (!player || !player->is_alive() || player->is_teammate() || player->dormant())
		return original(ecx, edx, update_type);

	auto index = ecx->index();
	auto animation_info = __animation_fix->get_animation_info(index);

	if (animation_info) {
		auto& alive_loop = player->animlayers()[ANIMATION_LAYER_ALIVELOOP];

		if (player->sim_time() == animation_info->old_simulation_time) {
			if (alive_loop.cycle == animation_info->alive_loop_cycle)
				player->sim_time() = player->old_sim_time();
		}
		else {
			auto server_tick = __interfaces->client_state->clock_drift_mgr.server_tick;
			auto network_time_base = __interfaces->global_vars->get_network_base(server_tick, index);
			auto old_network_time_base = __interfaces->global_vars->get_network_base(animation_info->old_server_tick, index);

			auto save_server_tick = (TIME_TO_TICKS(player->sim_time()) % 100)
				|| alive_loop.cycle != animation_info->alive_loop_cycle
				|| network_time_base == old_network_time_base;

			if (save_server_tick)
				animation_info->old_server_tick = server_tick;
			else {
				animation_info->old_simulation_time = player->sim_time();
				player->sim_time() = player->old_sim_time();
			}
		}

		original(ecx, edx, update_type);
	}
	else
		return original(ecx, edx, update_type);
}

// stop to update animstate move weight & cycle
// required for better animation sync with custom fixed velocity values
void __fastcall notify_on_layer_change_weight(c_cs_player* ecx, void* edx, int layer, int weight) {
	return;
}

void __fastcall notify_on_layer_change_cycle(c_cs_player* ecx, void* edx, int layer, int cycle) {
	return;
}

// prevent bone snapshots from update when player change weapon
void __fastcall notify_on_layer_change_sequence(c_cs_player* ecx, void* edx, int layer, int sequence) {
	return;
}

void __fastcall accumulate_layers(c_cs_player* ecx, void* edx, bone_setup_t& bone_setup, vec3_t pos[], quaternion_t q[], float curtime) {
	static auto original = __hooks->get_original(&accumulate_layers);

	RESTORE(ecx->lod_flags());

	ecx->lod_flags().remove(0xA);
	original(ecx, edx, bone_setup, pos, q, curtime);
}

// just don't ask me what it does & where i got it
// it's just works and adjust simtime bounds to correct lerp on exploiters
void __fastcall on_latch_interpolated_variables(c_cs_player* ecx, void* edx, int type) {
	static auto original = __hooks->get_original(&on_latch_interpolated_variables);

	auto bits_type = bits_t{ type };
	if (ecx->predictable() || ecx == __local_player) {
		original(ecx, edx, type);
		return;
	}

	if (!ecx->is_player() || !bits_type.has(LATCH_SIMULATION_VAR))
		return original(ecx, edx, type);

	static auto cl_interp = __convars->get(XOR("cl_interp"));
	static auto sv_clockcorrection_msecs = __convars->get(XOR("sv_clockcorrection_msecs"));

	auto animation = __animation_fix->get_animation_info(ecx->index());

	const auto clock_correction = sv_clockcorrection_msecs->get_float() / 1000.f;
	const auto interp = cl_interp->get_float();

	{
		RESTORE(ecx->sim_time());

		ecx->sim_time() = std::clamp(ecx->sim_time() + animation->dt_interpolation_fraction,
			__interfaces->global_vars->curtime - clock_correction + interp,
			__interfaces->global_vars->curtime + clock_correction + interp);

		original(ecx, edx, type);
	}

	animation->dt_interpolation_fraction = __interfaces->global_vars->curtime - ecx->sim_time() + interp;	
}

INLINE vec3_t get_chase_cam_offset(c_cs_player* player) {
	auto view_vectors = __interfaces->game_rules->get_view_vectors();

	if (player && player->is_alive()) {
		if (player->flags().has(FL_DUCKING))
			return view_vectors->duck_view;

		return view_vectors->view;
	}

	return view_vectors->dead_view_height;
}

// purpose: change in-spectator distance
// AS NEVERLOSE U*#%T&*((*#@YUT
void __fastcall calc_chase_cam_view(c_cs_player* ecx, void* edx, vec3_t& eye_origin, vec3_t& eye_angles, float& fov) {
	static auto original = __hooks->get_original(&calc_chase_cam_view);

	if (ecx != __local_player)
		return original(ecx, edx, eye_origin, eye_angles, fov);

	auto target = ecx->observer_target().get().cast<c_cs_player*>();
	if (!target) {
		eye_origin = ecx->origin() + ecx->view_offset();
		eye_angles = ecx->eye_angles();
		return;
	}

	auto animating = reinterpret_cast<c_base_entity*>(reinterpret_cast<std::uintptr_t>(target) + 4);
	if (!animating || !target->get_model()) {
		FIND_OFFSET(__modules->client, OFFSET_CALC_ROAMING_VIEW).cast<void(__thiscall*)(void*, vec3_t&, vec3_t&, float&)>()(ecx, eye_origin, eye_angles, fov);
		return;
	}

	vec3_t forward{}, view_point{};
	auto origin = target->get_render_origin() + get_chase_cam_offset(target);

	vec3_t view_angles{};
	if (ecx->observer_mode() == OBS_MODE_IN_EYE)
		view_angles = eye_angles;
	else if (ecx == __local_player)
		__interfaces->engine->get_view_angles(view_angles);
	else
		view_angles = ecx->eye_angles();

	view_angles.z = 0.f;

	// change it later, okay?
	ecx->observer_chase_distance() = 120.f;

	math::angle_vectors(view_angles, forward);
	forward = forward.normalized();

	view_point = origin + forward * (-ecx->observer_chase_distance());

	c_game_trace trace{};
	c_trace_filter_no_npcs_or_player filter{};
	filter.skip = target;

	__interfaces->engine_trace->trace_ray({ origin, view_point, {-6.f, -6.f, -6.f}, {6.f, 6.f, 6.f} }, MASK_SOLID, &filter, &trace);

	if (trace.fraction < 1.f) {
		view_point = trace.end;
		ecx->observer_chase_distance() = (origin - eye_origin).length();
	}

	eye_angles = view_angles;
	eye_origin = view_point;
}
#endif

void c_hooks::init_detour_hooks() {
	ADD_DETOUR_RELATIVE(model_renderable_animating, client, OFFSET_MODEL_RENDERABLE_ANIMATING);

	ADD_DETOUR(get_exposure_range, client, OFFSET_GET_EXPOSURE_RANGE);
	ADD_DETOUR(calc_view_model_bob, client, OFFSET_CALC_VIEW_MODEL_BOB);
	ADD_DETOUR(physics_simulate, client, OFFSET_PHYSICS_SIMULATE);
	ADD_DETOUR(update_postscreen_effects, client, OFFSET_UPDATE_POSTSCREEN_EFFECTS);
	ADD_DETOUR(draw_fog, client, OFFSET_SHOULD_DRAW_FOG);
	ADD_DETOUR(process_interpolated_list, client, OFFSET_PROCESS_INTERPOLATED_LIST);
	ADD_DETOUR(calc_viewmodel_view, client, OFFSET_CALC_VIEWMODEL_VIEW);
	ADD_DETOUR(build_transformations, client, OFFSET_BUILD_TRANSFORMATIONS);
	ADD_DETOUR(do_extra_bone_processing, client, OFFSET_DO_EXTRA_BONE_PROCESSING);
	ADD_DETOUR(add_renderable, client, OFFSET_ADD_RENDERABLE);
	ADD_DETOUR(update_client_side_animation, client, OFFSET_UPDATE_CLIENT_SIDE_ANIMATION);
	ADD_DETOUR(calc_view, client, OFFSET_CALC_VIEW);
	ADD_DETOUR(modify_eye_position, client, OFFSET_MODIFY_EYE_POSITION);
	ADD_DETOUR(setup_bones, client, OFFSET_SETUP_BONES);
	ADD_DETOUR(want_reticle_shown, client, OFFSET_WANT_RETICLE_SHOWN);

	ADD_DETOUR(send_datagram, engine, OFFSET_SEND_DATAGRAM);
	ADD_DETOUR(send_net_msg, engine, OFFSET_SEND_NET_MSG);
	ADD_DETOUR(msg_voice_data, engine, OFFSET_MSG_VOICE_DATA);
	ADD_DETOUR(read_packets, engine, OFFSET_READ_PACKETS);
	ADD_DETOUR(cl_move, engine, OFFSET_CL_MOVE);
	ADD_DETOUR(process_packet, engine, OFFSET_PROCESS_PACKET);
	ADD_DETOUR(engine_paint, engine, OFFSET_ENGINE_PAINT);

	ADD_DETOUR(get_color_modulation, materialsystem, OFFSET_GET_COLOR_MODULATION);

#ifndef LEGACY
	ADD_DETOUR_RELATIVE(clamp_bones_in_bbox, client, OFFSET_CLAMP_BONES_IN_BBOX);

	ADD_DETOUR(notify_on_layer_change_weight, client, OFFSET_NOTIFY_ON_LAYER_CHANGE_WEIGHT);
	ADD_DETOUR(notify_on_layer_change_sequence, client, OFFSET_NOTIFY_ON_LAYER_CHANGE_SEQUENCE);
	ADD_DETOUR(notify_on_layer_change_cycle, client, OFFSET_NOTIFY_ON_LAYER_CHANGE_CYCLE);
	ADD_DETOUR(accumulate_layers, client, OFFSET_ACCUMULATE_LAYERS);
	ADD_DETOUR(trace_filter_to_head_collision, client, OFFSET_TRACE_FILTER_TO_HEAD_COLLISION);
	ADD_DETOUR(on_bbox_change_callback, client, OFFSET_ON_BBOX_CHANGE_CALLBACK);
	ADD_DETOUR(reset_latched, client, OFFSET_RESET_LATCHED);
	ADD_DETOUR(estimate_abs_velocity, client, OFFSET_ESTIMATE_ABS_VELOCITY);
	ADD_DETOUR(post_data_update, client, OFFSET_POST_DATA_UPDATE);
	ADD_DETOUR(on_latch_interpolated_variables, client, OFFSET_ON_LATCH_INTERPOLATED_VARIABLES);
	ADD_DETOUR(calc_chase_cam_view, client, OFFSET_CALC_CHASE_CAM_VIEW);
#endif
}