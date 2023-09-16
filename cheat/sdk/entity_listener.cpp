#include "../main_definitions.hpp"
#include "../security/security.hpp"
#include "memory.hpp"
#include "entities.hpp"
#include "entity_listener.hpp"

INLINE bool erase_entity(int index, listened_entities& entities) {
	const auto it = std::find_if(entities.begin(), entities.end(), [&](const listened_entity_t& entity)
		{ return entity.idx == index; });

	if (it == entities.end())
		return false;

	entities.erase(it);
	return true;
}

void c_listener_entity::on_entity_created(c_base_entity* entity) {
	auto client_class = entity->get_client_class();
	if (!client_class)
		return;

	auto class_id = client_class->class_id;
	if (class_id == CCSPlayerResource)
		__interfaces->player_resource = reinterpret_cast<c_cs_player_resource*>(entity);

	auto get_entity_type = [&]() {
		if (entity->is_player())
			return ENT_PLAYER;

		if (entity->is_weapon() || entity->is_grenade(class_id) || entity->is_bomb(class_id))
			return ENT_WEAPON;

		if (class_id == CFogController)
			return ENT_FOG;

		if (class_id == CEnvTonemapController)
			return ENT_TONEMAP;

		if (class_id == CSprite)
			return ENT_LIGHT;

		if (class_id == CCSRagdoll)
			return ENT_RAGDOLL;

		return ENT_INVALID;
	};

	int type = get_entity_type();
	if (type == ENT_INVALID)
		return;

	entities[type].emplace_back(entity);
}

void c_listener_entity::on_entity_deleted(c_base_entity* entity) {
	int index = entity->index();
	if (index < 0)
		return;

	for (int i = 0; i < ENT_MAX; ++i)
		erase_entity(index, entities[i]);
}