#pragma once

class c_bone_builder;

struct server_matrix_t {
	c_bone_builder builder{};
	alignas(16) matrix3x4_t matrix[128]{};

	INLINE void reset() {
		builder.reset();

		for (auto& i : matrix)
			i = {};
	}
};

struct bone_blending_t {
	vec3_t v_angle{};

	c_animation_layers layers[13]{};
	float poses[24]{};

	INLINE void store(c_cs_player* player) {
		v_angle = player->render_angles();

		player->store_layers(layers);
		player->store_poses(poses);
	}

	INLINE void restore(c_cs_player* player) {
		player->render_angles() = player->eye_angles() = v_angle;

		player->set_layers(layers);
		player->set_poses(poses);
	}

	INLINE void reset() {
		v_angle.reset();

		for (auto& i : layers)
			i = {};

		for (auto& i : poses)
			i = 0.f;
	}
};

class c_local_animation_fix : public c_feature {
private:
	bool landing{};
	bool jumping{};

	bits_t old_flags{};
	int old_move_type{};

	float old_spawn_time{};

	c_animation_layers backup_layers[13]{};
	c_animation_layers fresh_layers[13]{};

	bone_blending_t real_blending{};
	bone_blending_t restore_blending{};
	server_matrix_t real_matrix{};

	c_animation_state updated_state{};
	networked_vars_t old_networked_vars{};

	INLINE void reset_all() {
		landing = false;
		jumping = false;

		old_flags = 0;
		old_move_type = 0;

		old_spawn_time = 0.f;

		for (auto& i : backup_layers)
			i = {};

		for (auto& i : fresh_layers)
			i = {};

		real_blending.reset();
		restore_blending.reset();
		real_matrix.reset();

		updated_state = {};

		old_networked_vars.reset();
	}

	void handle_jump_animations(c_animation_state* state, c_animation_layers* layers, c_user_cmd* cmd);
	void handle_strafing(c_animation_state* state, c_user_cmd* cmd);
public:
	virtual void predict(c_user_cmd* cmd) override;
	virtual void update_client_side_animation(c_cs_player* player) override;

	virtual void map_init_post() override;
	virtual void map_shutdown() override;
	virtual void tick_local_death() override;
};

#ifdef _DEBUG
inline auto __local_animation_fix = std::make_unique<c_local_animation_fix>();
#else
CREATE_DUMMY_PTR(c_local_animation_fix);
DECLARE_XORED_PTR(c_local_animation_fix, GET_XOR_KEYUI32);

#define __local_animation_fix XORED_PTR(c_local_animation_fix)
#endif