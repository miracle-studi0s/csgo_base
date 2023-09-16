#include "../callbacks.hpp"

#include "entities.hpp"
#include "local_player.hpp"

#include "../render/renderer.hpp"

void c_local_player::update() {
	local = **FIND_OFFSET(__modules->client, OFFSET_LOCAL_PLAYER).cast<c_cs_player***>();

	if (!local || !local->is_alive()) {
		weapon = nullptr;
		weapon_info = nullptr;
		return;
	}

	weapon = local->active_weapon().get().cast<c_base_combat_weapon*>();
	if (weapon)
		weapon_info = __interfaces->weapon_system->get_weapon_data(weapon->item_definition_index());
}

void c_local_player::frame(int stage) {
	update();
}

void c_local_player::map_init_pre() {
	update();
}

void c_local_player::map_shutdown() {
	local = nullptr;
	weapon = nullptr;
	weapon_info = nullptr;
}