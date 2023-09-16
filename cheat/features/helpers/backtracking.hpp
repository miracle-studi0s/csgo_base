#pragma once

struct animation_side_t;
struct clamp_bones_info_t;

class c_backtracking : public c_animations, c_feature {
private:
	bool is_tick_valid(c_cs_player* player, animation_record_t* record);

	void build_roll_matrix(c_cs_player* player, animation_side_t* side, int side_index, float& fresh_tick, vec3_t& fresh_angles, clamp_bones_info_t& clamp_info);
	void clamp_matrix(c_cs_player* player, animation_side_t* side, float& fresh_tick, vec3_t& fresh_angles, clamp_bones_info_t& clamp_info);

public:
	float get_lerp_time();
	animation_record_t* get_first_record(c_cs_player* player);
	animation_record_t* get_last_record(c_cs_player* player);
	void set_record(c_cs_player* player, animation_record_t* record, matrix3x4_t* matrix);

	void update_player_lagcomp(c_cs_player* player, float& fresh_tick);

	virtual void collect(c_cs_player* player, animation_info_t* info, animation_record_t* record) override;
	virtual void predict(c_user_cmd* cmd) override;
};

#ifdef _DEBUG
inline auto __backtracking = std::make_unique<c_backtracking>();
#else
CREATE_DUMMY_PTR(c_backtracking);
DECLARE_XORED_PTR(c_backtracking, GET_XOR_KEYUI32);

#define __backtracking XORED_PTR(c_backtracking)
#endif