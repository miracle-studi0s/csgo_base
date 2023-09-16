#pragma once

#include "../security/security.hpp"

#include "memory.hpp"
#include "modules.hpp"
#include "math.hpp"
#include "valve.hpp"
#include "netvars.hpp"
#include "offsets.hpp"
#include "interfaces.hpp"
#include "convar_list.hpp"
#include "local_player.hpp"

class c_base_entity;
class c_cs_player;
class c_base_handle;
class c_base_combat_weapon;

class c_renderable;
class c_networkable;
class c_thinkable;
class c_unknown;
class c_client_entity;

class c_studio_hdr {
public:
	OFFSET(bone_flags, c_utl_vector< int >&, 0x30);
	OFFSET(bone_parent_count, int&, 0x50);

	void* get_sequence_desc(int sequence);
};

class c_handle_entity {
public:
	virtual ~c_handle_entity() {}
	virtual void set_ref_handle(std::uint32_t& handle) = 0;
	virtual const std::uint32_t& get_ref_handle() const = 0;
};

class c_collideable {
public:
	virtual c_handle_entity* get_entity_handle() = 0;
	virtual vec3_t& get_mins() const = 0;
	virtual vec3_t& get_maxs() const = 0;

	virtual void world_space_trigger_bounds(vec3_t* pVecWorldMins, vec3_t* pVecWorldMaxs) const = 0;
	virtual bool test_collision(const ray_t& ray, unsigned int fContentsMask, c_game_trace& tr) = 0;
	virtual bool test_hitboxes(const ray_t& ray, unsigned int fContentsMask, c_game_trace& tr) = 0;
	virtual int get_collision_model_index() = 0;

	virtual const model_t* get_collision_model() = 0;

	virtual const vec3_t& get_collision_origin() const = 0;
	virtual const vec3_t& get_collision_angles() const = 0;
};

class c_unknown : public c_handle_entity {
public:
	virtual c_collideable* get_collideable() = 0;
	virtual c_networkable* get_networkable_entity() = 0;
	virtual c_renderable* get_renderable_entity() = 0;
	virtual c_client_entity* get_client_entity() = 0;
	virtual c_base_entity* get_base_entity() = 0;
	virtual c_thinkable* get_thinkable_entity() = 0;
	virtual void* get_client_alpha_property() = 0;
};

class c_renderable {
public:
	virtual c_unknown* get_i_unknown_entity() = 0;

	virtual const vec3_t& get_render_origin(void) = 0;
	virtual const vec3_t& get_render_angles(void) = 0;

	virtual bool should_render(void) = 0;
	virtual int get_render_flags(void) = 0;

	virtual void unused(void) const {}

	virtual unsigned short get_shadow_handle() const = 0;
	virtual unsigned short& get_render_handle() = 0;

	virtual model_t* get_model() const = 0;
	virtual int draw_model(int flags, const int& instance) = 0;

	virtual int get_body() = 0;

	virtual void get_color_modulation(float* color) = 0;

	virtual bool lod_test() = 0;

	virtual bool setup_bones(matrix3x4_t* matrix, int max_bones, int mask, float time) = 0;
	virtual void setup_weights(const matrix3x4_t* matrix, int flex_cnt, float* flex_weights, float* flex_delayed_weights) = 0;

	virtual void useless_func1(void) = 0;

	virtual void* useless_func2() = 0;

	virtual void get_render_bounds(vec3_t& mins, vec3_t& maxs) = 0;
};

class c_networkable {
public:
	virtual c_unknown* get_n_unknown_entity() = 0;
	virtual void release() = 0;
	virtual c_client_class* get_client_class() = 0;
	virtual void should_transmit(int state) = 0;
	virtual void pre_data_changed(int update_type) = 0;
	virtual void data_changed(int update_type) = 0;
	virtual void pre_update(int update_type) = 0;
	virtual void post_update(int update_type) = 0;
	virtual void unk(void) = 0;
	virtual bool dormant(void) = 0;
	virtual int index(void) = 0;
	virtual void recieve_message() = 0;
	virtual void* get_data_table_base_ptr() = 0;
	virtual void set_destroyed_on_recreate_entities(void) = 0;
};

class c_thinkable {
public:
	virtual c_unknown* get_t_unknown_entity() = 0;
};

class c_client_entity : public c_unknown, public c_renderable, public c_networkable, public c_thinkable {
public:
	virtual void release(void) = 0;

	virtual const vec3_t& get_abs_origin(void) = 0;
	virtual const vec3_t& get_abs_angles(void) = 0;
};

class c_ik_context {
public:
	void constructor() {
		static auto ik_ctor = FIND_OFFSET(__modules->client, OFFSET_IK_CONTEXT_CONSTRUCT).cast<void(__thiscall*)(void*)>();
		ik_ctor(this);
	}

	void destructor() {
		static auto ik_dector = FIND_OFFSET(__modules->client, OFFSET_IK_CONTEXT_DESTRUCT).cast<void(__thiscall*)(void*)>();
		ik_dector(this);
	}

	void clear_targets() {
		auto i = 0;
		auto count = *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + 0xFF0);

		if (count > 0) {
			auto target = reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + 0xD0);

			do {
				*target = -9999;
				target += 85;
				++i;
			} while (i < count);
		}
	}

	void init(c_studio_hdr* hdr, vec3_t* angles, vec3_t* origin, float time, int frames, int mask) {
		using init_fn = void(__thiscall*)(void*, c_studio_hdr*, vec3_t*, vec3_t*, float, int, int);
		FIND_OFFSET(__modules->client, OFFSET_IK_CONTEXT_INIT).cast<init_fn>()(this, hdr, angles, origin, time, frames, mask);
	}

	void update_targets(vec3_t* pos, quaternion_t* qua, matrix3x4_t* matrix, uint8_t* bone_computed) {
		using update_targets_fn = void(__thiscall*)(void*, vec3_t*, quaternion_t*, matrix3x4_t*, uint8_t*);
		FIND_OFFSET(__modules->client, OFFSET_IK_CONTEXT_UPDATE_TARGETS).cast<update_targets_fn>()(this, pos, qua, matrix, bone_computed);
	}

	void solve_dependencies(vec3_t* pos, quaternion_t* qua, matrix3x4_t* matrix, uint8_t* bone_computed) {
		using solve_dependencies_fn = void(__thiscall*)(void*, vec3_t*, quaternion_t*, matrix3x4_t*, uint8_t*);
		FIND_OFFSET(__modules->client, OFFSET_IK_CONTEXT_SOLVE_DEPENDENCIES).cast<solve_dependencies_fn>()(this, pos, qua, matrix, bone_computed);
	}
};

struct bone_setup_t {
	c_studio_hdr* hdr{};
	int mask{};
	float* pose_parameter{};
	void* pose_debugger{};

	void init_pose(vec3_t pos[], quaternion_t q[], c_studio_hdr* hdr) {
		static auto init_pose_fn = static_cast<std::uint64_t>(FIND_OFFSET(__modules->client, OFFSET_INIT_POSE).pointer);

		__asm
		{
			mov eax, this
			mov esi, q
			mov edx, pos
			push dword ptr[hdr + 4]
			mov ecx, [eax]
			push esi
			call init_pose_fn
			add esp, 8
		}
	}

	void accumulate_pose(vec3_t pos[], quaternion_t q[], int sequence, float cycle, float weight, float time, c_ik_context* ctx) {
		using accumulate_pose_fn = void(__thiscall*)(bone_setup_t*, vec3_t*, quaternion_t*, int, float, float, float, c_ik_context*);
		FIND_OFFSET(__modules->client, OFFSET_ACCUMULATE_POSE).cast<accumulate_pose_fn>()(this, pos, q, sequence, cycle, weight, time, ctx);
	}

	void calc_autoplay_sequences(vec3_t pos[], quaternion_t q[], float real_time, c_ik_context* ctx) {
		static auto calc_autoplay_sequences_fn = static_cast<std::uint64_t>(FIND_OFFSET(__modules->client, OFFSET_CALC_AUTOPLAY_SEQUENCES).pointer);

		__asm
		{
			movss xmm3, real_time
			mov eax, ctx
			mov ecx, this
			push eax
			push q
			push pos
			call calc_autoplay_sequences_fn
		}
	}

	void calc_bone_adjust(vec3_t pos[], quaternion_t q[], float* controllers, int mask) {
		static auto calc_bone_adjust_fn = static_cast<std::uint64_t>(FIND_OFFSET(__modules->client, OFFSET_CALC_BONE_ADJUST).pointer);

		__asm
		{
			mov eax, controllers
			mov ecx, this
			mov edx, pos; a2
			push dword ptr[ecx + 4]; a5
			mov ecx, [ecx]; a1
			push eax; a4
			push q; a3
			call calc_bone_adjust_fn
			add esp, 0xC
		}
	}
};

class c_cs_player_resource {
public:
	NETVAR(player_c4_index, int&, "DT_CSPlayerResource", "m_iPlayerC4");
};

class c_base_entity : public c_client_entity {
public:
	NETVAR(origin, vec3_t&, "DT_BaseEntity", "m_vecOrigin");
	NETVAR(bb_mins, vec3_t&, "DT_BaseEntity", "m_vecMins");
	NETVAR(bb_maxs, vec3_t&, "DT_BaseEntity", "m_vecMaxs");
	NETVAR(ragdoll_vec_force, vec3_t&, "DT_CSRagdoll", "m_vecForce");
	NETVAR(ragdoll_velocity, vec3_t&, "DT_CSRagdoll", "m_vecRagdollVelocity");

	NETVAR(smoke_effect_tick_begin, int, "DT_SmokeGrenadeProjectile", "m_nSmokeEffectTickBegin");
	NETVAR(fire_coint, int, "DT_Inferno", "m_fireCount");
	NETVAR(life_state, int, "DT_BasePlayer", "m_lifeState");
	NETVAR(sequence, int, "DT_BaseAnimating", "m_nSequence");
	NETVAR(owner, int, "DT_BaseEntity", "m_hOwnerEntity");

#ifdef LEGACY
	OFFSET(move_type, int&, 0x25B);
	OFFSET(grenade_spawn_time, float, 0x29B0);
#else
	OFFSET(move_type, int&, 0x25C);
	OFFSET(grenade_spawn_time, float, 0x2A04);
#endif

	NETVAR(new_sequence_parity, int&, "DT_BaseViewModel", "m_nNewSequenceParity");
	NETVAR(animation_parity, int&, "DT_BaseViewModel", "m_nAnimationParity");

	NETVAR(model_index, int&, "DT_BaseEntity", "m_nModelIndex");
	NETVAR(skybox_area, int&, "DT_BaseEntity", "m_skyBoxArea");
	NETVAR_PTR(fire_x_delta, int, "DT_Inferno", "m_fireXDelta");
	NETVAR_PTR(fire_y_delta, int, "DT_Inferno", "m_fireYDelta");
	NETVAR_PTR(fire_z_delta, int, "DT_Inferno", "m_fireZDelta");
	NETVAR(bomb_site, int, "DT_PlantedC4", "m_nBombSite");

	NETVAR_PTR(pose_parameter, float, "DT_BaseAnimating", "m_flPoseParameter");

	NETVAR_OFFSET(old_cycle, float&, "DT_BaseAnimating", "m_nSequence", 0x30);
	NETVAR_OFFSET(old_anim_time, float&, "DT_BaseEntity", "m_flAnimTime", 4);

	NETVAR(defuse_count_down, float, "DT_PlantedC4", "m_flDefuseCountDown");
	NETVAR(timer_length, float, "DT_PlantedC4", "m_flTimerLength");
	NETVAR(anim_time, float&, "DT_BaseEntity", "m_flAnimTime");
	NETVAR(c4_blow, float, "DT_PlantedC4", "m_flC4Blow");
	NETVAR(cycle, float&, "DT_BaseAnimating", "m_flCycle");

	NETVAR(did_smoke_effect, bool, "DT_SmokeGrenadeProjectile", "m_bDidSmokeEffect");
	NETVAR(bomb_ticking, bool, "DT_PlantedC4", "m_bBombTicking");
	NETVAR(bomb_defused, bool, "DT_PlantedC4", "m_bBombDefused");
	NETVAR_OFFSET(nade_exploded, bool, "DT_BaseCSGrenadeProjectile", "m_nExplodeEffectTickBegin", 4);

	NETVAR_PTR(m_bFireIsBurning, bool, "DT_Inferno", "m_bFireIsBurning");

	NETVAR_PTR(wearables, c_base_handle, "DT_BaseCombatCharacter", "m_hMyWearables");

	NETVAR_OFFSET(coordinate_frame, matrix3x4_t&, "DT_BaseEntity", "m_CollisionGroup", -0x30);

	NETVAR(ground_entity, c_base_handle&, "DT_BasePlayer", "m_hGroundEntity");
	NETVAR(viewmodel_weapon, c_base_handle, "DT_BaseViewModel", "m_hWeapon");
	NETVAR(viewmodel_owner, c_base_handle, "DT_BaseViewModel", "m_hOwner");
	NETVAR(observer_target, c_base_handle, "DT_CSPlayer", "m_hObserverTarget");
	NETVAR(ragdoll_player, c_base_handle, "DT_CSRagdoll", "m_hPlayer");
	NETVAR(active_weapon, c_base_handle, "DT_BasePlayer", "m_hActiveWeapon");
	NETVAR(bomb_defuser, c_base_handle, "DT_PlantedC4", "m_hBombDefuser");
	NETVAR(move_parent, c_base_handle, "DT_BaseEntity", "moveparent");
	NETVAR(viewmodel, c_base_handle, "DT_BasePlayer", "m_hViewModel[0]");

	VFUNC(is_player(), bool(__thiscall*)(decltype(this)), IS_PLAYER_VFUNC);
	VFUNC(is_weapon(), bool(__thiscall*)(decltype(this)), IS_WEAPON_VFUNC);
	VFUNC(set_model_index(int idx), bool(__thiscall*)(decltype(this), int), 75, idx);

	INLINE bool is_bomb(const int& class_id) {
		return class_id == CC4 || class_id == CPlantedC4;
	}

	INLINE bool is_grenade(const int& class_id) {
		switch (class_id) {
		case CBaseCSGrenade:
		case CBaseCSGrenadeProjectile:
		case CDecoyGrenade:
		case CDecoyProjectile:
		case CMolotovGrenade:
		case CMolotovProjectile:
		case CSensorGrenade:
		case CSensorGrenadeProjectile:
		case CSmokeGrenade:
		case CSmokeGrenadeProjectile:
		case CIncendiaryGrenade:
		case CInferno:

#ifndef LEGACY
		case CSnowballProjectile:
		case CBreachCharge:
		case CBreachChargeProjectile:
		case CBumpMine:
		case CBumpMineProjectile:
#endif
			return true;
			break;
		}
		return false;
	}

	INLINE c_studio_hdr* get_studio_hdr() {
		return *(c_studio_hdr**)((std::uintptr_t)this + *FIND_OFFSET(__modules->client, OFFSET_STUDIO_HDR).cast<std::uintptr_t*>());
	}

	int get_sequence_activity(int sequence);
	void attachments_helper();

	PFUNC(set_abs_origin(const vec3_t& origin), void(__thiscall*)(decltype(this), const vec3_t&), FIND_OFFSET(__modules->client, OFFSET_SET_ABS_ORIGIN), origin);
	PFUNC(set_abs_angles(const vec3_t& ang), void(__thiscall*)(decltype(this), const vec3_t&), FIND_OFFSET(__modules->client, OFFSET_SET_ABS_ANGLES), ang);
	PFUNC(invalidate_physics_recursive(int flags), void(__thiscall*)(decltype(this), int), FIND_OFFSET(__modules->client, OFFSET_INVALIDATE_PHYSICS_RECURSIVE), flags);
	PFUNC(lookup_sequence(const char* name), int(__thiscall*)(decltype(this), const char*), FIND_OFFSET(__modules->client, OFFSET_LOOKUP_SEQUENCE), name);
	PFUNC(lookup_bone(const char* name), int(__thiscall*)(decltype(this), const char*), FIND_OFFSET(__modules->client, OFFSET_LOOKUP_BONE), name);
	VFUNC(get_pred_desc_map(), datamap_t* (__thiscall*)(decltype(this)), 17);

	VFUNC(update_ik_locks(float time), void(__thiscall*)(decltype(this), float), 192, time);
	VFUNC(calc_ik_locks(float time), void(__thiscall*)(decltype(this), float), 193, time);
	PFUNC2(set_collision_bounds(vec3_t& mins, vec3_t& maxs), void(__thiscall*)(void*, vec3_t&, vec3_t&), FIND_OFFSET(__modules->client, OFFSET_SET_COLLISION_BOUNDS),
		get_collideable(), mins, maxs);

	PFUNC2(get_pose_parameter(c_studio_hdr* hdr, int index), mstudioposeparamdesc_t*(__thiscall*)(c_studio_hdr*, int), FIND_OFFSET(__modules->client, OFFSET_GET_POSE_PARAMETER),
		hdr, index);
	
	VFUNC(update_dispatch_layer(c_animation_layers* layer, c_studio_hdr* hdr, int sequence),
		void(__thiscall*)(decltype(this), c_animation_layers*, c_studio_hdr*, int), UPDATE_DISPATCH_LAYER_VFUNC, layer, hdr, sequence);
};

class c_cs_player : public c_base_entity {
public:
	DATAMAP(abs_velocity, vec3_t&, "C_BaseEntity", "m_vecAbsVelocity");
	DATAMAP(network_origin, vec3_t&, "C_BaseEntity", "m_vecNetworkOrigin");

	DATAMAP(effects, bits_t&, "C_BaseEntity", "m_fEffects");
	DATAMAP(e_flags, bits_t&, "C_BaseEntity", "m_iEFlags");

	DATAMAP(surface_friction, float&, "C_BasePlayer", "m_surfaceFriction");
	DATAMAP(water_jump_time, float&, "C_BasePlayer", "m_flWaterJumpTime");
	DATAMAP(next_attack, float&, "C_BaseCombatCharacter", "m_flNextAttack");
	DATAMAP(gravity, float&, "C_BaseEntity", "m_flGravity");

	DATAMAP_PTR(buttons, int, "C_BasePlayer", "m_nButtons");

	DATAMAP(button_released, int&, "C_BasePlayer", "m_afButtonReleased");
	DATAMAP(button_pressed, int&, "C_BasePlayer", "m_afButtonPressed");
	DATAMAP(button_last, int&, "C_BasePlayer", "m_afButtonLast");
	DATAMAP(water_level, int&, "C_BaseEntity", "m_nWaterLevel");

	OFFSET_PTR(my_weapons, std::uint32_t, 0x2E08);
	OFFSET(entity_flags, bits_t&, 0x68);

#ifndef LEGACY
	OFFSET(jiggle_bones_enabled, bool&, 0x2930);
	OFFSET(use_new_animstate, bool&, 0x9B14);
	OFFSET(predictable, bool&, 0x2EE);

	OFFSET(ik_ctx, int&, 0x2670);
	OFFSET(lod_flags, bits_t&, 0xA28);

	OFFSET(collision_change_origin, float&, 0x9920);
	OFFSET(collision_change_time, float&, 0x9924);
	OFFSET(observer_chase_distance, float&, 0x33A0);

	OFFSET(last_bone_setup_time, float&, 0x2924);
	OFFSET(model_recent_bone_counter, std::uint32_t&, 0x2690);

	OFFSET(bone_cache, c_utl_vector<matrix3x4_t>&, 0x2914);

	OFFSET(cmd_context, cmd_context_t&, 0x350C);
	OFFSET(animstate, c_animation_state*, 0x9960);
	OFFSET(animlayers, c_animation_layers*, 0x2990);

	OFFSET(predicted_cmd, c_user_cmd*, 0x3348);
	OFFSET(updated_cmd, c_user_cmd&, 0x3298);
#else
	OFFSET(jiggle_bones_enabled, bool&, 0x291C);
	OFFSET(use_new_animstate, bool&, 0x39E1);

	OFFSET(ik_ctx, int&, 0x2570);

	OFFSET(last_bone_setup_time, float&, 0x2914);
	OFFSET(model_recent_bone_counter, std::uint32_t&, 0x2680);

	OFFSET(bone_cache, c_utl_vector<matrix3x4_t>&, 0x2900);

	OFFSET(cmd_context, cmd_context_t&, 0x34D0);
	OFFSET(animstate, c_animation_state*, 0x3874);
	OFFSET(animlayers, c_animation_layers*, 0x2970);

	OFFSET(predicted_cmd, c_user_cmd*, 0x3314);
	OFFSET(updated_cmd, c_user_cmd&, 0x326C);
#endif

	NETVAR_OFFSET(render_angles, vec3_t&, "DT_BasePlayer", "deadflag", 4);
	NETVAR(aim_punch_angle_vel, vec3_t&, "DT_CSPlayer", "m_aimPunchAngleVel");
	NETVAR(view_punch_angle, vec3_t&, "DT_CSPlayer", "m_viewPunchAngle");
	NETVAR(aim_punch_angle, vec3_t&, "DT_CSPlayer", "m_aimPunchAngle");
	NETVAR(ladder_normal, vec3_t&, "DT_CSPlayer", "m_vecLadderNormal");
	NETVAR(base_velocity, vec3_t&, "DT_BasePlayer", "m_vecBaseVelocity");
	NETVAR(ang_rotation, vec3_t&, "DT_BaseEntity", "m_angRotation");
	NETVAR(view_offset, vec3_t&, "DT_BasePlayer", "m_vecViewOffset[0]");
	NETVAR(eye_angles, vec3_t&, "DT_CSPlayer", "m_angEyeAngles[0]");
	NETVAR(velocity, vec3_t&, "DT_BasePlayer", "m_vecVelocity[0]");

	NETVAR(thirdperson_recoil, float&, "DT_CSPlayer", "m_flThirdpersonRecoil");
	NETVAR(velocity_modifier, float&, "DT_CSPlayer", "m_flVelocityModifier");
	NETVAR(lower_body_yaw, float&, "DT_CSPlayer", "m_flLowerBodyYawTarget");
	NETVAR(fall_velocity, float&, "DT_BasePlayer", "m_flFallVelocity");
	NETVAR(step_size, float&, "DT_CSPlayer", "m_flStepSize");
	NETVAR(duck_amount, float&, "DT_CSPlayer", "m_flDuckAmount");
	NETVAR(model_scale, float&, "DT_CSPlayer", "m_flModelScale");
	NETVAR(duck_speed, float&, "DT_CSPlayer", "m_flDuckSpeed");
	NETVAR(max_speed, float&, "DT_CSPlayer", "m_flMaxSpeed");
	NETVAR(stamina, float&, "DT_CSPlayer", "m_flStamina");

	NETVAR_OFFSET(old_sim_time, float&, "DT_BaseEntity", "m_flSimulationTime", 4);
	NETVAR_OFFSET(spawn_time, float&, "DT_CSPlayer", "m_iAddonBits", -4);
	NETVAR(sim_time, float&, "DT_BaseEntity", "m_flSimulationTime");

	NETVAR(collision_group, int, "DT_BaseEntity", "m_CollisionGroup");
	NETVAR(player_state, int, "DT_CSPlayer", "m_iPlayerState");
	NETVAR(armor_value, int, "DT_CSPlayer", "m_ArmorValue");
	NETVAR(hitbox_set, int, "DT_BaseAnimating", "m_nHitboxSet");
	NETVAR(health, int, "DT_CSPlayer", "m_iHealth");
	NETVAR(team, int, "DT_BaseEntity", "m_iTeamNum");

	NETVAR(flags, bits_t&, "DT_CSPlayer", "m_fFlags");

	NETVAR_OFFSET(button_disabled, int&, "DT_BasePlayer", "m_hViewEntity", -0xC);
	NETVAR_OFFSET(button_forced, int&, "DT_BasePlayer", "m_hViewEntity", -0x8);

	NETVAR_OFFSET(final_predicted_tick, int&, "DT_BasePlayer", "m_nTickBase", 4);
	NETVAR(duck_jump_time_msecs, int&, "DT_CSPlayer", "m_nDuckJumpTimeMsecs");
	NETVAR(observer_mode, int&, "DT_CSPlayer", "m_iObserverMode");
	NETVAR(move_state, int&, "DT_CSPlayer", "m_iMoveState");
	NETVAR(tickbase, int&, "DT_BasePlayer", "m_nTickBase");

	NETVAR(ragdoll, c_base_handle, "DT_CSPlayer", "m_hRagdoll");

	NETVAR_OFFSET_PTR(bone_accessor, c_bone_accessor, "DT_BaseAnimating", "m_nForceBone", 28);

	NETVAR(deadflag, bool&, "DT_BasePlayer", "deadflag");
	NETVAR(has_gun_game_immunity, bool, "DT_CSPlayer", "m_bGunGameImmunity");
	NETVAR(has_defuser, bool, "DT_CSPlayer", "m_bHasDefuser");
	NETVAR(is_defusing, bool, "DT_CSPlayer", "m_bIsDefusing");
	NETVAR(has_helmet, bool, "DT_CSPlayer", "m_bHasHelmet");
	NETVAR(has_heavy_armor, bool, "DT_CSPlayer", "m_bHasHeavyArmor");
	NETVAR(wait_for_no_attack, bool, "DT_CSPlayer", "m_bWaitForNoAttack");
	NETVAR(ducking, bool, "DT_CSPlayer", "m_bDucking");

	NETVAR(client_side_animation, bool&, "DT_BaseAnimating", "m_bClientSideAnimation");
	NETVAR(allow_auto_movement, bool&, "DT_CSPlayer", "m_bAllowAutoMovement");
	NETVAR(looking_at_weapon, bool&, "DT_CSPlayer", "m_bIsLookingAtWeapon");
	NETVAR(no_interp_parity, bool&, "DT_BasePlayer", "m_ubEFNoInterpParity");
	NETVAR_OFFSET(no_interp_parity_old, bool&, "DT_BasePlayer", "m_ubEFNoInterpParity", 1);
	NETVAR(is_walking, bool&, "DT_CSPlayer", "m_bIsWalking");
	NETVAR(is_scoped, bool&, "DT_CSPlayer", "m_bIsScoped");
	NETVAR(strafing, bool&, "DT_CSPlayer", "m_bStrafing");

	VFUNC(get_sequence_cycle_rate(void* hdr, int sequence), float(__thiscall*)(decltype(this), void*, int), GET_SEQUENCE_CYCLE_RATE_VFUNC, hdr, sequence);
	VFUNC(get_layer_sequence_cycle_rate(c_animation_layers* layer, int sequence), float(__thiscall*)(decltype(this), c_animation_layers*, int), GET_LAYER_SEQUENCE_CYCLE_RATE_VFUNC, layer, sequence);
	VFUNC(update_client_side_animation(), void(__thiscall*)(decltype(this)), UPDATE_CLIENT_SIDE_ANIMATION_FUNC);

	INLINE bool is_alive() {
		return life_state() == 0 && health() > 0;
	}

	INLINE void store_layers(c_animation_layers* layers) {
		std::memcpy(layers, animlayers(), sizeof(c_animation_layers) * 13);
	}

	INLINE void set_layers(c_animation_layers* layers) {
		std::memcpy(animlayers(), layers, sizeof(c_animation_layers) * 13);
	}

	INLINE void store_poses(float* poses) {
		std::memcpy(poses, pose_parameter(), sizeof(float) * 24);
	}

	INLINE void set_poses(float* poses) {
		std::memcpy(poses, pose_parameter(), sizeof(float) * 24);
	}

	INLINE void store_bone_cache(matrix3x4_t* matrix) {
		auto& cache = bone_cache();
		math::memcpy_sse(matrix, cache.base(), sizeof(matrix3x4_t) * cache.count());
	}

	INLINE void set_bone_cache(matrix3x4_t* matrix) {
		auto& cache = bone_cache();
		math::memcpy_sse(cache.base(), matrix, sizeof(matrix3x4_t) * cache.count());
	}

	INLINE player_info_t get_player_info() {
		player_info_t info{};
		__interfaces->engine->get_player_info(this->index(), &info);
		return info;
	}

	INLINE std::string get_name(std::optional<std::string> new_string = {}) {
		if (new_string.has_value())
			return *new_string;

		return get_player_info().name;
	}

	INLINE void invalidate_bone_cache() {
		auto accessor = bone_accessor();
		if (!accessor)
			return;

		accessor->readable_bones = accessor->writable_bones = 0;

		std::uintptr_t cache = *FIND_OFFSET(__modules->client, OFFSET_INVALIDATE_BONE_CACHE).add(0xA).cast<std::uintptr_t*>();

		model_recent_bone_counter() = (*(std::uint32_t*)cache) - 1;
		last_bone_setup_time() = -FLT_MAX;
	}

	INLINE bool is_bot() {
		auto info = get_player_info();
		return info.fakeplayer;
	}

	INLINE bool is_teammate(bool check_local = true, c_cs_player* local = __local_player) {
		static auto mp_teammates_are_enemies = __convars->get(XOR("mp_teammates_are_enemies"));

		if (!local)
			return false;

		if (check_local && this == local)
			return true;

		if (mp_teammates_are_enemies && mp_teammates_are_enemies->get_int())
			return false;

		return this != local && team() == local->team();
	}

	INLINE bool is_armored(const int hitgroup) {
		bool is_armored = false;

		if (armor_value() > 0) {
			switch (hitgroup) {
			case HITGROUP_GENERIC:
			case HITGROUP_CHEST:
			case HITGROUP_STOMACH:
			case HITGROUP_LEFTARM:
			case HITGROUP_RIGHTARM:
			case HITGROUP_NECK:
				is_armored = true;
				break;
			case HITGROUP_HEAD:
				if (this->has_helmet())
					is_armored = true;
				[[fallthrough]];
			case HITGROUP_LEFTLEG:
			case HITGROUP_RIGHTLEG:
				if (this->has_heavy_armor())
					is_armored = true;
				break;
			default:
				break;
			}
		}

		return is_armored;
	}

	void interpolate_moveparent_pos();
	void update_weapon_dispatch_layers();
	float studio_set_pose_parameter(int index, float value, float& ctl_value);
	float studio_get_pose_parameter(int index, float value);
	vec3_t get_hitbox_position(int hitbox, matrix3x4_t* matrix = nullptr);
	int get_ping();
	void run_pre_think();
	void run_think();
	void run_post_think();
};

class c_base_combat_weapon : public c_base_entity {
public:
	OFFSET(activity, int&, 0x32A4);

	NETVAR(original_owner_xuid_high, int&, "DT_BaseAttributableItem", "m_OriginalOwnerXuidHigh");
	NETVAR(original_owner_xuid_low, int&, "DT_BaseAttributableItem", "m_OriginalOwnerXuidLow");
	NETVAR(fallback_stat_trak, int&, "DT_BaseAttributableItem", "m_nFallbackStatTrak");
	NETVAR(fallback_paint_kit, int&, "DT_BaseAttributableItem", "m_nFallbackPaintKit");
	NETVAR(entity_quality, int&, "DT_BaseAttributableItem", "m_iEntityQuality");
	NETVAR(fallback_seed, int&, "DT_BaseAttributableItem", "m_nFallbackSeed");
	NETVAR(item_id_high, int&, "DT_BaseAttributableItem", "m_iItemIDHigh");
	NETVAR(account_id, int&, "DT_BaseAttributableItem", "m_iAccountID");
	NETVAR(zoom_level, int, "DT_WeaponCSBaseGun", "m_zoomLevel");
	NETVAR(clip1, int, "DT_BaseCombatWeapon", "m_iClip1");

	NETVAR_PTR(custom_name, char, "DT_BaseAttributableItem", "m_szCustomName");

	NETVAR(next_secondary_attack, float, "DT_BaseCombatWeapon", "m_flNextSecondaryAttack");
	NETVAR(burst_shots_remaining, int, "DT_WeaponCSBaseGun", "m_iBurstShotsRemaining");
	NETVAR(next_primary_attack, float, "DT_BaseCombatWeapon", "m_flNextPrimaryAttack");
	NETVAR(next_burst_shot, float, "CWeaponCSBaseGun", "m_fNextBurstShot");
	NETVAR(last_shot_time, float, "DT_WeaponCSBase", "m_fLastShotTime");
	NETVAR(throw_time, float, "DT_BaseCSGrenade", "m_fThrowTime");

	NETVAR(postpone_fire_ready_time, float&, "DT_WeaponCSBase", "m_flPostponeFireReadyTime");
	NETVAR(accuracy_penalty, float&, "DT_WeaponCSBase", "m_fAccuracyPenalty");
	NETVAR(throw_strength, float&, "DT_WeaponCSBase", "m_flThrowStrength");
	NETVAR(fallback_wear, float&, "DT_BaseAttributableItem", "m_flFallbackWear");
	NETVAR(recoil_index, float&, "DT_WeaponCSBase", "m_flRecoilIndex");

	NETVAR(item_definition_index, short&, "DT_BaseAttributableItem", "m_iItemDefinitionIndex");

	NETVAR(initalized, bool&, "DT_WearableItem", "m_bInitialized");
	NETVAR(pin_pulled, bool, "DT_BaseCSGrenade", "m_bPinPulled");
	NETVAR(started_arming, bool, "DT_WeaponC4", "m_bStartedArming");

	NETVAR_PTR(thrower, c_cs_player, "DT_BaseCSGrenade", "m_hThrower");

	NETVAR(weapon_world_model, c_base_handle, "DT_BaseCombatWeapon", "m_hWeaponWorldModel");

	VFUNC(get_inaccuracy(), float(__thiscall*)(decltype(this)), GET_INACCURACY_FUNC);
	VFUNC(get_spread(), float(__thiscall*)(decltype(this)), GET_SPREAD_FUNC);
	VFUNC(get_zoom_fov(int level), int(__thiscall*)(decltype(this), int), GET_ZOOM_FOV_VFUNC, level);
	VFUNC(get_zoom_time(int level), float(__thiscall*)(decltype(this), int), GET_ZOOM_TIME_VFUNC, level);
	VFUNC(update_accuracy_penalty(), void(__thiscall*)(decltype(this)), UPDATE_ACCURACY_PENALTY_FUNC);

	INLINE bool is_heavy_pistols() {
		short idx = item_definition_index();
		return idx == WEAPON_DEAGLE || idx == WEAPON_REVOLVER;
	}

	INLINE bool is_pistols() {
		short idx = item_definition_index();
		auto info = __interfaces->weapon_system->get_weapon_data(idx);
		if (!info)
			return false;

		int type = info->weapon_type;
		return type == WEAPONTYPE_PISTOL && !is_heavy_pistols();
	}

	INLINE bool is_knife() {
		short idx = item_definition_index();
		auto info = __interfaces->weapon_system->get_weapon_data(idx);
		if (!info)
			return false;

		return info->weapon_type == WEAPONTYPE_KNIFE && idx != WEAPON_TASER;
	}

	INLINE bool is_grenade() {
		short idx = item_definition_index();
		return idx >= WEAPON_FLASHBANG && idx <= WEAPON_INCGRENADE;
	}

	INLINE bool is_rifle() {
		short idx = item_definition_index();
		auto info = __interfaces->weapon_system->get_weapon_data(idx);
		if (!info)
			return false;

		return info->weapon_type == WEAPONTYPE_RIFLE;
	}

	INLINE bool is_taser() {
		short idx = item_definition_index();
		auto info = __interfaces->weapon_system->get_weapon_data(idx);
		if (!info)
			return false;

		int type = info->weapon_type;
		return idx == WEAPON_TASER;
	}

	INLINE bool is_misc_weapon() {
		short idx = item_definition_index();
		auto info = __interfaces->weapon_system->get_weapon_data(idx);
		if (!info)
			return false;

		int type = info->weapon_type;
		return idx == WEAPON_TASER || type == WEAPONTYPE_KNIFE || type >= WEAPONTYPE_C4 && type <= WEAPONTYPE_MELEE;
	}

	INLINE bool is_auto_sniper() {
		short idx = item_definition_index();

		switch (idx) {
		case WEAPON_G3SG1:
		case WEAPON_SCAR20:
			return true;
		default:
			return false;
		}
	}

	INLINE bool is_sniper() {
		short idx = item_definition_index();

		switch (idx) {
		case WEAPON_AWP:
		case WEAPON_G3SG1:
		case WEAPON_SCAR20:
		case WEAPON_SSG08:
			return true;
		default:
			return false;
		}
	}

	INLINE bool is_scoping_weapon() {
		short idx = item_definition_index();

		switch (idx) {
		case WEAPON_AWP:
		case WEAPON_G3SG1:
		case WEAPON_SCAR20:
		case WEAPON_SSG08:
		case WEAPON_SG556:
		case WEAPON_AUG:
			return true;
		default:
			return false;
		}
	}

	INLINE std::string get_weapon_name() {
		auto replace = [&](std::string& str, const std::string& from, const std::string& to) {
			size_t start_pos = str.find(from);
			if (start_pos == std::string::npos)
				return false;
			str.replace(start_pos, from.length(), to);
			return true;
		};

		short idx = item_definition_index();
		auto weapon_info = __interfaces->weapon_system->get_weapon_data(idx);
		if (!weapon_info)
			return "";

		// skip SFUI_WEAPON
		std::string name = weapon_info->hud_name + 13;

		std::transform(name.begin(), name.end(), name.begin(), ::toupper);

		replace(name, "_", " ");
		return name;
	}

	INLINE const char8_t* get_weapon_icon() {
		switch (item_definition_index()) {
		case WEAPON_DEAGLE:
			return u8"\uE001";

		case WEAPON_ELITE:
			return u8"\uE002";

		case WEAPON_FIVESEVEN:
			return u8"\uE003";

		case WEAPON_GLOCK:
			return u8"\uE004";

		case WEAPON_AK47:
			return u8"\uE007";

		case WEAPON_AUG:
			return u8"\uE008";

		case WEAPON_AWP:
			return u8"\uE009";

		case WEAPON_FAMAS:
			return u8"\uE00A";

		case WEAPON_G3SG1:
			return u8"\uE00B";

		case WEAPON_GALILAR:
			return u8"\uE00D";

		case WEAPON_M249:
			return u8"\uE00E";

		case WEAPON_M4A1:
			return u8"\uE010";

		case WEAPON_MAC10:
			return u8"\uE011";

		case WEAPON_P90:
			return u8"\uE013";

		case WEAPON_MP5SD:
			return u8"\uE017";

		case WEAPON_UMP45:
			return u8"\uE018";

		case WEAPON_XM1014:
			return u8"\uE019";

		case WEAPON_BIZON:
			return u8"\uE01A";

		case WEAPON_MAG7:
			return u8"\uE01B";

		case WEAPON_NEGEV:
			return u8"\uE01C";

		case WEAPON_SAWEDOFF:
			return u8"\uE01D";

		case WEAPON_TEC9:
			return u8"\uE01E";

		case WEAPON_TASER:
			return u8"\uE01F";

		case WEAPON_HKP2000:
			return u8"\uE020";

		case WEAPON_MP7:
			return u8"\uE021";

		case WEAPON_MP9:
			return u8"\uE022";

		case WEAPON_NOVA:
			return u8"\uE023";

		case WEAPON_P250:
			return u8"\uE024";

		case WEAPON_SCAR20:
			return u8"\uE026";

		case WEAPON_SG556:
			return u8"\uE027";

		case WEAPON_SSG08:
			return u8"\uE028";

		case WEAPON_KNIFE:
			return u8"\uE02A";

		case WEAPON_FLASHBANG:
			return u8"\uE02B";

		case WEAPON_HEGRENADE:
			return u8"\uE02C";

		case WEAPON_SMOKEGRENADE:
			return u8"\uE02D";

		case WEAPON_MOLOTOV:
		case WEAPON_FIREBOMB:
			return u8"\uE02E";

		case WEAPON_DECOY:
		case WEAPON_DIVERSION:
			return u8"\uE02F";

		case WEAPON_INCGRENADE:
			return u8"\uE030";

		case WEAPON_C4:
			return u8"\uE031";

		case WEAPON_HEALTHSHOT:
			return u8"\uE039";

		case WEAPON_KNIFE_GG:
		case WEAPON_KNIFE_T:
			return u8"\uE03B";

		case WEAPON_M4A1_SILENCER:
			return u8"\uE03C";

		case WEAPON_USP_SILENCER:
			return u8"\uE03D";

		case WEAPON_CZ75A:
			return u8"\uE03F";

		case WEAPON_REVOLVER:
			return u8"\uE040";

		case WEAPON_TAGRENADE:
			return u8"\uE044";

		case WEAPON_FISTS:
			return u8"\uE045";

		case WEAPON_TABLET:
			return u8"\uE048";

		case WEAPON_MELEE:
			return u8"\uE04A";

		case WEAPON_AXE:
			return u8"\uE04B";

		case WEAPON_HAMMER:
			return u8"\uE04C";

		case WEAPON_SPANNER:
			return u8"\uE04E";

		case WEAPON_KNIFE_BAYONET:
			return u8"\uE1F4";

		case WEAPON_KNIFE_CSS:
			return u8"\uE1F7";

		case WEAPON_KNIFE_FLIP:
			return u8"\uE1F9";

		case WEAPON_KNIFE_GUT:
			return u8"\uE1FA";

		case WEAPON_KNIFE_KARAMBIT:
			return u8"\uE1FB";

		case WEAPON_KNIFE_M9_BAYONET:
			return u8"\uE1FC";

		case WEAPON_KNIFE_TACTICAL:
			return u8"\uE1FD";

		case WEAPON_KNIFE_FALCHION:
			return u8"\uE200";

		case WEAPON_KNIFE_SURVIVAL_BOWIE:
			return u8"\uE202";

		case WEAPON_KNIFE_BUTTERFLY:
			return u8"\uE203";

		case WEAPON_KNIFE_PUSH:
			return u8"\uE204";

		case WEAPON_KNIFE_CORD:
			return u8"\uE205";

		case WEAPON_KNIFE_CANIS:
			return u8"\uE206";

		case WEAPON_KNIFE_URSUS:
			return u8"\uE207";

		case WEAPON_KNIFE_GYPSY_JACKKNIFE:
			return u8"\uE208";

		case WEAPON_KNIFE_OUTDOOR:
			return u8"\uE209";

		case WEAPON_KNIFE_STILETTO:
			return u8"\uE20A";

		case WEAPON_KNIFE_WIDOWMAKER:
			return u8"\uE20B";

		case WEAPON_KNIFE_SKELETON:
			return u8"\uE20D";
		default:
			return u8"\u003F";
		}
	}

	INLINE const char* get_grenade_name(const int& class_id) {
		model_t* model = get_model();
		if (!model)
			return "";

		switch (class_id) {
		case CBaseCSGrenadeProjectile:
			return std::strstr(model->name, CXOR("flashbang")) ? CXOR("FLASH") : CXOR("HE GRENADE");

#ifndef LEGACY
		case CBreachChargeProjectile:
			return CXOR("BREACH");

		case CBumpMineProjectile:
			return CXOR("MINE");
#endif
		case CDecoyProjectile:
			return CXOR("DECOY");

		case CMolotovProjectile:
		case CSensorGrenadeProjectile:
			return CXOR("SENSOR");

		case CSmokeGrenadeProjectile:
			return CXOR("SMOKE");

#ifndef LEGACY
		case CSnowballProjectile:
			return CXOR("SNOW");
#endif
		}
		return CXOR("UNK");
	}
};