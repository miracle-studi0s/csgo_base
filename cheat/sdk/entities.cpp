#include "entities.hpp"

void* c_studio_hdr::get_sequence_desc(int sequence) {
	void* sequence_desc = nullptr;

	auto hdr_model = reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(this) + 4);
	if (hdr_model)
		sequence_desc = FIND_OFFSET(__modules->client, OFFSET_GET_SEQUENCE_DESC).cast<void*(__thiscall*)(void*, int)>()(this, sequence);
	else {
		auto hdr_sequence = *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + 0xBC);
		auto hdr_unk = *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + 0xC0);

		if (sequence < 0 || sequence >= hdr_sequence)
			sequence = 0;

		sequence_desc = *reinterpret_cast<void**>(reinterpret_cast<std::uintptr_t>(this) + hdr_unk + 212 * sequence);
	}

	return sequence_desc;
}

int c_base_entity::get_sequence_activity(int sequence) {
	auto hdr = __interfaces->model_info->get_studio_model(this->get_model());
	if (!hdr)
		return 0;

	return FIND_OFFSET(__modules->client, OFFSET_GET_SEQUENCE_ACTIVITY).cast<int(__fastcall*)(decltype(this), void*, int)>()(this, hdr, sequence);
}

void c_base_entity::attachments_helper() {
	auto hdr = get_studio_hdr();
	if (!hdr)
		return;

	FIND_OFFSET(__modules->client, OFFSET_ATTACHMENTS_HELPER).cast<void(__thiscall*)(decltype(this), void*)>()(this, hdr);
}

// due to some bugs & bad interpolation
// position of moveparent sometimes break
// this solution adjust coordinate frame (world position) of moveparent to abs origin
void c_cs_player::interpolate_moveparent_pos() {
	auto parent = move_parent().get().cast<c_base_entity*>();
	if (parent) {
		// set new moveparent pos to interpolated one
		auto& frame = parent->coordinate_frame();
		frame.set_origin(get_abs_origin());
	}
}

void c_cs_player::update_weapon_dispatch_layers() {
	auto weapon = active_weapon().get().cast<c_base_combat_weapon*>();
	if (weapon) {
		auto world_weapon = weapon->weapon_world_model().get().cast<c_cs_player*>();
		if (world_weapon) {
			for (int i = 0; i < 13; i++) {
				auto layer = &animlayers()[i];
				layer->owner = this;
				layer->studio_hdr = get_studio_hdr();

				if (layer->sequence >= 2 && layer->weight > 0.f)
					update_dispatch_layer(layer, world_weapon->get_studio_hdr(), layer->sequence);
			}
		}
	}
}

// calculate ideal pose parameter value as game does it
float c_cs_player::studio_set_pose_parameter(int index, float value, float& ctl_value) {
	auto hdr = get_studio_hdr();
	if (!hdr)
		return 0.f;

	if (index < 0 || index > 24)
		return 0.f;

	auto pose_parameter = get_pose_parameter(hdr, index);
	if (!pose_parameter)
		return 0.f;

	if (pose_parameter->loop) {
		auto wrap = (pose_parameter->start + pose_parameter->end) / 2.f + pose_parameter->loop / 2.f;
		auto shift = pose_parameter->loop - wrap;

		value = value - pose_parameter->loop * std::floorf((value + shift) / pose_parameter->loop);
	}

	ctl_value = (value - pose_parameter->start) / (pose_parameter->end - pose_parameter->start);

	if (ctl_value < 0) ctl_value = 0;
	if (ctl_value > 1) ctl_value = 1;

	return ctl_value * (pose_parameter->end - pose_parameter->start) + pose_parameter->start;
}

float c_cs_player::studio_get_pose_parameter(int index, float value) {
	auto hdr = get_studio_hdr();
	if (!hdr)
		return 0.f;

	if (index < 0 || index > 24)
		return 0.f;

	auto pose_parameter = get_pose_parameter(hdr, index);
	if (!pose_parameter)
		return 0.f;

	if (pose_parameter->loop) {
		float wrap = (pose_parameter->start + pose_parameter->end) / 2.f + pose_parameter->loop / 2.f;
		float shift = pose_parameter->loop - wrap;

		value = value - pose_parameter->loop * std::floorf((value + shift) / pose_parameter->loop);
	}

	return (value - pose_parameter->start) / (pose_parameter->end - pose_parameter->start);
}

vec3_t c_cs_player::get_hitbox_position(int hitbox, matrix3x4_t* matrix) {
	if (!matrix)
		matrix = bone_cache().base();

	auto hdr = __interfaces->model_info->get_studio_model(get_model());

	if (!hdr)
		return {};

	auto hitbox_set = hdr->hitbox_set(this->hitbox_set());

	if (!hitbox_set)
		return {};

	auto hdr_hitbox = hitbox_set->hitbox(hitbox);

	if (!hdr_hitbox)
		return {};

	vec3_t min, max;

	math::vector_transform(hdr_hitbox->min, matrix[hdr_hitbox->bone], min);
	math::vector_transform(hdr_hitbox->max, matrix[hdr_hitbox->bone], max);

	return (min + max) * 0.5f;
}

int c_cs_player::get_ping() {
	if (!this)
		return 0;

	static auto offset = __netvars->get(HASH_CT("DT_CSPlayerResource"), HASH_CT("m_iPing"));

	if (offset != 0)
		return *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(__interfaces->player_resource) + offset + (index() * 4));

	return 0;
}

void c_cs_player::run_pre_think() {
	static auto physics_run_think = FIND_OFFSET(__modules->client, OFFSET_PHYSICS_RUN_THINK).cast<bool(__thiscall*)(void*, int)>();

	if (physics_run_think(this, 0))
		__memory->get_virtual(this, XORN(PHYSICS_RUN_THINK_VFUNC)).cast<void(__thiscall*)(void*)>()(this);
}

void c_cs_player::run_think() {
#ifdef LEGACY
	const auto next_think = reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + XORN(0xF8));
#else
	const auto next_think = reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + XORN(0xFC));
#endif

	static auto think = FIND_OFFSET(__modules->client, OFFSET_THINK).cast<void(__thiscall*)(void*, int)>();

	if (*next_think > 0 && *next_think <= this->tickbase()) {
		*next_think = -1;

		think(this, 0);
		__memory->get_virtual(this, XORN(UNK_THINK_VFUNC)).cast<void(__thiscall*)(void*)>()(this);
	}
}

void c_cs_player::run_post_think() {
	static auto post_think_physics = FIND_OFFSET(__modules->client, OFFSET_POST_THINK_PHYSICS).cast<bool(__thiscall*)(void*)>();
	static auto simulate_player_simulated_entities = FIND_OFFSET(__modules->client, OFFSET_SIMULATE_PLAYER_SIMULATED_ENTITIES).cast<void(__thiscall*)(void*)>();

	__memory->get_virtual(__interfaces->model_cache, XORN(MDL_CACHE_LOCK_VFUNC)).cast<void(__thiscall*)(void*)>()(__interfaces->model_cache);

	if (this->is_alive()) {
		__memory->get_virtual(this, XORN(UPDATE_BOUNDS_VFUNC)).cast<void(__thiscall*)(void*)>()(this);

		if (this->flags().has(FL_ONGROUND))
			this->fall_velocity() = 0.f;

		if (this->sequence() == -1)
			__memory->get_virtual(this, XORN(SET_SEQUENCE_VFUNC)).cast<void(__thiscall*)(void*, int)>()(this, 0);

		__memory->get_virtual(this, XORN(UNK_POST_THINK_VFUNC)).cast<void(__thiscall*)(void*)>()(this);
		post_think_physics(this);
	}

	simulate_player_simulated_entities(this);
	__memory->get_virtual(__interfaces->model_cache, XORN(MDL_CACHE_UNLOCK_VFUNC)).cast<void(__thiscall*)(void*)>()(__interfaces->model_cache);
}