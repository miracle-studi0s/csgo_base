#pragma once

class c_game_information : public c_feature {
public:
	bool* send_packet{};
	bool connected{};
	bool cl_lagcomp0{};

	int tickrate{};
	float lerp_time{};
	float max_unlag{};
	float predicted_time{};
	float ping{};

	INLINE void reset() {
		connected = false;
		cl_lagcomp0 = false;

		tickrate = 0;

		predicted_time = 0.f;
		lerp_time = 0.f;
		max_unlag = 0.f;
		ping = 0.f;
	}

	virtual void pre_predict(c_user_cmd* cmd) override;
	virtual void predict(c_user_cmd* cmd) override;
	virtual void post_predict(c_user_cmd* cmd) override;
	virtual void map_init_pre() override;
	virtual void map_shutdown() override;
	virtual void frame(int stage) override;
	virtual void tick(bool* send_packet) override;
};

#ifdef _DEBUG
inline auto __game_info = std::make_unique<c_game_information>();
#else
CREATE_DUMMY_PTR(c_game_information);
DECLARE_XORED_PTR(c_game_information, GET_XOR_KEYUI32);

#define __game_info XORED_PTR(c_game_information)
#endif