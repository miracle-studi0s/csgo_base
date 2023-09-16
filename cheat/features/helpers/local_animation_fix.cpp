#include "../../callbacks.hpp"
#include "../../main_definitions.hpp"
#include "../../security/security.hpp"
#include "../../sdk/sdk.hpp"

#include "game_information.hpp"
#include "threaded_animstate.hpp"
#include "server_bones.hpp"
#include "engine_prediction.hpp"
#include "local_animation_fix.hpp"

uint8_t* get_server_edict(c_cs_player* player) {
	static uintptr_t server_globals = **FIND_OFFSET(__modules->server, OFFSET_GET_SERVER_EDICT).add(0x2).cast< uintptr_t** >();
	int max_clients = *(int*)((uintptr_t)server_globals + 0x18);
	int index = player->index();
	if (index > 0 && max_clients >= 1) {
		if (index <= max_clients) {
			int v10 = index * 16;
			uintptr_t v11 = *(uintptr_t*)(server_globals + 96);
			if (v11) {
				if (!((*(uintptr_t*)(v11 + v10) >> 1) & 1)) {
					uintptr_t v12 = *(uintptr_t*)(v10 + v11 + 12);
					if (v12)
					{
						uint8_t* ret_ptr = nullptr;
						__asm
						{
							pushad
							mov ecx, v12
							mov eax, dword ptr[ecx]
							call dword ptr[eax + 0x14]
							mov ret_ptr, eax
							popad
						}

						return ret_ptr;
					}
				}
			}
		}
	}
	return nullptr;
}

void draw_server_hitbox(c_cs_player* player) {
	auto duration = __interfaces->global_vars->interval_per_tick * 2.f;

	auto server_player = get_server_edict(player);
	if (server_player) {
		static auto call___ptr = FIND_OFFSET(__modules->server, OFFSET_DRAW_SERVER_HITBOX).cast< uintptr_t* >();

		float current_duration = duration;

		__asm
		{
			pushad
			movss xmm1, current_duration
			push 1
			mov ecx, server_player
			call call___ptr
			popad
		}
	}
}

// due to valve are retards & play animation events only on server
// for correct animation sync we should play animation events by yourself
void c_local_animation_fix::handle_jump_animations(c_animation_state* state, c_animation_layers* layers, c_user_cmd* cmd) {
	auto land_or_climb = &layers[ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB];
	auto jump_or_fall = &layers[ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL];

	auto ground_entity_handle = __local_player->ground_entity();
	auto ground_entity = ground_entity_handle.get().cast<c_base_entity*>();

	auto old_on_ladder = old_move_type == MOVETYPE_LADDER;
	auto on_ladder = __local_player->move_type() == MOVETYPE_LADDER;

	auto& flags = __local_player->flags();

	auto landed = flags.has(FL_ONGROUND) && !old_flags.has(FL_ONGROUND);
	auto jumped = !flags.has(FL_ONGROUND) && old_flags.has(FL_ONGROUND);

	auto entered_ladder = on_ladder && !old_on_ladder;
	auto left_ladder = !on_ladder && old_on_ladder;

	old_flags = flags;
	old_move_type = __local_player->move_type();

	if (entered_ladder)
		state->set_layer_sequence(land_or_climb, state->select_sequence_from_activity_modifier(ACT_CSGO_CLIMB_LADDER), ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL);

	if (cmd->buttons.has(IN_JUMP) && ground_entity)
		state->set_layer_sequence(jump_or_fall, state->select_sequence_from_activity_modifier(ACT_CSGO_JUMP), ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL);

	if (flags.has(FL_ONGROUND)) {
		if (!landing && landed) {
			int activity = state->duration_in_air > 1.f ? ACT_CSGO_LAND_HEAVY : ACT_CSGO_LAND_LIGHT;
			state->set_layer_sequence(land_or_climb, state->select_sequence_from_activity_modifier(activity), ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB);

			landing = true;
		}
	}
	else {
		if (!cmd->buttons.has(IN_JUMP) && (jumped || left_ladder))
			state->set_layer_sequence(jump_or_fall, state->select_sequence_from_activity_modifier(ACT_CSGO_FALL), ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL);

		landing = false;
	}
}

// i should handle even strafe values...
void c_local_animation_fix::handle_strafing(c_animation_state* state, c_user_cmd* cmd) {
	auto buttons = cmd->buttons;

	vec3_t forward{};
	vec3_t right{};
	vec3_t up{};

	math::angle_vectors(vec3_t(0, state->abs_yaw, 0), forward, right, up);
	right = right.normalized();

	auto velocity = state->velocity_normalized_non_zero;
	auto speed = state->speed_as_portion_of_walk_top_speed;

	float vel_to_right_dot = velocity.dot(right);
	float vel_to_foward_dot = velocity.dot(forward);

	bool move_right = (buttons.has(IN_MOVERIGHT)) != 0;
	bool move_left = (buttons.has(IN_MOVELEFT)) != 0;
	bool move_forward = (buttons.has(IN_FORWARD)) != 0;
	bool move_backward = (buttons.has(IN_BACK)) != 0;

	bool strafe_right = (speed >= 0.73f && move_right && !move_left && vel_to_right_dot < -0.63f);
	bool strafe_left = (speed >= 0.73f && move_left && !move_right && vel_to_right_dot > 0.63f);
	bool strafe_forward = (speed >= 0.65f && move_forward && !move_backward && vel_to_foward_dot < -0.55f);
	bool strafe_backward = (speed >= 0.65f && move_backward && !move_forward && vel_to_foward_dot > 0.55f);

	__local_player->strafing() = (strafe_right || strafe_left || strafe_forward || strafe_backward);
}

void c_local_animation_fix::predict(c_user_cmd* cmd) {
	if (!__local_player->is_alive())
		return;

	*__game_info->send_packet = __interfaces->client_state->choked_commands >= 14;

	draw_server_hitbox(__local_player);

	auto state = __local_player->animstate();
	if (!state)
		return;

	if (old_spawn_time != __local_player->spawn_time()) {
		state->reset();
		old_spawn_time = __local_player->spawn_time();
		return;
	}

	if (!*__game_info->send_packet)
		return;

	__local_player->store_layers(backup_layers);

	// force variables from previous update 
	state->primary_cycle = updated_state.primary_cycle;
	state->move_weight = updated_state.move_weight;

	state->strafe_sequence = updated_state.strafe_sequence;
	state->strafe_change_weight = updated_state.strafe_change_weight;
	state->strafe_change_cycle = updated_state.strafe_change_cycle;
	state->acceleration_weight = updated_state.acceleration_weight;

	RESTORE(__local_player->render_angles());
	RESTORE(__local_player->eye_angles());

	// update all cmds from previous outgoing to current
	for (int i = 0; i <= __interfaces->client_state->choked_commands + 1; ++i) {
		old_networked_vars.store(cmd);

		auto cmd_index = __interfaces->client_state->last_outgoing_command + i;
		auto cmd = __interfaces->input->get_user_cmd(cmd_index % 150);

		// apply all information for animstate from this command
		// to get correct positions for animation calculations
		auto prediction_data = __engine_prediction->get_networked_vars(cmd_index);
		prediction_data->restore(true);

		// and, of course, apply angle from this command
		__local_player->render_angles() = __local_player->eye_angles() = cmd->viewangles;

		RESTORE(__local_player->cmd_context());
		__local_player->cmd_context().cmd_number = cmd->command_number;

		// apply updated layers from last update
		auto layers = __local_player->animlayers();
		layers[ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL].copy_to(fresh_layers[ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL]);
		layers[ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB].copy_to(fresh_layers[ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB]);

		// force owner for important in-game functions
		for (int i = 0; i < 13; i++) {
			auto layer = &layers[i];
			layer->owner = __local_player;
			layer->studio_hdr = __local_player->get_studio_hdr();
		}

		handle_jump_animations(state, __local_player->animlayers(), cmd);
		handle_strafing(state, cmd);

		RESTORE(__local_player->client_side_animation());
		__local_player->client_side_animation() = true;

		auto& angles = __local_player->eye_angles();
		__threaded_animstate->update(state, angles.y, angles.x, TICKS_TO_TIME(prediction_data->tickbase), prediction_data->tickbase);

		// force update interpolated variables
		if (*reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(__local_player) + 0x28C0) != -1) {
			__memory->get_virtual(__local_player, XORN(108)).cast<void(__thiscall*)(void*, int)>()(__local_player, 1);
		}

		__local_player->store_layers(fresh_layers);

		old_networked_vars.restore(true);
		__local_player->set_abs_origin(old_networked_vars.abs_origin);
	}

	// store blending data for bone matrix
	real_blending.store(__local_player);

	math::memcpy_sse(&updated_state, state, sizeof(updated_state));
	__local_player->set_layers(backup_layers);
}

void c_local_animation_fix::update_client_side_animation(c_cs_player* player) {
	if (player != __local_player)
		return;

	if (old_spawn_time != __local_player->spawn_time())
		return;

	auto state = __local_player->animstate();
	if (!state)
		return;

	restore_blending.store(player); {
		RESTORE(player->render_angles());
		RESTORE(player->eye_angles());

		real_blending.restore(player);

		player->set_abs_angles({ 0.f, state->abs_yaw, 0.f });
		real_matrix.builder.store(__local_player, real_matrix.matrix, 0x7FF00);
		real_matrix.builder.setup();

		player->set_bone_cache(real_matrix.matrix);
		player->attachments_helper();
		player->interpolate_moveparent_pos();
	}
	restore_blending.restore(player);
}

void c_local_animation_fix::map_init_post() {
	reset_all();
}

void c_local_animation_fix::map_shutdown() {
	reset_all();
}

void c_local_animation_fix::tick_local_death() {
	if (__local_player->is_alive())
		return;

	reset_all();
}