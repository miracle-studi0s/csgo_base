#pragma once

// to avoid changing global variables in game and avoid race condition
// the best solution would be rebuild parts when global vars were used 
// and then take custom variables instead

class c_threaded_animstate : public c_feature {
private:
	void setup_velocity(c_animation_state* state, float curtime);
	void setup_lean(c_animation_state* state, float curtime);

	// it gets bugged, idk why lol
	float last_velocity_test_time[65]{};
	float lower_body_realign_timer[65]{};
public:
	INLINE void reset_variables(int index) {
		last_velocity_test_time[index] = 0.f;
		lower_body_realign_timer[index] = 0.f;
	}

	virtual void map_init_pre() override;
	virtual void map_init_post() override;
	virtual void map_shutdown() override;

	void update(c_animation_state* state, float yaw, float pitch, float curtime, int framecount);
};

#ifdef _DEBUG
inline auto __threaded_animstate = std::make_unique<c_threaded_animstate>();
#else
CREATE_DUMMY_PTR(c_threaded_animstate);
DECLARE_XORED_PTR(c_threaded_animstate, GET_XOR_KEYUI32);

#define __threaded_animstate XORED_PTR(c_threaded_animstate)
#endif