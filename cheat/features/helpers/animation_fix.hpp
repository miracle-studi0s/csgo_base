#pragma once

#include <deque>
#include "server_bones.hpp"

enum animation_sides_t : int {
	SIDE_LEFT = -1,
	SIDE_ZERO,
	SIDE_RIGHT,
	SIDE_ORIGINAL,
};

enum record_info_t : unsigned int {
	RECORD_VALID = 1 << 0,
	RECORD_TICKBASE_DRIFT = 1 << 1,
	RECORD_SHOULD_EXTRAPOLATE = 1 << 2,
};

struct backup_animation_t {
	c_cs_player* backup_player{};

	bool strafing{};
	bool walking{};

	bits_t flags{};
	bits_t e_flags{};
	bits_t effects{};

	float duck_amount{};
	float primary_cycle{};
	float move_weight{};

	vec3_t velocity{};
	vec3_t origin{};
	vec3_t abs_origin{};

	c_animation_layers layers[13]{};

	INLINE backup_animation_t(c_cs_player* player) : backup_player(player) {
		auto state = backup_player->animstate();
		if (state) {
			primary_cycle = state->primary_cycle;
			move_weight = state->move_weight;
		}

		strafing = backup_player->strafing();
		walking = backup_player->is_walking();
		flags = backup_player->flags();
		e_flags = backup_player->e_flags();
		effects = backup_player->effects();

		duck_amount = backup_player->duck_amount();

		velocity = backup_player->velocity();
		origin = backup_player->origin();

		backup_player->store_layers(layers);
	}

	INLINE ~backup_animation_t() {
		auto state = backup_player->animstate();
		if (state) {
			state->primary_cycle = std::move(primary_cycle);
			state->move_weight = std::move(move_weight);
		}

		backup_player->strafing() = std::move(strafing);
		backup_player->is_walking() = std::move(walking);
		backup_player->flags() = std::move(flags);
		backup_player->e_flags() = std::move(e_flags);
		backup_player->effects() = std::move(effects);

		backup_player->duck_amount() = std::move(duck_amount);
		backup_player->velocity() = std::move(velocity);
		backup_player->origin() = std::move(origin);

		backup_player->set_layers(layers);
		backup_player = nullptr;
	}
};

struct animation_side_t {
	c_bone_builder bone_builder{};
	c_animation_layers layers[13]{};

	alignas(16) matrix3x4_t matrix[128]{};
	alignas(16) matrix3x4_t roll_matrix[128]{};

	INLINE void reset() {
		bone_builder.reset();

		for (auto& i : layers)
			i = {};

		for (auto& i : matrix)
			i = {};

		for (auto& i : roll_matrix)
			i = {};
	}
};

struct animation_record_t {
	bool updated = false;
	bool strafing{};
	bool walking{};

	bits_t information{};
	bits_t flags{};
	int choke_amount{};
	int server_tick_estimation{};
	float duck_amount{};
	float sim_time{};
	float collision_change_time{};
	float collision_change_origin{};

	vec3_t velocity{};
	vec3_t origin{};
	vec3_t maxs{};
	vec3_t mins{};
	vec3_t view_offset{};
	vec3_t eye_angles{};

	animation_side_t side_left{};
	animation_side_t side_right{};
	animation_side_t side_zero{};
	animation_side_t side_original{};

	c_base_entity* ground_entity{};

	INLINE void reset() {
		updated = false;

		strafing = false;
		walking = false;

		information = 0;
		flags = 0;
		choke_amount = 0;
		server_tick_estimation = 0;
		duck_amount = 0.f;
		sim_time = 0.f;
		collision_change_time = 0.f;
		collision_change_origin = 0.f;

		velocity.reset();
		origin.reset();
		view_offset.reset();
		maxs.reset();
		mins.reset();
		eye_angles.reset();

		side_left.reset();
		side_right.reset();
		side_zero.reset();
		side_original.reset();

		ground_entity = nullptr;
	}
};

struct animation_info_t {
	c_cs_player* player{};

	animation_record_t* prev_record{};
	animation_record_t* old_record{};

	bool came_from_dormant{};
	bool build_bones{};
	bool clamp_bones_to_bbox{};
	bool update_animations{};

	int old_server_tick{};

	float alive_loop_cycle{};
	float alive_loop_rate{};
	float old_simulation_time{};
	float shifted_simulation_time{};
	float old_spawn_time{};
	float dt_interpolation_fraction{};

	c_animation_state old_state{};
	std::deque<animation_record_t> animation_records{};

	matrix3x4_t render_matrix[128]{};

	INLINE bool updated(c_cs_player* player) {
		if (old_spawn_time != player->spawn_time())
			return false;

		if (!came_from_dormant)
			return false;

		if (animation_records.empty())
			return false;

		return true;
	}

	INLINE void store_from_player() {
		auto collideable = player->get_collideable();
		if (!collideable)
			return;

		auto simulation_tick = TIME_TO_TICKS(player->sim_time());
		auto old_simulation_tick = TIME_TO_TICKS(player->old_sim_time());

		auto& record = animation_records.emplace_front();
		record.strafing = player->strafing();
		record.walking = player->is_walking();

		record.flags = player->flags();
		record.choke_amount = std::clamp(simulation_tick - old_simulation_tick, 0, 16);

		record.server_tick_estimation = simulation_tick - __interfaces->client_state->clock_drift_mgr.server_tick;

		record.duck_amount = player->duck_amount();
		record.sim_time = player->sim_time();

		record.velocity = player->velocity();
		record.origin = player->origin();

		record.maxs = collideable->get_maxs();
		record.mins = collideable->get_mins();

		record.collision_change_time = player->collision_change_time();
		record.collision_change_origin = player->collision_change_origin();

		record.view_offset = player->view_offset();
		record.eye_angles = player->eye_angles();

		record.ground_entity = player->ground_entity().get().cast<c_base_entity*>();

		player->store_layers(record.side_original.layers);
	}

	INLINE void clear_records() {
		prev_record = nullptr;
		old_record = nullptr;

		animation_records.clear();
	}

	INLINE void clear() {
		player = nullptr;
		prev_record = nullptr;
		old_record = nullptr;

		came_from_dormant = false;
		build_bones = false;
		clamp_bones_to_bbox = false;
		update_animations = false;

		old_server_tick = 0;

		alive_loop_cycle = 0.f;
		alive_loop_rate = 0.f;
		old_simulation_time = 0.f;
		shifted_simulation_time = 0.f;
		old_spawn_time = 0.f;
		dt_interpolation_fraction = 0.f;

		old_state = {};
		animation_records.clear();

		for (auto& i : render_matrix)
			i = {};
	}
};

class c_animation_fix : public c_feature {
private:
	animation_info_t animation_info[65]{};

	INLINE animation_side_t* get_animation_side(animation_record_t* record, int side) {
		switch (side) {
		case SIDE_LEFT:
			return &record->side_left;
		case SIDE_RIGHT:
			return &record->side_right;
		case SIDE_ZERO:
			return &record->side_zero;
		case SIDE_ORIGINAL:
			return &record->side_original;
		}

		return nullptr;
	}

	INLINE void erase_all_animations() {
		for (auto& i : animation_info)
			i.clear();
	}

	void calculate_choked_ticks(c_cs_player* player, animation_info_t* info);
	void calculate_timing_for_jump_and_land(c_cs_player* player, animation_info_t* info, int& jump_time, int& land_time);
	void correct_velocity(c_cs_player* player, animation_info_t* info, c_animation_state* state);
	void correct_jump_and_land_on_teleport(c_cs_player* player, animation_info_t* info, c_animation_state* state);
	void correct_jump_and_land_on_unchoke(c_cs_player* player, animation_info_t* info, c_animation_state* state);
	void correct_jump_and_land(c_cs_player* player, animation_info_t* info, c_animation_state* state, int& current_command_tick, int& jump_time, int& land_time);

	void update_client_side_animation(c_cs_player* player, animation_info_t* info, c_animation_state* state);

	void simulate_animation_side(c_cs_player* player, animation_info_t* info, c_animation_state* state, int side, bool should_update,
		bool teleported, animation_record_t* record, animation_record_t* prev_record);

	void update_animations(c_cs_player* player, animation_info_t* info, c_animation_state* state);

public:
	animation_info_t* get_animation_info(int index) {
		if (index > 64 || index < 0)
			return nullptr;

		return &animation_info[index];
	}

	void collect_info(c_cs_player* player);

	virtual void frame_post(int stage) override;
	virtual void map_init_post() override;
	virtual void map_shutdown() override;
	virtual void update_client_side_animation(c_cs_player* player) override;
};

#ifdef _DEBUG
inline auto __animation_fix = std::make_unique<c_animation_fix>();
#else
CREATE_DUMMY_PTR(c_animation_fix);
DECLARE_XORED_PTR(c_animation_fix, GET_XOR_KEYUI32);

#define __animation_fix XORED_PTR(c_animation_fix)
#endif