#pragma once

void __fastcall packet_start(c_client_state* ecx, void* edx, int incoming, int outgoing);

struct unpred_vars_t {
	bool in_prediction{};
	bool first_time_predicted{};

	int random_seed{};
	int prediction_player{};

	float recoil_index{};
	float accuracy_penalty{};

	float curtime{};
	float frametime{};

	bits_t flags{};

	vec3_t velocity{};

	c_user_cmd* predicted_cmd{};
	c_user_cmd updated_cmd{};

	c_base_handle ground_entity{};

	INLINE void store() {
		in_prediction = __interfaces->prediction->in_prediction;
		first_time_predicted = __interfaces->prediction->is_first_time_predicted;

		flags = __local_player->flags();
		velocity = __local_player->velocity();

		if (__local_weapon) {
			recoil_index = __local_weapon->recoil_index();
			accuracy_penalty = __local_weapon->accuracy_penalty();
		}

		curtime = __interfaces->global_vars->curtime;
		frametime = __interfaces->global_vars->frametime;

		ground_entity.set_value(__local_player->ground_entity().get_value());

		// hardcoded because it doesn't parse with netvars
#ifdef LEGACY
		predicted_cmd = *reinterpret_cast<c_user_cmd**>(reinterpret_cast<std::uintptr_t>(__local_player) + XORN(0x3314));
		updated_cmd = *reinterpret_cast<c_user_cmd*>(reinterpret_cast<std::uintptr_t>(__local_player) + XORN(0x326C));
#else
		predicted_cmd = *reinterpret_cast<c_user_cmd**>(reinterpret_cast<std::uintptr_t>(__local_player) + XORN(0x3348));
		updated_cmd = *reinterpret_cast<c_user_cmd*>(reinterpret_cast<std::uintptr_t>(__local_player) + XORN(0x3298));
#endif
	}

	INLINE void restore() {
		__interfaces->prediction->in_prediction = in_prediction;
		__interfaces->prediction->is_first_time_predicted = first_time_predicted;

		if (__local_weapon) {
			__local_weapon->recoil_index() = recoil_index;
			__local_weapon->accuracy_penalty() = accuracy_penalty;
		}

		__interfaces->global_vars->curtime = curtime;
		__interfaces->global_vars->frametime = frametime;

#ifdef LEGACY
		* reinterpret_cast<c_user_cmd**>(reinterpret_cast<std::uintptr_t>(__local_player) + XORN(0x3314)) = predicted_cmd;
		*reinterpret_cast<c_user_cmd*>(reinterpret_cast<std::uintptr_t>(__local_player) + XORN(0x326C)) = updated_cmd;
#else
		* reinterpret_cast<c_user_cmd**>(reinterpret_cast<std::uintptr_t>(__local_player) + XORN(0x3348)) = predicted_cmd;
		*reinterpret_cast<c_user_cmd*>(reinterpret_cast<std::uintptr_t>(__local_player) + XORN(0x3298)) = updated_cmd;
#endif
	}

	INLINE void reset() {
		in_prediction = false;
		first_time_predicted = false;

		random_seed = 0;
		prediction_player = 0;

		recoil_index = 0.f;
		accuracy_penalty = 0.f;

		curtime = 0.f;
		frametime = 0.f;

		flags = 0;

		velocity.reset();

		predicted_cmd = nullptr;
		updated_cmd = {};

		ground_entity = 0;
	}
};

struct networked_vars_t {
	bool done = false;
	bool walking{};
	bool scoped{};

	int command_number{};
	int tickbase{};
	int move_state{};
	int move_type{};
	bits_t flags{};

	float spread{};
	float inaccuracy{};
	float duck_amount{};

	float stamina{};
	float recoil_index{};
	float acc_penalty{};
	float lower_body_yaw{};
	float thirdperson_recoil{};
	float fall_velocity{};
	float velocity_modifier{};

	vec3_t origin{};
	vec3_t abs_origin{};
	vec3_t velocity{};
	vec3_t viewoffset{};
	vec3_t aimpunch{};
	vec3_t aimpunch_vel{};
	vec3_t viewpunch{};
	vec3_t viewangles{};
	vec3_t ladder_normal{};
	vec3_t base_velocity{};
	vec3_t network_origin{};

	c_base_handle ground_entity{};

	INLINE void store(c_user_cmd* cmd, bool no_ground_entity = false) {
		walking = __local_player->is_walking();
		scoped = __local_player->is_scoped();

		command_number = cmd->command_number;

		tickbase = __local_player->tickbase();
		move_state = __local_player->move_state();
		move_type = __local_player->move_type();
		flags = __local_player->flags();

		if (__local_weapon) {
			spread = __local_weapon->get_spread();
			inaccuracy = __local_weapon->get_inaccuracy();

			recoil_index = __local_weapon->recoil_index();
			acc_penalty = __local_weapon->accuracy_penalty();
		}

		lower_body_yaw = __local_player->lower_body_yaw();
		thirdperson_recoil = __local_player->thirdperson_recoil();
		duck_amount = __local_player->duck_amount();
		stamina = __local_player->stamina();
		fall_velocity = __local_player->fall_velocity();
		velocity_modifier = __local_player->velocity_modifier();

		origin = __local_player->origin();
		abs_origin = __local_player->get_abs_origin();
		velocity = __local_player->velocity();
		viewoffset = __local_player->view_offset();
		aimpunch = __local_player->aim_punch_angle();
		aimpunch_vel = __local_player->aim_punch_angle_vel();
		viewpunch = __local_player->view_punch_angle();
		ladder_normal = __local_player->ladder_normal();
		base_velocity = __local_player->base_velocity();
		network_origin = __local_player->network_origin();

		if (!no_ground_entity)
			ground_entity.set_value(__local_player->ground_entity().get_value());

		viewangles = cmd->viewangles;

		done = true;
	}

	INLINE void restore(bool animations = false) {
		if (!done)
			return;

		if (__local_weapon) {
			__local_weapon->recoil_index() = recoil_index;
			__local_weapon->accuracy_penalty() = acc_penalty;
		}

		__local_player->duck_amount() = duck_amount;
		__local_player->origin() = origin;
		__local_player->set_abs_origin(abs_origin);
		__local_player->view_offset() = viewoffset;
		__local_player->aim_punch_angle() = aimpunch;
		__local_player->aim_punch_angle_vel() = aimpunch_vel;
		__local_player->view_punch_angle() = viewpunch;

		if (animations) {
			__local_player->is_walking() = walking;
			__local_player->is_scoped() = scoped;

			__local_player->lower_body_yaw() = lower_body_yaw;
			__local_player->thirdperson_recoil() = thirdperson_recoil;

			__local_player->move_state() = move_state;
			__local_player->move_type() = move_type;

			__local_player->ground_entity().set_value(ground_entity.get_value());
			__local_player->flags().bits = flags.bits;

			__local_player->abs_velocity() = __local_player->velocity() = velocity;
			__local_player->ladder_normal() = ladder_normal;
		}
	}

	INLINE void reset() {
		done = false;
		walking = false;
		scoped = false;

		tickbase = 0;
		move_state = 0;
		move_type = 0;
		flags = 0;

		duck_amount = 0.f;

		spread = 0.f;
		inaccuracy = 0.f;
		stamina = 0.f;
		fall_velocity = 0.f;
		velocity_modifier = 0.f;

		recoil_index = 0.f;
		acc_penalty = 0.f;
		lower_body_yaw = 0.f;
		thirdperson_recoil = 0.f;

		origin.reset();
		abs_origin.reset();
		velocity.reset();
		viewoffset.reset();
		aimpunch.reset();
		aimpunch_vel.reset();
		viewpunch.reset();
		viewangles.reset();
		ladder_normal.reset();
		base_velocity.reset();
		network_origin.reset();

		ground_entity = 0;
	}
};

struct viewmodel_info_t {
	bool looking_at_weapon{};

	int cmd_tick{};
	int sequence{};
	int animation_parity{};
	int new_sequence_parity{};
	int model_index{};

	float anim_time{};
	float old_anim_time{};

	float cycle{};
	float old_cycle{};

	c_base_entity* active_weapon{};

	// don't ask why..
	INLINE void store(c_user_cmd* cmd, c_cs_player* viewmodel) {
		cmd_tick = cmd->command_number;

		auto owner = viewmodel->viewmodel_owner().get().cast<c_cs_player*>();
		if (owner) {
			looking_at_weapon = owner->looking_at_weapon();
			active_weapon = owner->active_weapon().get().cast<c_base_entity*>();
		}

		sequence = viewmodel->sequence();
		animation_parity = viewmodel->animation_parity();
		new_sequence_parity = viewmodel->new_sequence_parity();
		model_index = viewmodel->model_index();
		model_index = viewmodel->model_index();

		anim_time = viewmodel->anim_time();
		old_anim_time = viewmodel->old_anim_time();

		cycle = viewmodel->cycle();
		old_cycle = viewmodel->old_cycle();
	}

	INLINE void reset() {
		looking_at_weapon = false;

		cmd_tick = 0;
		sequence = 0;
		animation_parity = 0;
		new_sequence_parity = 0;
		model_index = 0;

		anim_time = 0.f;
		old_anim_time = 0.f;

		cycle = 0.f;
		old_cycle = 0.f;

		active_weapon = nullptr;
	}
};

class c_engine_prediction : public c_feature {
private:
	bool in_prediction{};
	bool should_repredict{};

	int predicted_tickbase{};
	c_user_cmd* last_cmd{};

	c_move_data move_data{};

	unpred_vars_t unpred_vars{};
	networked_vars_t networked_vars[150]{};
	networked_vars_t unpred_networked_vars[150]{};
	networked_vars_t sent_vars[150]{};
	viewmodel_info_t viewmodel_info[150]{};
	
	std::string last_sound_name{};

	std::vector<int> outgoing_commands{};

	void update_ingame_prediction();
	void update_predicted_tickbase(c_user_cmd* cmd);
	void update_viewmodel_info(c_user_cmd* cmd);

public:
	int* prediction_player{};
	int* prediction_random_seed{};

	INLINE networked_vars_t* get_networked_vars(int cmd) {
		return &networked_vars[cmd % 150];
	}

	INLINE networked_vars_t* get_unpred_networked_vars(int cmd) {
		return &unpred_networked_vars[cmd % 150];
	}

	INLINE networked_vars_t* get_sent_vars(int cmd) {
		return &sent_vars[cmd % 150];
	}

	INLINE viewmodel_info_t* get_viewmodel_info(int cmd) {
		return &viewmodel_info[cmd % 150];
	}

	INLINE unpred_vars_t* get_unpredicted_vars() {
		return &unpred_vars;
	}

	INLINE c_move_data* get_move_data() {
		return &move_data;
	}

	INLINE std::string& get_last_sound_name() {
		return last_sound_name;
	}

	INLINE bool should_repredict_vars() {
		return should_repredict;
	}

	INLINE bool is_in_prediction() {
		return in_prediction;
	}

	INLINE int get_tickbase() {
		return predicted_tickbase;
	}

	INLINE void reset_all() {
		in_prediction = false;

		predicted_tickbase = 0;
		last_cmd = nullptr;

		move_data = {};

		unpred_vars.reset();

		for (auto& i : networked_vars)
			i.reset();

		for (auto& i : unpred_networked_vars)
			i.reset();

		for (auto& i : sent_vars)
			i.reset();

		for (auto& i : viewmodel_info)
			i.reset();

		last_sound_name = "";

		outgoing_commands.clear();
	}

	void repredict(c_user_cmd* cmd);

	virtual void pre_predict(c_user_cmd* cmd) override;
	virtual void post_predict(c_user_cmd* cmd) override;
	virtual void run_command(c_user_cmd* cmd) override;

	virtual void map_init_pre() override;
	virtual void map_init_post() override;
	virtual void map_shutdown() override;

	virtual void tick_local_death() override;
	virtual void tick_end(bool* send_packet, int sequence_number) override;
	virtual void frame(int stage) override;

	virtual void on_packet_start(c_client_state* ecx, void* edx, int incoming, int outgoing) override;
	virtual void on_packet_end(int command) override;
};

#ifdef _DEBUG
inline auto __engine_prediction = std::make_unique<c_engine_prediction>();
#else
CREATE_DUMMY_PTR(c_engine_prediction);
DECLARE_XORED_PTR(c_engine_prediction, GET_XOR_KEYUI32);

#define __engine_prediction XORED_PTR(c_engine_prediction)
#endif