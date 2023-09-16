#include "../../callbacks.hpp"
#include "../../main_definitions.hpp"
#include "../../security/security.hpp"
#include "../../sdk/sdk.hpp"
#include "server_bones.hpp"
#include "ik_solver.hpp"
#include "animation_fix.hpp"

// pasted from legendware that was pasted from eexomi
// works P since 2021

namespace bone_merge {
	std::uintptr_t& get_bone_merge(c_cs_player* player) {
		return *reinterpret_cast<std::uintptr_t*>(reinterpret_cast<std::uintptr_t>(player)
			+ *FIND_OFFSET(__modules->client, OFFSET_GET_BONE_MERGE).cast<std::uintptr_t*>());
	}

	void update_cache(std::uintptr_t& bone_merge) {
		FIND_OFFSET(__modules->client, OFFSET_UPDATE_MERGE_CACHE).relative(1).cast<void(__thiscall*)(std::uintptr_t)>()(bone_merge);
	}
}

void merge_matching_poses(std::uintptr_t& bone_merge, float* poses, float* target_poses) {
	bone_merge::update_cache(bone_merge);

	if (*reinterpret_cast<std::uintptr_t*>(bone_merge + 0x10) && *reinterpret_cast<std::uintptr_t*>(bone_merge + 0x8C)) {
		int* index = reinterpret_cast<int*>(bone_merge + 0x20);
		for (int i = 0; i < 24; ++i) {
			if (*index != -1) {
				c_cs_player* target = *(c_cs_player**)(bone_merge + 0x4);
				c_studio_hdr* hdr = target->get_studio_hdr();
				float pose_param_value = 0.f;

				if (hdr && *(studiohdr_t**)hdr && i >= 0) {
					float pose = target_poses[i];
					auto pose_param = target->get_pose_parameter(hdr, i);

					pose_param_value = pose * (pose_param->end - pose_param->start) + pose_param->start;
				}

				c_cs_player* second_target = *(c_cs_player**)(bone_merge);
				c_studio_hdr* second_hdr = second_target->get_studio_hdr();

				poses[*index] = second_target->studio_get_pose_parameter(*index, pose_param_value);
			}

			++index;
		}
	}
}

void clamp_bones_info_t::store(c_cs_player* player) {
	auto collideable = player->get_collideable();
	if (!collideable)
		return;

	collision_change_time = player->collision_change_time();
	collision_change_origin = player->collision_change_origin();

	auto& base_origin = player == __local_player ? player->get_abs_origin() : player->origin();
	origin = base_origin;
	view_offset = player->view_offset();
	collision_origin = base_origin;
	maxs = collideable->get_maxs();

	ground_entity = player->ground_entity().get().cast<c_base_entity*>();
}

void clamp_bones_info_t::store(animation_record_t* record) {
	collision_change_time = record->collision_change_time;
	collision_change_origin = record->collision_change_origin;

	origin = record->origin;
	view_offset = record->view_offset;
	collision_origin = record->origin;
	maxs = record->maxs;

	ground_entity = record->ground_entity;
}

void c_bone_builder::clamp_bones_in_bbox(c_cs_player* player, matrix3x4_t* matrix, int mask, float curtime, vec3_t& eye_angles, clamp_bones_info_t& clamp_info) {
	auto hdr = player->get_studio_hdr();
	if (!hdr)
		return;

	auto studiohdr = *reinterpret_cast<studiohdr_t**>(hdr);
	if (!studiohdr)
		return;

	auto head_bone = player->lookup_bone(CXOR("head_0"));
	if (head_bone < 0)
		return;

	auto collision_origin = clamp_info.collision_origin;
	auto collision_bb_max = clamp_info.maxs;

	auto max_collision_pos_z = collision_origin.z + collision_bb_max.z;
	auto change_collision_time_diff = curtime - clamp_info.collision_change_time;
	auto new_position_z = max_collision_pos_z;

	if (change_collision_time_diff < 0.2f) {
		auto time_multiplier = std::clamp(change_collision_time_diff * 5.f, 0.f, 1.f);

		new_position_z = math::interpolate_inversed(time_multiplier, clamp_info.collision_change_origin, max_collision_pos_z);
	}

	auto head_bone_position = matrix[head_bone].get_origin();

	vec3_t eye_position{}, forward{}, right{}, up{};

	if (player != __local_player)
		eye_position = clamp_info.origin + clamp_info.view_offset;
	else
		FIND_OFFSET(__modules->client, OFFSET_WEAPON_SHOOTPOS).cast<float* (__thiscall*)(void*, vec3_t*)>()(player, &eye_position);

	math::angle_vectors(eye_angles, forward, right, up);

	vec3_t head_position_to_change{};
	auto target_difference = head_bone_position.dot(right) - eye_position.dot(right);
	if (std::fabs(target_difference) <= 3.f) {
		head_position_to_change = head_bone_position;
	}
	else {
		auto head_direction = 0.f;
		if (target_difference < 0.f)
			head_direction = -1.f;
		else
			head_direction = 1.f;

		auto adjusted_right_direction = (right.normalized() * 3.f) * head_direction;
		head_position_to_change = (head_bone_position - (right * target_difference)) + adjusted_right_direction;
	}

	if (head_position_to_change.z > (new_position_z - 4.f))
		head_position_to_change.z = new_position_z - 4.f;

	auto head_position_delta = vec3_t{ head_position_to_change.x - eye_position.x, head_position_to_change.y - eye_position.y, 0.f };
	if (head_position_delta.length() > 11.f) {
		auto normalized_delta = head_position_delta.normalized();

		head_position_to_change.x = (normalized_delta.x * 11.f) + eye_position.x;
		head_position_to_change.y = (normalized_delta.y * 11.f) + eye_position.y;
	}

	auto head_pos_difference = head_bone_position - head_position_to_change;
	if (!head_pos_difference.valid() || head_pos_difference.length_sqr() >= 900.f)
		return;

	auto left_ankle_bone = player->lookup_bone(CXOR("ankle_L"));
	auto right_ankle_bone = player->lookup_bone(CXOR("ankle_R"));
	auto spine_3_bone = player->lookup_bone(CXOR("spine_3"));

	mstudioikchain_t* left_chain = nullptr;
	mstudioikchain_t* right_chain = nullptr;

	for (int i = 0; i < studiohdr->numikchains; i++) {
		mstudioikchain_t* ik_chain = studiohdr->ik_chain(i);
		const mstudioiklink_t* ik_link = ik_chain->link(2);

		if (ik_link->bone == left_ankle_bone)
			left_chain = ik_chain;
		else if (ik_link->bone == right_ankle_bone)
			right_chain = ik_chain;

		if (left_chain && right_chain)
			break;
	}

	if (studiohdr->bones <= 0)
		return;

	auto left_ankle_bone_position = matrix[left_ankle_bone].get_origin();
	auto right_ankle_bone_position = matrix[right_ankle_bone].get_origin();
	auto lowest_ankle_position_z = std::min(left_ankle_bone_position.z, right_ankle_bone_position.z);

	auto solve_left_ik = false;
	auto solve_right_ik = false;

	auto ground_entity = clamp_info.ground_entity;

	for (int i = 0; i < studiohdr->bones; ++i) {
		auto& current_bone = matrix[i];
		auto current_bone_origin = current_bone.get_origin();

		if (!(hdr->bone_flags()[i] & mask))
			continue;

		if (i == left_ankle_bone && left_chain && ground_entity) {
			solve_left_ik = true;
			continue;
		}
		else if (i == right_ankle_bone && right_chain && ground_entity) {
			solve_right_ik = true;
			continue;
		}

		auto head_adjust = 1.f;
		auto height_amount = 0.f;
		auto bone_parent = -1;

		if (ground_entity) {
			if (hdr->bone_parent_count()) {
				if (i >= 0 && spine_3_bone >= 0) {
					auto studio_hdr_mat_iter = *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(hdr) + 0x44);
					bone_parent = *reinterpret_cast<int*>(studio_hdr_mat_iter + 4 * i);
					if (bone_parent != -1) {
						while (bone_parent != spine_3_bone) {
							bone_parent = *reinterpret_cast<int*>(studio_hdr_mat_iter + 4 * bone_parent);
							if (bone_parent == -1)
								goto LABEL_56;
						}
						goto LABEL_63;
					}
				}
			}
		LABEL_56:
			auto height_diff = current_bone_origin.z - (head_bone_position.z - head_position_to_change.z);
			if (lowest_ankle_position_z == head_bone_position.z) {
				if ((height_diff - head_bone_position.z) >= 0.f) {
					head_adjust = 1.f;
					goto LABEL_62;
				}
			LABEL_60:
				head_adjust = 0.f;
			}
			else
			{
				auto height_lerp = (height_diff - lowest_ankle_position_z) / (head_bone_position.z - lowest_ankle_position_z);
				if (height_lerp < 0.f)
					goto LABEL_60;
				head_adjust = std::min(height_lerp, 1.f);
			}
		LABEL_62:
			height_amount = head_bone_position.z - head_position_to_change.z;
		}
	LABEL_63:
		current_bone_origin -= (head_pos_difference * head_adjust);
		current_bone.set_origin(current_bone_origin);
	}

	if (solve_left_ik)
		studio_solve_ik(left_chain->link(0)->bone, left_chain->link(1)->bone, left_ankle_bone, left_ankle_bone_position, matrix);

	if (solve_right_ik)
		studio_solve_ik(right_chain->link(0)->bone, right_chain->link(1)->bone, right_ankle_bone, right_ankle_bone_position, matrix);
}

bool c_bone_builder::can_be_animated(c_cs_player* player) {
	if (!player->use_new_animstate() || !player->animstate())
		return false;

	auto weapon = player->active_weapon().get().cast<c_base_combat_weapon*>();

	if (!weapon)
		return false;

	auto world_model = weapon->weapon_world_model().get().cast<c_cs_player*>();
	if (!world_model || *reinterpret_cast<short*>(reinterpret_cast<std::uintptr_t>(world_model) + 0x26E) == -1)
		return player == __local_player;

	return true;
}

void c_bone_builder::get_skeleton(vec3_t* position, quaternion_t* q) {
	alignas(16) vec3_t new_position[128]{};
	alignas(16) quaternion_t new_q[128]{};

	auto ik_context = reinterpret_cast<c_ik_context*>(animating->ik_ctx());

	if (!ik_ctx)
		ik_context = nullptr;

	alignas(16) char buffer[32];
	alignas(16) bone_setup_t* bone_setup = (bone_setup_t*)&buffer;

	bone_setup->hdr = hdr;
	bone_setup->mask = mask;
	bone_setup->pose_parameter = poses.data();
	bone_setup->pose_debugger = nullptr;

	bone_setup->init_pose(position, q, hdr);
	bone_setup->accumulate_pose(position, q, animating->sequence(), animating->cycle(), 1.f, time, ik_context);

	int layer[13]{};

	for (int i = 0; i < layer_count; ++i) {
		c_animation_layers& final_layer = layers[i];

		if (final_layer.weight > 0.f && final_layer.order != 13 && final_layer.order >= 0 && final_layer.order < layer_count)
			layer[final_layer.order] = i;
	}

	char tmp_buffer[4208]{};
	auto world_ik = reinterpret_cast<c_ik_context*>(tmp_buffer);

	auto weapon = animating->active_weapon().get().cast<c_base_combat_weapon*>();
	c_cs_player* world_weapon = nullptr;
	if (weapon)
		world_weapon = weapon->weapon_world_model().get().cast<c_cs_player*>();

	auto wrong_weapon = [&]() {
		if (can_be_animated(animating) && world_weapon) {
			auto bone_merge = bone_merge::get_bone_merge(world_weapon);
			if (bone_merge) {
				merge_matching_poses(bone_merge, poses_world.data(), poses.data());

				auto world_hdr = world_weapon->get_studio_hdr();

				world_ik->constructor();
				world_ik->init(world_hdr, &angles, &origin, time, 0, BONE_USED_BY_BONE_MERGE);

				alignas(16) char buffer2[32]{};
				alignas(16) bone_setup_t* world_setup = reinterpret_cast<bone_setup_t*>(&buffer2);

				world_setup->hdr = world_hdr;
				world_setup->mask = BONE_USED_BY_BONE_MERGE;
				world_setup->pose_parameter = poses_world.data();
				world_setup->pose_debugger = nullptr;

				world_setup->init_pose(new_position, new_q, world_hdr);

				for (int i = 0; i < layer_count; ++i) {
					c_animation_layers* layer = &layers[i];

					if (layer && layer->sequence > 1 && layer->weight > 0.f) {
						if (dispatch && animating == __local_player)
							animating->update_dispatch_layer(layer, world_hdr, layer->sequence);

						if (!dispatch || layer->second_dispatch_sequence <= 0 || layer->second_dispatch_sequence >= (*reinterpret_cast<studiohdr_t**>(world_hdr))->local_seq)
							bone_setup->accumulate_pose(position, q, layer->sequence, layer->cycle, layer->weight, time, ik_context);
						else if (dispatch) {
							static auto copy_from_follow = FIND_OFFSET(__modules->client, OFFSET_COPY_FROM_FOLLOW).relative(1).cast<void(__thiscall*)(std::uintptr_t, vec3_t*, quaternion_t*, int, vec3_t*, quaternion_t*)>();
							static auto add_dependencies = FIND_OFFSET(__modules->client, OFFSET_ADD_DEPENDENCIES).cast<void(__thiscall*)(c_ik_context*, float, int, float, const float[], float)>();
							static auto copy_to_follow = FIND_OFFSET(__modules->client, OFFSET_COPY_TO_FOLLOW).relative(1).cast<void(__thiscall*)(std::uintptr_t, vec3_t*, quaternion_t*, int, vec3_t*, quaternion_t*)>();

							copy_from_follow(bone_merge, position, q, BONE_USED_BY_BONE_MERGE, new_position, new_q);
							if (ik_context)
								add_dependencies(ik_context, *reinterpret_cast<float*>(reinterpret_cast<std::uintptr_t>(animating) + 0xA14), layer->sequence, layer->cycle, poses.data(), layer->weight);

							world_setup->accumulate_pose(new_position, new_q, layer->second_dispatch_sequence, layer->cycle, layer->weight, time, world_ik);
							copy_to_follow(bone_merge, new_position, new_q, BONE_USED_BY_BONE_MERGE, position, q);
						}
					}
				}

				world_ik->destructor();
				return false;
			}
		}
		return true;
	};

	if (wrong_weapon()) {
		for (int i = 0; i < this->layer_count; ++i) {
			int layer_count = layer[i];

			if (layer_count >= 0 && layer_count < this->layer_count) {
				c_animation_layers* final_layer = &layers[i];
				bone_setup->accumulate_pose(position, q, final_layer->sequence, final_layer->cycle, final_layer->weight, time, ik_context);
			}
		}
	}

	if (ik_context) {
		world_ik->constructor();
		world_ik->init(hdr, &angles, &origin, time, 0, mask);
		bone_setup->calc_autoplay_sequences(position, q, time, world_ik);
		world_ik->destructor();
	}
	else
		bone_setup->calc_autoplay_sequences(position, q, time, nullptr);

	bone_setup->calc_bone_adjust(position, q, reinterpret_cast<float*>(reinterpret_cast<std::uintptr_t>(animating) + 0xA54), mask);
}

void c_bone_builder::studio_build_matrices(c_studio_hdr* hdr, const matrix3x4_t& world_transform, vec3_t* pos, quaternion_t* q, int mask, matrix3x4_t* out, uint32_t* bone_computed) {
	int i = 0;
	int chain_length = 0;
	int bone = -1;
	auto studio_hdr = *reinterpret_cast<studiohdr_t**>(hdr);

	if (bone < -1 || bone >= studio_hdr->bones)
		bone = 0;

	c_utl_vector<int>* bone_parent = reinterpret_cast<c_utl_vector<int>*>(reinterpret_cast<std::uintptr_t>(hdr) + 0x44);
	c_utl_vector<int>* bone_flags = reinterpret_cast<c_utl_vector<int>*>(reinterpret_cast<std::uintptr_t>(hdr) + 0x30);

	int chain[128]{};
	if (bone <= -1) {
		chain_length = studio_hdr->bones;

		for (i = 0; i < studio_hdr->bones; ++i)
			chain[chain_length - i - 1] = i;
	}
	else {
		i = bone;

		do {
			chain[chain_length++] = i;
			i = bone_parent->element(i);
		} while (i != -1);
	}

	matrix3x4_t bone_matrix{};
	for (int j = chain_length - 1; j >= 0; --j) {
		i = chain[j];

		if ((1 << (i & 0x1F)) & bone_computed[i >> 5])
			continue;

		int flag = bone_flags->element(i);
		int parent = bone_parent->element(i);

		if ((flag & mask) && q) {
			bone_matrix.quaternion_matrix(q[i], pos[i]);

			if (parent == -1)
				out[i] = world_transform.contact_transforms(bone_matrix);
			else
				out[i] = out[parent].contact_transforms(bone_matrix);
		}
	}
}

void c_bone_builder::store(c_cs_player* player, matrix3x4_t* matrix, int mask) {
	auto state = player->animstate();

	animating = player;
	origin = player == __local_player ? player->get_abs_origin() : player->origin();
	layers = player->animlayers();
	hdr = player->get_studio_hdr();
	layer_count = 13;
	angles = player->get_abs_angles();
	this->matrix = matrix;
	this->mask = mask;
	time = TICKS_TO_TIME(__local_player->tickbase());
	attachments = false;
	ik_ctx = false;
	dispatch = true;
	 
	player->store_poses(poses.data());

	eye_angles = player->eye_angles();

	auto weapon = player->active_weapon().get().cast<c_base_combat_weapon*>();

	c_base_combat_weapon* world_weapon = nullptr;
	if (weapon)
		world_weapon = weapon->weapon_world_model().get().cast<c_base_combat_weapon*>();

	if (world_weapon)
		std::memcpy(poses_world.data(), world_weapon->pose_parameter(), sizeof(float) * 24);
	else
		std::memcpy(poses_world.data(), player->pose_parameter(), sizeof(float) * 24);

	filled = true;
}

void c_bone_builder::setup() {
	alignas(16) vec3_t position[128]{ };
	alignas(16) quaternion_t q[128]{ };

	auto ik_context = (c_ik_context*)animating->ik_ctx();

	if (!ik_ctx)
		ik_context = nullptr;

	hdr = animating->get_studio_hdr();

	if (!hdr)
		return;

	std::uint32_t bone_computed[8]{};
	std::memset(bone_computed, 0, 8 * sizeof(std::uint32_t));

	bool sequences_available = !*reinterpret_cast<int*>(*reinterpret_cast<std::uintptr_t*>(hdr) + 0x150) || *reinterpret_cast<int*>(*reinterpret_cast<std::uintptr_t*>(hdr) + 0x4);

	if (ik_context) {
		ik_context->init(hdr, &angles, &origin, time, TIME_TO_TICKS(time), mask);

		if (sequences_available)
			get_skeleton(position, q);

		animating->update_ik_locks(time);
		ik_context->update_targets(position, q, matrix, (std::uint8_t*)bone_computed);
		animating->calc_ik_locks(time);
		ik_context->solve_dependencies(position, q, matrix, (std::uint8_t*)bone_computed);
	}
	else if (sequences_available)
		get_skeleton(position, q);

	matrix3x4_t transform{};
	transform.angle_matrix(angles, origin);

	studio_build_matrices(hdr, transform, position, q, mask, matrix, bone_computed);

	if (mask & BONE_USED_BY_ATTACHMENT && attachments)
		animating->attachments_helper();

	animating->last_bone_setup_time() = time;

	animating->bone_accessor()->readable_bones |= mask;
	animating->bone_accessor()->writable_bones |= mask;

	std::uintptr_t cache = *FIND_OFFSET(__modules->client, OFFSET_INVALIDATE_BONE_CACHE).add(0xA).cast<std::uintptr_t*>();
	animating->model_recent_bone_counter() = *reinterpret_cast<std::uint32_t*>(cache);

#ifndef LEGACY
	//FIND_OFFSET(__modules->client, OFFSET_CLAMP_BONES_IN_BBOX).relative(1).cast<void(__thiscall*)(void*, matrix3x4_t*, int)>()(animating, matrix, 0x7FF00);

	if (animating == __local_player) {
		clamp_bones_info_t info{};
		info.store(animating);

		clamp_bones_in_bbox(animating, matrix, mask, time, animating->eye_angles(), info);
	}
#endif
}