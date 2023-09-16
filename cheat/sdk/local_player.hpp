#pragma once

class c_cs_player;
class c_base_combat_weapon;
struct weapon_info_t;

class c_local_player : public c_feature {
private:
	void update();

public:
	c_cs_player* local{};
	c_base_combat_weapon* weapon{};
	weapon_info_t* weapon_info{};

	virtual void frame(int stage) override;
	virtual void map_init_pre() override;
	virtual void map_shutdown() override;
};

#ifdef _DEBUG
inline auto __local = std::make_unique<c_local_player>();
#else
CREATE_DUMMY_PTR(c_local_player);
DECLARE_XORED_PTR(c_local_player, GET_XOR_KEYUI32);

#define __local XORED_PTR(c_local_player)
#endif

#define __local_player __local->local
#define __local_weapon __local->weapon
#define __local_weapon_info __local->weapon_info