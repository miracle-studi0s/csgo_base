#include "../../callbacks.hpp"
#include "../../main_definitions.hpp"
#include "../../security/security.hpp"
#include "../../sdk/sdk.hpp"
#include "threaded_animstate.hpp"

using ANIMSTATE_FUNC_FN = void(__thiscall*)(c_animation_state*);

float get_sequence_animtag(c_studio_hdr* hdr, int sequence, int tag) {
	auto sequence_desc = hdr->get_sequence_desc(sequence);
	if (!sequence_desc)
		return 0.f;

	int anim_tags = *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(sequence_desc) + 0xC4);
	if (anim_tags == 0)
		return 0.f;

	for (int i = 0; i < anim_tags; ++i) {
		auto array_index = 12 * i;

		auto anim_tag_ptr = reinterpret_cast<std::uintptr_t>(sequence_desc) + array_index + *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(sequence_desc) + 0xC0);
		auto anim_tag = *reinterpret_cast<int*>(anim_tag_ptr);

		if (anim_tag == -1)
			continue;

		if (anim_tag == 0) {
			auto anim_tag_name = reinterpret_cast<const char*>(anim_tag_ptr + *reinterpret_cast<int*>(anim_tag_ptr + 8));
			auto func = FIND_OFFSET(__modules->client, OFFSET_INDEX_FROM_ANIM_TAG_NAME).cast<int(__thiscall*)(const char*)>();
			anim_tag = func(anim_tag_name);
		}

		if (anim_tag == tag) {
			auto cycle = *reinterpret_cast<float*>(anim_tag_ptr + 4);

			if (cycle >= 0.f && cycle < 1.f)
				return cycle;
		}
	}

	return 0.f;
}

void c_threaded_animstate::map_init_pre() {
	for (int i = 0; i < 65; ++i)
		reset_variables(i);
}

void c_threaded_animstate::map_init_post() {
	for (int i = 0; i < 65; ++i)
		reset_variables(i);
}

void c_threaded_animstate::map_shutdown() {
	for (int i = 0; i < 65; ++i)
		reset_variables(i);
}

void c_threaded_animstate::setup_velocity(c_animation_state* state, float curtime) {
	auto player = reinterpret_cast<c_cs_player*>(state->player);
	auto hdr = player->get_studio_hdr();
	if (!hdr)
		return;

	auto weapon = player->active_weapon().get().cast<c_base_combat_weapon*>();
	if (!weapon)
		return;

	auto weapon_info = __interfaces->weapon_system->get_weapon_data(weapon->item_definition_index());
	auto max_speed = weapon && weapon_info ?
		std::max<float>((player->is_scoped() ? weapon_info->max_speed_alt : weapon_info->max_speed), 0.001f)
		: CS_PLAYER_SPEED_RUN;

	auto abs_velocity = player->velocity();

	state->velocity_length_z = abs_velocity.z;
	abs_velocity.z = 0.f;

	state->player_is_accelerating = (state->velocity_last.length_sqr() < abs_velocity.length_sqr());

	state->velocity = math::approach(abs_velocity, state->velocity, state->last_update_increment * 2000);
	state->velocity_normalized = state->velocity.normalized();

	state->velocity_length_xy = std::min<float>(state->velocity.length(), CS_PLAYER_SPEED_RUN);

	if (state->velocity_length_xy > 0)
		state->velocity_normalized_non_zero = state->velocity_normalized;

	state->speed_as_portion_of_run_top_speed = std::clamp<float>(state->velocity_length_xy / max_speed, 0, 1);
	state->speed_as_portion_of_walk_top_speed = state->velocity_length_xy / (max_speed * CS_PLAYER_SPEED_WALK_MODIFIER);
	state->speed_as_portion_of_crouch_top_speed = state->velocity_length_xy / (max_speed * CS_PLAYER_SPEED_DUCK_MODIFIER);

	if (state->speed_as_portion_of_walk_top_speed >= 1)
		state->static_approach_speed = state->velocity_length_xy;
	else if (state->speed_as_portion_of_walk_top_speed < 0.5f)
		state->static_approach_speed = math::approach(80, state->static_approach_speed, state->last_update_increment * 60);

	bool started_moving_this_frame = false;
	bool stopped_moving_this_frame = false;

	if (state->velocity_length_xy > 0) {
		started_moving_this_frame = (state->duration_moving <= 0);
		state->duration_still = 0;
		state->duration_moving += state->last_update_increment;
	}
	else {
		stopped_moving_this_frame = (state->duration_still <= 0);
		state->duration_moving = 0;
		state->duration_still += state->last_update_increment;
	}

	auto adjust = &player->animlayers()[ANIMATION_LAYER_ADJUST];

	if (!state->adjust_started && stopped_moving_this_frame && state->on_ground && !state->on_ladder && !state->landing && state->stutter_step < 50.f) {
		state->set_layer_sequence(adjust, state->select_sequence_from_activity_modifier(ACT_CSGO_IDLE_ADJUST_STOPPEDMOVING), ANIMATION_LAYER_ADJUST);
		state->adjust_started = true;
	}

	int layer_activity = player->get_sequence_activity(adjust->sequence);
	if (layer_activity == ACT_CSGO_IDLE_ADJUST_STOPPEDMOVING || layer_activity == ACT_CSGO_IDLE_TURN_BALANCEADJUST) {
		if (state->adjust_started && state->speed_as_portion_of_crouch_top_speed <= 0.25f) {
			float previous_weight = adjust->weight;
			state->increment_layer_cycle(adjust, false);
			state->set_layer_weight(adjust, state->get_layer_ideal_weight_from_seq_cycle(adjust, ACT_CSGO_IDLE_TURN_BALANCEADJUST));
			state->set_layer_weight_rate(adjust, previous_weight);

			state->adjust_started = !(state->is_layer_sequence_finished(adjust, state->last_update_increment));
		}
		else {
			state->adjust_started = false;

			float previous_weight = adjust->weight;
			state->set_layer_weight(adjust, math::approach(0, previous_weight, state->last_update_increment * 5.f));
			state->set_layer_weight_rate(adjust, previous_weight);
		}
	}

	state->abs_yaw_last = state->abs_yaw;
	state->abs_yaw = std::clamp<float>(state->abs_yaw, -360, 360);

	auto eye_delta = math::angle_diff(state->eye_yaw, state->abs_yaw);

	auto speed_portion_walk = state->speed_as_portion_of_walk_top_speed;
	auto speed_portion_duck = state->speed_as_portion_of_crouch_top_speed;
	auto transition = state->walk_run_transition;
	auto duck_amount = state->anim_duck_amount;

	auto aim_matrix_width_range = math::interpolate_inversed(std::clamp(speed_portion_walk, 0.f, 1.f), 1.f,
		math::interpolate_inversed(transition, 0.8f, 0.5f));

	if (duck_amount > 0)
		aim_matrix_width_range = math::interpolate_inversed(duck_amount * std::clamp(speed_portion_duck, 0.f, 1.f),
			aim_matrix_width_range, 0.5f);

	auto yaw_max = state->aim_yaw_max * aim_matrix_width_range;
	auto yaw_min = state->aim_yaw_min * aim_matrix_width_range;

	if (eye_delta <= yaw_max) {
		if (yaw_min > eye_delta)
			state->abs_yaw = std::fabs(yaw_min) + state->eye_yaw;
	}
	else
		state->abs_yaw = state->eye_yaw - std::fabs(yaw_max);

	auto& lower_body_realign_timer = this->lower_body_realign_timer[player->index()];
	if (state->on_ground) {
		if (state->velocity_length_xy > 0.1f) {
			state->abs_yaw = math::approach_angle(state->eye_yaw, state->abs_yaw, state->last_update_increment * (30.0f + 20.0f * state->walk_run_transition));

			lower_body_realign_timer = curtime + 0.22f;
			player->lower_body_yaw() = state->eye_yaw;
		}
		else {
			state->abs_yaw = math::approach_angle(player->lower_body_yaw(), state->abs_yaw, state->last_update_increment * 100.f);

			if (curtime > lower_body_realign_timer && std::abs(math::angle_diff(state->abs_yaw, state->eye_yaw)) > 35.0f) {
				lower_body_realign_timer = curtime + 1.1f;

				player->lower_body_yaw() = state->eye_yaw;
			}
		}
	}

	if (state->velocity_length_xy <= 1.f && state->on_ground && !state->on_ladder && !state->landing
		&& state->last_update_increment > 0 && (std::abs(math::angle_diff(state->abs_yaw_last, state->abs_yaw)) / state->last_update_increment) > 120.f) {
		state->set_layer_sequence(adjust, state->select_sequence_from_activity_modifier(ACT_CSGO_IDLE_TURN_BALANCEADJUST), ANIMATION_LAYER_ADJUST);
		state->adjust_started = true;
	}

	if (adjust->weight > 0) {
		state->increment_layer_cycle(adjust, false);
		state->increment_layer_weight(adjust);
	}

	if (state->velocity_length_xy > 0 && state->on_ground) {
		float raw_yaw_ideal = (std::atan2f(-state->velocity[1], -state->velocity[0]) * 180.f / M_PI);
		if (raw_yaw_ideal < 0)
			raw_yaw_ideal += 360;

		state->move_yaw_ideal = math::normalize_yaw(math::angle_diff(raw_yaw_ideal, state->abs_yaw));
	}

	state->move_yaw_current_to_ideal = math::normalize_yaw(math::angle_diff(state->move_yaw_ideal, state->move_yaw));

	auto move = &player->animlayers()[ANIMATION_LAYER_MOVEMENT_MOVE];
	auto strafe_change = &player->animlayers()[ANIMATION_LAYER_MOVEMENT_STRAFECHANGE];

	if (started_moving_this_frame && state->move_weight <= 0.f) {
		state->move_yaw = state->move_yaw_ideal;

		auto move_sequence = move->sequence;
		if (move_sequence != -1) {
			auto sequence_desc = hdr->get_sequence_desc(move->sequence);
			auto anim_tags = *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(sequence_desc) + 0xC4);

			if (anim_tags > 0) {
				if (std::abs(math::angle_diff(state->move_yaw, 180)) <= EIGHT_WAY_WIDTH) //N
					state->primary_cycle = get_sequence_animtag(hdr, move_sequence, ANIMTAG_STARTCYCLE_N);
				else if (std::abs(math::angle_diff(state->move_yaw, 135)) <= EIGHT_WAY_WIDTH) //NE
					state->primary_cycle = get_sequence_animtag(hdr, move_sequence, ANIMTAG_STARTCYCLE_NE);
				else if (std::abs(math::angle_diff(state->move_yaw, 90)) <= EIGHT_WAY_WIDTH) //E
					state->primary_cycle = get_sequence_animtag(hdr, move_sequence, ANIMTAG_STARTCYCLE_E);
				else if (std::abs(math::angle_diff(state->move_yaw, 45)) <= EIGHT_WAY_WIDTH) //SE
					state->primary_cycle = get_sequence_animtag(hdr, move_sequence, ANIMTAG_STARTCYCLE_SE);
				else if (std::abs(math::angle_diff(state->move_yaw, 0)) <= EIGHT_WAY_WIDTH) //S
					state->primary_cycle = get_sequence_animtag(hdr, move_sequence, ANIMTAG_STARTCYCLE_S);
				else if (std::abs(math::angle_diff(state->move_yaw, -45)) <= EIGHT_WAY_WIDTH) //SW
					state->primary_cycle = get_sequence_animtag(hdr, move_sequence, ANIMTAG_STARTCYCLE_SW);
				else if (std::abs(math::angle_diff(state->move_yaw, -90)) <= EIGHT_WAY_WIDTH) //W
					state->primary_cycle = get_sequence_animtag(hdr, move_sequence, ANIMTAG_STARTCYCLE_W);
				else if (std::abs(math::angle_diff(state->move_yaw, -135)) <= EIGHT_WAY_WIDTH) //NW
					state->primary_cycle = get_sequence_animtag(hdr, move_sequence, ANIMTAG_STARTCYCLE_NW);
			}
		}
	}
	else {
		if (strafe_change->weight >= 1)
			state->move_yaw = state->move_yaw_ideal;
		else {
			float move_weight = math::interpolate_inversed(state->anim_duck_amount,
				std::clamp(state->speed_as_portion_of_walk_top_speed, 0.f, 1.f),
				std::clamp(state->speed_as_portion_of_crouch_top_speed, 0.f, 1.f));

			float ratio = state->bias(move_weight, 0.18f) + 0.1f;

			state->move_yaw = math::normalize_yaw(state->move_yaw + (state->move_yaw_current_to_ideal * ratio));
		}
	}

	state->pose_param_mappings[PLAYER_POSE_PARAM_MOVE_YAW].set_value(player, state->move_yaw);

	float aim_yaw = math::angle_diff(state->eye_yaw, state->abs_yaw);
	if (aim_yaw >= 0 && state->aim_yaw_max != 0)
		aim_yaw = (aim_yaw / state->aim_yaw_max) * 60.0f;
	else if (state->aim_yaw_min != 0)
		aim_yaw = (aim_yaw / state->aim_yaw_min) * -60.0f;

	state->pose_param_mappings[PLAYER_POSE_PARAM_BODY_YAW].set_value(player, aim_yaw);

	// we need non-symmetrical arbitrary min/max bounds for vertical aim (pitch) too
	float pitch = math::angle_diff(state->eye_pitch, 0);
	if (pitch > 0)
		pitch = (pitch / state->aim_pitch_max) * 90.f;
	else
		pitch = (pitch / state->aim_pitch_min) * -90.f;

	state->pose_param_mappings[PLAYER_POSE_PARAM_BODY_PITCH].set_value(player, pitch);
	state->pose_param_mappings[PLAYER_POSE_PARAM_SPEED].set_value(player, state->speed_as_portion_of_walk_top_speed);
	state->pose_param_mappings[PLAYER_POSE_PARAM_STAND].set_value(player, 1.0f - (state->anim_duck_amount * state->in_air_smooth_value));
}

void c_threaded_animstate::setup_lean(c_animation_state* state, float curtime) {
	auto player = reinterpret_cast<c_cs_player*>(state->player);
	auto lean = &player->animlayers()[ANIMATION_LAYER_LEAN];
	lean->weight = lean->cycle = 0.f;
}

void c_threaded_animstate::update(c_animation_state* state, float yaw, float pitch, float curtime, int framecount) {
	auto player = reinterpret_cast<c_cs_player*>(state->player);
	if (!player || !player->is_alive())
		return;

	if (!FIND_OFFSET(__modules->client, OFFSET_CACHE_SEQUENCES).cast<bool(__thiscall*)(c_animation_state*)>()(state))
		return;

	pitch = math::normalize_yaw(pitch + player->thirdperson_recoil());

	if (state->last_update_frame == framecount || state->last_update_time == curtime)
		return;

	state->last_update_increment = std::max<float>(0.f, curtime - state->last_update_time);

	state->eye_yaw = math::normalize_yaw(yaw);
	state->eye_pitch = math::normalize_yaw(pitch);
	state->position_current = player->origin();
	state->weapon = player->active_weapon().get().cast<c_base_combat_weapon*>();

	if (state->weapon != state->weapon_last || state->first_run_since_init) {
		for (int i = 0; i < 13; ++i) {
			auto layer = &player->animlayers()[i];
			if (layer) {
				layer->studio_hdr = nullptr;
				layer->dispatch_sequence = -1;
				layer->second_dispatch_sequence = -1;
			}
		}
	}

	state->anim_duck_amount = std::clamp<float>(math::approach(std::clamp<float>(player->duck_amount() + state->duck_additional, 0, 1),
		state->anim_duck_amount, state->last_update_increment * 6.0f), 0, 1);

	__memory->get_virtual(__interfaces->model_cache, XORN(MDL_CACHE_LOCK_VFUNC)).cast<void(__thiscall*)(void*)>()(__interfaces->model_cache);
	{
		__memory->get_virtual(player, XORN(SET_SEQUENCE_VFUNC)).cast<void(__thiscall*)(void*, int)>()(player, 0);

		*reinterpret_cast<float*>(reinterpret_cast<std::uintptr_t>(player) + XORN(0x286)) = 0.f;

		auto& cycle = *(float*)((uintptr_t)player + XORN(0xA134));
		if (cycle != 0.f) {
			cycle = 0;
			player->invalidate_physics_recursive(8);
		}
	}
	__memory->get_virtual(__interfaces->model_cache, XORN(MDL_CACHE_UNLOCK_VFUNC)).cast<void(__thiscall*)(void*)>()(__interfaces->model_cache);

	{
		setup_velocity(state, curtime);

		FIND_OFFSET(__modules->client, OFFSET_SETUP_AIM_MATRIX).cast<ANIMSTATE_FUNC_FN>()(state);
		FIND_OFFSET(__modules->client, OFFSET_SETUP_WEAPON_ACTION).cast<ANIMSTATE_FUNC_FN>()(state);
		FIND_OFFSET(__modules->client, OFFSET_SETUP_MOVEMENT).cast<ANIMSTATE_FUNC_FN>()(state);

		auto alive_loop = &player->animlayers()[ANIMATION_LAYER_ALIVELOOP];
		state->increment_layer_cycle(alive_loop, true);

		FIND_OFFSET(__modules->client, OFFSET_SETUP_WHOLE_BODY_ACTION).cast<ANIMSTATE_FUNC_FN>()(state);

		auto flashed = &player->animlayers()[ANIMATION_LAYER_FLASHED];
		if (flashed->weight > 0) {
			if (flashed->weight_delta_rate < 0.f)
				state->increment_layer_weight(flashed);
		}

		FIND_OFFSET(__modules->client, OFFSET_SETUP_FLINCH).cast<ANIMSTATE_FUNC_FN>()(state);

		setup_lean(state, curtime);
	}

	for (int i = 0; i < 13; ++i) {
		auto layer = &player->animlayers()[i];
		if (layer->sequence == 0) {
			if (layer->owner && layer->weight != 0.f) {
				if (layer->weight == 0.0f)
					player->invalidate_physics_recursive(16);
			}

			layer->weight = 0.f;
		}
	}

	player->set_abs_angles({ 0.f, state->abs_yaw, 0.f });

	state->weapon_last = state->weapon;
	state->position_last = state->position_current;
	state->first_run_since_init = false;
	state->last_update_frame = framecount;
	state->last_update_time = curtime;
}