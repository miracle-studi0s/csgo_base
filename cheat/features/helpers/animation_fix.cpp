#include "../../callbacks.hpp"
#include "../../main_definitions.hpp"
#include "../../security/security.hpp"
#include "../../sdk/sdk.hpp"
#include "threaded_animstate.hpp"
#include "game_information.hpp"
#include "animation_fix.hpp"
#include "backtracking.hpp"

//uint8_t* get_server_edict(c_cs_player* player) {
//	static uintptr_t server_globals = **FIND_OFFSET(__modules->server, OFFSET_GET_SERVER_EDICT).add(0x2).cast< uintptr_t** >();
//	int max_clients = *(int*)((uintptr_t)server_globals + 0x18);
//	int index = player->index();
//	if (index > 0 && max_clients >= 1) {
//		if (index <= max_clients) {
//			int v10 = index * 16;
//			uintptr_t v11 = *(uintptr_t*)(server_globals + 96);
//			if (v11) {
//				if (!((*(uintptr_t*)(v11 + v10) >> 1) & 1)) {
//					uintptr_t v12 = *(uintptr_t*)(v10 + v11 + 12);
//					if (v12)
//					{
//						uint8_t* ret_ptr = nullptr;
//						__asm
//						{
//							pushad
//							mov ecx, v12
//							mov eax, dword ptr[ecx]
//							call dword ptr[eax + 0x14]
//							mov ret_ptr, eax
//							popad
//						}
//
//						return ret_ptr;
//					}
//				}
//			}
//		}
//	}
//	return nullptr;
//}

//void draw_server_hitbox(c_cs_player* player) {
//	auto duration = __interfaces->global_vars->interval_per_tick;
//
//	auto server_player = get_server_edict(player);
//	if (server_player) {
//		static auto call___ptr = FIND_OFFSET(__modules->server, OFFSET_DRAW_SERVER_HITBOX).cast< uintptr_t* >();
//
//		float current_duration = duration;
//
//		__asm
//		{
//			pushad
//			movss xmm1, current_duration
//			push 1
//			mov ecx, server_player
//			call call___ptr
//			popad
//		}
//	}
//}

//void draw_hitbox(c_cs_player* player, matrix3x4_t* bones, int idx, int idx2, bool dur = false) {
//	auto studio_model = __interfaces->model_info->get_studio_model(player->get_model());
//	if (!studio_model)
//		return;
//
//	auto hitbox_set = studio_model->hitbox_set(0);
//	if (!hitbox_set)
//		return;
//
//	for (int i = 0; i < hitbox_set->num_hitboxes; i++) {
//		auto hitbox = hitbox_set->hitbox(i);
//		if (!hitbox)
//			continue;
//
//		vec3_t vMin, vMax;
//		math::vector_transform(hitbox->min, bones[hitbox->bone], vMin);
//		math::vector_transform(hitbox->max, bones[hitbox->bone], vMax);
//
//		if (hitbox->radius != -1.f)
//			__interfaces->debug_overlay->add_capsule_overlay(vMin, vMax, hitbox->radius, 255, 255 * idx, 255 * idx2, 150, dur ? __interfaces->global_vars->interval_per_tick : 0.1f, 0, 1);
//	}
//}

void c_animation_fix::calculate_choked_ticks(c_cs_player* player, animation_info_t* info) {
	auto record = &info->animation_records.front();
	if (info->prev_record) {
		auto layer_alive_loop = &record->side_original.layers[ANIMATION_LAYER_ALIVELOOP];
		auto old_layer_alive_loop = &info->prev_record->side_original.layers[ANIMATION_LAYER_ALIVELOOP];

		auto current_playback_rate = layer_alive_loop->playback_rate;
		auto previous_playback_rate = old_layer_alive_loop->playback_rate;

		auto current_cycle = layer_alive_loop->cycle;
		auto previous_cycle = static_cast<int>(old_layer_alive_loop->cycle / (__interfaces->global_vars->interval_per_tick * previous_playback_rate) + 0.5f);

		auto cycle = 0;

		if (current_playback_rate == previous_playback_rate)
			cycle = static_cast<int>(current_cycle / (current_playback_rate * __interfaces->global_vars->interval_per_tick) + 0.5f);
		else
			cycle = static_cast<int>(previous_cycle + ((current_cycle / current_playback_rate + (1.f - old_layer_alive_loop->cycle) / previous_playback_rate) / __interfaces->global_vars->interval_per_tick + 0.5f));

		auto layer_delta = cycle - previous_cycle;
		if (layer_delta <= 18)
			record->choke_amount = std::max(layer_delta, 1);

		record->choke_amount = std::clamp(record->choke_amount, 0, 16);
	}
}

// pasted from gamesense
// calculate when animation began & in choke cycle you compare this time to possible land timing
void c_animation_fix::calculate_timing_for_jump_and_land(c_cs_player* player, animation_info_t* info, int& jump_time, int& land_time) {
	auto record = &info->animation_records.front();

	// we have information that he is on ground now
	// now let's guess, he is landing?
	if (player->flags().has(FL_ONGROUND)) {
		auto layer_land = &record->side_original.layers[ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB];
		auto old_layer_land = &info->prev_record->side_original.layers[ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB];

		// yes, he possibly landing now
		// because of landing server info was sent to server
		// now calculate start time where he was not landing on choke cycle
		if (!info->prev_record->flags.has(FL_ONGROUND) && layer_land->weight != 0.f && old_layer_land->weight == 0.f) {
			auto sequence_activity = player->get_sequence_activity(layer_land->sequence);
			if (sequence_activity == ACT_CSGO_LAND_LIGHT || sequence_activity == ACT_CSGO_LAND_HEAVY)
				land_time = TIME_TO_TICKS(record->sim_time - (layer_land->cycle / layer_land->playback_rate)) + 1;
		}
	}
	else {
		auto layer_jump = &record->side_original.layers[ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL];
		auto old_layer_jump = &info->prev_record->side_original.layers[ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL];

		// another case - he is jumping now
		// now calculate start time where he was not jumping on choke cycle
		if (layer_jump->cycle != old_layer_jump->cycle
			|| layer_jump->sequence != old_layer_jump->sequence
			&& layer_jump->cycle < old_layer_jump->cycle) {
			auto sequence_activity = player->get_sequence_activity(layer_jump->sequence);
			if (sequence_activity == ACT_CSGO_JUMP)
				jump_time = TIME_TO_TICKS(record->sim_time - (layer_jump->cycle / layer_jump->playback_rate)) + 1;
		}
	}
}

// used for break lc players 
void c_animation_fix::correct_jump_and_land_on_teleport(c_cs_player* player, animation_info_t* info, c_animation_state* state) {
	auto record = &info->animation_records.front();

	// we don't know if enemy was jumping previous tick
	// just force replay jump animation for him
	if (player->flags().has(FL_ONGROUND)) {
		state->on_ground = true;
		state->landing = false;
	}

	auto last_update_time = state->last_update_time;
	auto layer_jump = &record->side_original.layers[ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL];
	if (player->get_sequence_activity(layer_jump->sequence) == ACT_CSGO_JUMP) {
		auto duration_in_air = record->sim_time - layer_jump->cycle / layer_jump->playback_rate;
		if (duration_in_air > last_update_time) {
			state->on_ground = false;
			player->pose_parameter()[6] = 0.f;
			state->duration_in_air = 0.f;
			state->last_update_time = duration_in_air;
		}
	}
}

// this one is used for enemies without fakelags
void c_animation_fix::correct_jump_and_land_on_unchoke(c_cs_player* player, animation_info_t* info, c_animation_state* state) {
	auto record = &info->animation_records.front();
	if (!info->prev_record)
		return;

	// another case - we knew that he was jumping on previous tick
	// now we can check if he changed his direction & update animation time
	if (!player->flags().has(FL_ONGROUND)) {
		auto layer_jump = &record->side_original.layers[ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL];
		auto old_layer_jump = &info->prev_record->side_original.layers[ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL];

		if (layer_jump->weight > 0.f && layer_jump->cycle > 0.f) {
			if (player->get_sequence_activity(layer_jump->sequence) == ACT_CSGO_JUMP) {
				if (layer_jump->cycle != old_layer_jump->cycle || layer_jump->sequence != old_layer_jump->sequence && old_layer_jump->cycle > layer_jump->cycle) {
					player->pose_parameter()[6] = 0.f;
					state->duration_in_air = layer_jump->cycle / layer_jump->playback_rate;
				}
			}
		}
	}
}

void c_animation_fix::correct_velocity(c_cs_player* player, animation_info_t* info, c_animation_state* state) {
	auto weapon = player->active_weapon().get().cast<c_base_combat_weapon*>();
	if (!weapon)
		return;

	auto weapon_info = __interfaces->weapon_system->get_weapon_data(weapon->item_definition_index());

	auto record = &info->animation_records.front();

	// reset velocity on player teleport as server does
	if (player->effects().has(EF_NOINTERP) || player->no_interp_parity() != player->no_interp_parity_old()) {
		record->velocity.reset();
		return;
	}

	auto time_delta = TICKS_TO_TIME(record->choke_amount);

	auto max_speed = weapon && weapon_info ?
		std::max<float>((player->is_scoped() ? weapon_info->max_speed_alt : weapon_info->max_speed), 0.001f)
		: CS_PLAYER_SPEED_RUN;

	if (player->is_walking())
		max_speed *= CS_PLAYER_SPEED_WALK_MODIFIER;

	if (player->duck_amount() >= 1.f)
		max_speed *= CS_PLAYER_SPEED_DUCK_MODIFIER;

	if (info->prev_record) {
		// get velocity based on direction
		if (time_delta > 0.f)
			record->velocity = (record->origin - info->prev_record->origin) / time_delta;

		if (record->flags.has(FL_ONGROUND) && info->prev_record->flags.has(FL_ONGROUND)) {
			auto& layer_aliveloop = info->prev_record->side_original.layers[ANIMATION_LAYER_ALIVELOOP];

			// calculate server value of m_flSpeedAsPortionOfRunTopSpeed via layer from SetupAliveLoop:
			/*
				code from server.dll:
				v27 = 0.0;
				v28 = (*(this + 228) - 0.55000001) * 2.8571432;
				if ( v28 >= 0.0 )
					v27 = fminf(v28, 1.0);
				sub_104B02A0(this, 1.0 - v27, 11);
			*/

			auto base_value = 1.f - layer_aliveloop.weight;
			auto fake_speed_portion = base_value / 2.85f;
			if (fake_speed_portion > 0.f)
				fake_speed_portion += 0.55f;

			// adjust the velocity by server m_flSpeedAsPortionOfRunTopSpeed by inverting in-game formulas
			// code from game:
			/*
				m_flSpeedAsPortionOfRunTopSpeed = clamp( m_flVelocityLengthXY / flMaxSpeedRun, 0, 1 );
				m_flSpeedAsPortionOfWalkTopSpeed = m_flVelocityLengthXY / (flMaxSpeedRun * CS_PLAYER_SPEED_WALK_MODIFIER);
				m_flSpeedAsPortionOfCrouchTopSpeed = m_flVelocityLengthXY / (flMaxSpeedRun * CS_PLAYER_SPEED_DUCK_MODIFIER);
			*/

			auto velocity_length = std::min<float>(record->velocity.length(), CS_PLAYER_SPEED_RUN);
			if (velocity_length > 0.1f && fake_speed_portion > 0.f)
				record->velocity *= ((fake_speed_portion * max_speed) / velocity_length);
		}

		if (record->flags.has(FL_ONGROUND))
			record->velocity.z = 0.f;
	}
	else {
		// with inital record we can't get correct values from aliveloop layer
		// so, use networked movement layer to calculate correct velocity
		if (record->flags.has(FL_ONGROUND)) {
			auto& layer_movement = record->side_original.layers[ANIMATION_LAYER_MOVEMENT_MOVE];

			auto velocity_length = std::min<float>(record->velocity.length(), CS_PLAYER_SPEED_RUN);
			if (velocity_length > 0.1f && layer_movement.weight > 0.f)
				record->velocity *= ((layer_movement.weight * max_speed) / velocity_length);

			record->velocity.z = 0.f;
		}
	}

	record->velocity.reset_invalid();
}

// by using previous calculations, adjust on ground parameter on each choked command
void c_animation_fix::correct_jump_and_land(c_cs_player* player, animation_info_t* info, c_animation_state* state, int& current_command_tick, int& jump_time, int& land_time) {
	auto record = &info->animation_records.front();
	if (current_command_tick == land_time) {
		player->flags().force(FL_ONGROUND);

		auto record_layer_land = &record->side_original.layers[ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB];
		auto layer_land = &player->animlayers()[ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB];
		layer_land->cycle = 0.f;
		layer_land->playback_rate = record_layer_land->playback_rate;
	}

	if (current_command_tick == jump_time - 1)
		player->flags().force(FL_ONGROUND);

	if (current_command_tick == jump_time) {
		player->flags().remove(FL_ONGROUND);

		auto record_layer_jump = &record->side_original.layers[ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL];
		auto layer_jump = &player->animlayers()[ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL];
		layer_jump->cycle = 0.f;
		layer_jump->playback_rate = record_layer_jump->playback_rate;
	}
}

void c_animation_fix::update_client_side_animation(c_cs_player* player, animation_info_t* info, c_animation_state* state) {
	auto animation_time = player->sim_time();
	auto animation_ticks = TIME_TO_TICKS(animation_time);

	{
		RESTORE(player->client_side_animation());

		player->client_side_animation() = true;

		info->update_animations = true;

		auto& angles = player->eye_angles();
		__threaded_animstate->update(state, angles.y, angles.x, animation_time, animation_ticks);

		info->update_animations = false;
	}
}

// correct animations for each side & build correct bones
void c_animation_fix::simulate_animation_side(c_cs_player* player, animation_info_t* info, c_animation_state* state, int side, bool should_update,
	bool teleported, animation_record_t* record, animation_record_t* prev_record) {
	for (int i = 0; i < 13; i++) {
		auto layer = &player->animlayers()[i];
		layer->owner = player;
		layer->studio_hdr = player->get_studio_hdr();
	}

	// fix broken cycle & weight from animations
	// because we stopped networking of them
	if (prev_record) {
		auto layer_move = &prev_record->side_original.layers[ANIMATION_LAYER_MOVEMENT_MOVE];

		state->primary_cycle = layer_move->cycle;
		state->move_weight = layer_move->weight;
	}
	else {
		auto layer_move = &record->side_original.layers[ANIMATION_LAYER_MOVEMENT_MOVE];

		state->primary_cycle = layer_move->cycle;
		state->move_weight = layer_move->weight;

		correct_jump_and_land_on_teleport(player, info, state);

		// force call an animaiton update
		state->last_update_time = record->sim_time - __interfaces->global_vars->interval_per_tick;
	}

	if (should_update) {
		if (!teleported) {
			if (record->choke_amount < 2 || !prev_record) {
				correct_jump_and_land_on_unchoke(player, info, state);

				player->velocity() = record->velocity;

				auto max_rotation = state->get_max_rotation();
				if (side != SIDE_ORIGINAL)
					state->abs_yaw = math::normalize_yaw(player->eye_angles().y + max_rotation * side);

				update_client_side_animation(player, info, state);
			}
			else {
				int land_time{}, jump_time{};
				calculate_timing_for_jump_and_land(player, info, jump_time, land_time);

				auto choke_float = static_cast<float>(record->choke_amount);

				auto simulation_time_tick = TIME_TO_TICKS(record->sim_time);
				auto prev_simulation_time = simulation_time_tick - record->choke_amount;

				for (int i = 0; i <= record->choke_amount; ++i) {
					auto current_command_tick = prev_simulation_time + i;
					auto current_command_time = TICKS_TO_TIME(current_command_tick);

					auto current_float_tick = static_cast<float>(i);
					float lerp_step = current_float_tick / static_cast<float>(record->choke_amount);

					correct_jump_and_land(player, info, state, current_command_tick, land_time, jump_time);

					auto lerp_origin = math::interpolate_inversed(lerp_step, prev_record->origin, record->origin);
					auto lerp_velocity = math::interpolate_inversed(lerp_step, prev_record->velocity, record->velocity);
					auto lerp_duck_amount = math::interpolate_inversed(lerp_step, prev_record->duck_amount, record->duck_amount);

					player->origin() = lerp_origin;
					player->abs_velocity() = player->velocity() = lerp_velocity;
					player->duck_amount() = lerp_duck_amount;

					player->set_abs_origin(player->origin());

					RESTORE(player->sim_time());

					player->sim_time() = current_command_time;

					auto max_rotation = state->get_max_rotation();
					if (side != SIDE_ORIGINAL)
						state->abs_yaw = math::normalize_yaw(player->eye_angles().y + max_rotation * side);

					update_client_side_animation(player, info, state);
				}
			}
		}
		else {
			correct_jump_and_land_on_teleport(player, info, state);

			player->velocity() = record->velocity;

			auto max_rotation = state->get_max_rotation();
			if (side != SIDE_ORIGINAL)
				state->abs_yaw = math::normalize_yaw(player->eye_angles().y + max_rotation * side);

			update_client_side_animation(player, info, state);
		}
	}

	player->invalidate_bone_cache();
	auto side_to_update = get_animation_side(record, side); {
		player->set_layers(record->side_original.layers);

		auto& builder = side_to_update->bone_builder;
		builder.store(player, side_to_update->matrix, 0x7FF00);
		builder.setup();

		//draw_hitbox(player, side_to_update->matrix, side == SIDE_LEFT ? 0 : side == SIDE_ZERO ? 0 : 1, side == SIDE_LEFT ? 1 : side == SIDE_ZERO ? 0 : 1, true);
	}
}

void c_animation_fix::update_animations(c_cs_player* player, animation_info_t* info, c_animation_state* state) {
	auto record = &info->animation_records.front();

	auto teleported = player->effects().has(EF_NOINTERP) || player->no_interp_parity_old() != player->no_interp_parity();
	player->update_weapon_dispatch_layers();

	calculate_choked_ticks(player, info);
	correct_velocity(player, info, state);

	player->e_flags().remove(EFL_DIRTY_ABSVELOCITY);

	bool should_update = true;
	if (info->prev_record) {
		// enemy has tickbase drift & server compensated his simulation time
		// don't update wrong animations cuz cheat think that enemy is fake-lagging due to tickbase drift
		if (info->prev_record->server_tick_estimation >= 2 && record->server_tick_estimation < 0
			|| info->prev_record->server_tick_estimation < 0 && record->server_tick_estimation >= 2)
			should_update = false;
	}

	// build correct safe-point matrices with custom animations
	for (int i = SIDE_LEFT; i < SIDE_ORIGINAL; ++i) {
		simulate_animation_side(player, info, state, i, should_update, teleported, record, info->prev_record);
		math::memcpy_sse(state, &info->old_state, sizeof(info->old_state));
	}

	// finally, build the main matrix
	simulate_animation_side(player, info, state, SIDE_ORIGINAL, should_update, teleported, record, info->prev_record);
	//draw_server_hitbox(player);
}

void c_animation_fix::collect_info(c_cs_player* player) {
	auto state = player->animstate();
	if (!state)
		return;

	auto index = player->index();

	auto& info = animation_info[index];
	auto& animation_records = info.animation_records;

	if (info.player != player) {
		info.clear();

		__threaded_animstate->reset_variables(player->index());

		info.player = player;
		return;
	}

	if (!player->is_alive()) {
		info.clear();

		__threaded_animstate->reset_variables(player->index());
		return;
	}

	if (player->dormant()) {
		info.came_from_dormant = false;

		__threaded_animstate->reset_variables(player->index());

		animation_records.clear();
		return;
	}

	info.came_from_dormant = true;

	if (!info.player)
		return;

	auto& alive_loop = player->animlayers()[ANIMATION_LAYER_ALIVELOOP];
	if (alive_loop.cycle == info.alive_loop_cycle)
		return;

	info.alive_loop_cycle = alive_loop.cycle;

	if (player->sim_time() == player->old_sim_time())
		return;

	if (info.old_spawn_time != player->spawn_time()) {
		info.clear_records();

		player->client_side_animation() = true;

		state->player = player;
		state->reset();

		info.old_spawn_time = player->spawn_time();
		return;
	}

	info.store_from_player();

	if (!animation_records.empty() && animation_records.size() > 1) 
		info.prev_record = &animation_records[1];

	{
		backup_animation_t backup_animation{ player };

		math::memcpy_sse(&info.old_state, state, sizeof(info.old_state));

		update_animations(player, &info, state);

		c_animations::exec_collect(player, &info, &animation_records.front());
	}

	if (animation_records.size() > __game_info->tickrate - 1)
		animation_records.erase(animation_records.end() - 1);
}

#ifdef _DEBUG
void threaded_collect_info(const std::unique_ptr<c_animation_fix>& ptr, c_cs_player* player) {
	ptr->collect_info(player);
}
#else
void threaded_collect_info(c_animation_fix* ptr, c_cs_player* player) {
	ptr->collect_info(player);
}
#endif

void c_animation_fix::frame_post(int stage) {
	if (stage != XORN(FRAME_NET_UPDATE_END))
		return;

	if (!__game_info->connected)
		return;

	if (!__local_player)
		return;

	__entlistener->for_each_player([&](c_cs_player* player) {
#ifdef _DEBUG
		__thread_pool->add_task(threaded_collect_info, std::cref(__animation_fix), player);
#else
		__thread_pool->add_task(threaded_collect_info, __animation_fix, player);
#endif
		}, false);

	__thread_pool->wait_all();

	// call interpolation update for enemies
	// required to fix broken netvars (bbox, etc)
	__entlistener->for_each_player([&](c_cs_player* player) {
		if (*reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(player) + 0x28C0) != -1) {
			__memory->get_virtual(player, XORN(108)).cast<void(__thiscall*)(void*, int)>()(player, 1);
		}
		});
}

void c_animation_fix::map_init_post() {
	erase_all_animations();
}

void c_animation_fix::map_shutdown() {
	erase_all_animations();
}

void c_animation_fix::update_client_side_animation(c_cs_player* player) {
	if (player == __local_player)
		return;

	auto index = player->index();

	auto& info = animation_info[index];

	auto& animation_records = info.animation_records;
	if (animation_records.empty())
		return;

	if (info.old_spawn_time != player->spawn_time())
		return;

	if (!info.came_from_dormant)
		return;

	auto first_record = &animation_records.front();

	auto& side_original = first_record->side_original;
	math::memcpy_sse(info.render_matrix, side_original.matrix, sizeof(info.render_matrix));

	clamp_bones_info_t clamp_info{};
	clamp_info.store(first_record);

	auto& builder = side_original.bone_builder;
	builder.clamp_bones_in_bbox(player, info.render_matrix, builder.mask, builder.time, first_record->eye_angles, clamp_info);

	math::change_bones_position(info.render_matrix, 128, first_record->origin, player->get_render_origin());
	player->set_bone_cache(info.render_matrix);
	player->interpolate_moveparent_pos();
	player->attachments_helper();
}