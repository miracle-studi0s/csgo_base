#include "hooks.hpp"
#include "../sdk/sdk.hpp"
#include "../render/renderer.hpp"
#include "../cheat.hpp"
#include "../features/features.hpp"

#define ADD_VMT(hook_interface, hook_index, hook_func) \
    add_hook(hook_func, __memory->get_virtual(hook_interface, hook_index).cast<void*>())

enum indices_t : int {
	// client hooks
	FSN_IDX = LEGACY_DESYNC_VFUNC(36, 37),
	CL_CREATE_MOVE_IDX = LEGACY_DESYNC_VFUNC(21, 22),
	LEVEL_INIT_PRE_ENTITY_IDX = 5,
	LEVEL_INIT_POST_ENTITY_IDX = 6,
	LEVEL_INIT_SHUTDOWN_IDX = 7,
	WRITE_USER_CMD_DELTA_TO_BUFFER_IDX = LEGACY_DESYNC_VFUNC(23, 24),

	// d3d hooks
	PRESENT_IDX = 17,
	RESET_IDX = 16,

	// vgui panel hooks
	PAINT_TRAVERSE_IDX = 41,

	// client mode hooks
	OVERRIDE_VIEW_IDX = 18,
	CREATE_MOVE_IDX = 24,
	DO_POST_SCREEN_EFFECTS_IDX = 44,

	// engine hooks
	IS_HLTV_IDX = 93,
	IS_CONNECTED_IDX = 27,

	// bsp query hooks
	LIST_LEAVES_IN_BOX_IDX = 6,

	// client state hooks
	PACKET_START_IDX = 5,
	PACKET_END_IDX = 6,

	// prediction hooks
	RUN_COMMAND_IDX = 19,

	// game movement hooks
	PROCESS_MOVEMENT_IDX = 1,

	// key values system hook
	ALLOC_KEY_VALUES_IDX = 2,

	// surface hook
	ON_SCREEN_SIZE_CHANGED = 116,

	// engine sound hook
	EMIT_SOUND = 5,
};

void __stdcall frame_stage_notify(int stage) {
	static auto original = __hooks->get_original(&frame_stage_notify);

#ifdef _DEBUG
	if (__cheat->unload)
		return original(stage);
#endif

	c_feature::exec_frame(stage);

	original(stage);

	c_feature::exec_frame_post(stage);
	c_render::exec_frame_post(stage);
}

void __fastcall cl_create_move_proxy(void* _this, int, int sequence_number, float input_sample_frametime, bool active);

void __stdcall cl_create_move(int sequence_number, float input_sample_frametime, bool active, bool& send_packet) {
	static auto original = __hooks->get_original(&cl_create_move_proxy);

	c_feature::exec_tick(&send_packet);
	original(__interfaces->client, 0, sequence_number, input_sample_frametime, active);
	c_feature::exec_tick_end(&send_packet, sequence_number);
}

__declspec(naked) void __fastcall cl_create_move_proxy(void* _this, int, int sequence_number, float input_sample_frametime, bool active) {
	__asm
	{
		push ebp
		mov ebp, esp
		push ebx
		push esp
		push dword ptr[active]
		push dword ptr[input_sample_frametime]
		push dword ptr[sequence_number]
		call cl_create_move
		pop ebx
		pop ebp
		retn 0Ch
	}
}

void __fastcall level_init_pre_entity(void* ecx, void* edx, const char* map) {
	static auto original = __hooks->get_original(&level_init_pre_entity);

	c_sdk::exec_map_init_pre();
	c_feature::exec_map_init_pre();

	original(ecx, edx, map);
}

void __fastcall level_init_post_entity(void* ecx, void* edx) {
	static auto original = __hooks->get_original(&level_init_post_entity);

	c_sdk::exec_map_init_post();
	c_feature::exec_map_init_post();

	original(ecx, edx);
}

void __fastcall level_shutdown(void* ecx, void* edx) {
	static auto original = __hooks->get_original(&level_shutdown);

	c_feature::exec_map_shutdown();

	original(ecx, edx);
}

bool __fastcall write_usercmd_to_delta_buffer(void* ecx, void* edx, int slot, void* buf, int from, int to, bool isnewcommand) {
	static auto original = __hooks->get_original(&write_usercmd_to_delta_buffer);
	return original(ecx, edx, slot, buf, from, to, isnewcommand);
}

HRESULT __stdcall present(c_d3d_device* device, void* a, void* b, void* c, void* d) {
	static auto original = __hooks->get_original(&present);

#ifdef _DEBUG
	if (__cheat->unload)
		return original(device, a, b, c, d);
#endif

	IDirect3DStateBlock9* d3d9_state_block = nullptr;
	if (device->CreateStateBlock(D3DSBT_PIXELSTATE, &d3d9_state_block) < 0)
		return original(device, a, b, c, d);

	static bool init = false;
	if (!init) {
		c_render::exec_init();
		init = true;
	}

	DWORD colorwrite{}, srgbwrite{};
	IDirect3DVertexDeclaration9* vert_dec = nullptr;
	IDirect3DVertexShader9* vert_shader = nullptr;
	DWORD dwOld_D3DRS_COLORWRITEENABLE = NULL;
	device->GetRenderState(D3DRS_COLORWRITEENABLE, &colorwrite);
	device->GetRenderState(D3DRS_SRGBWRITEENABLE, &srgbwrite);

	device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
	device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);

	device->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
	//removes the source engine color correction
	device->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

	device->GetRenderState(D3DRS_COLORWRITEENABLE, &dwOld_D3DRS_COLORWRITEENABLE);
	device->GetVertexDeclaration(&vert_dec);
	device->GetVertexShader(&vert_shader);
	device->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
	device->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
	device->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	device->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	device->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
	device->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, NULL);

	d3d9_state_block->Capture(); {
		c_render::exec_start(); {

		}
		c_render::exec_end();
	}

	d3d9_state_block->Apply();
	d3d9_state_block->Release();

	device->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite);
	device->SetRenderState(D3DRS_SRGBWRITEENABLE, srgbwrite);
	device->SetRenderState(D3DRS_COLORWRITEENABLE, dwOld_D3DRS_COLORWRITEENABLE);
	device->SetRenderState(D3DRS_SRGBWRITEENABLE, true);
	device->SetVertexDeclaration(vert_dec);
	device->SetVertexShader(vert_shader);

	return original(device, a, b, c, d);
}

HRESULT __stdcall reset(c_d3d_device* device, D3DPRESENT_PARAMETERS* params) {
	static auto original = __hooks->get_original(&reset);

	auto hr = original(device, params);

	if (SUCCEEDED(hr))
		c_render::exec_reset();

	return hr;
}

void __fastcall paint_traverse(c_panel* ecx, void* edx, unsigned int panel, bool repaint, bool force) {
	static auto original = __hooks->get_original(&paint_traverse);
	original(ecx, edx, panel, repaint, force);
}

void __fastcall override_view(void* ecx, void* edx, c_view_setup* setup) {
	static auto original = __hooks->get_original(&override_view);
	original(ecx, edx, setup);
}

bool __fastcall do_post_screen_effects(void* ecx, void* edx, c_view_setup* setup) {
	static auto original = __hooks->get_original(&do_post_screen_effects);
	return original(ecx, edx, setup);
}

bool __fastcall create_move(void* ecx, void* edx, float input_sample_time, c_user_cmd* cmd) {
	static auto original = __hooks->get_original(&create_move);

	if (!cmd || !cmd->command_number || !__local_player || !__game_info->connected)
		return original(ecx, edx, input_sample_time, cmd);

	if (original(ecx, edx, input_sample_time, cmd)) {
		__interfaces->engine->set_view_angles(cmd->viewangles);
		__interfaces->prediction->set_local_view_angles(cmd->viewangles);
	}

	c_feature::exec_pre_predict(cmd); {
		c_feature::exec_predict(cmd);
	}
	c_feature::exec_post_predict(cmd);

	c_feature::exec_tick_local_death();

	return false;
}

bool __fastcall is_hltv(void* ecx, void* edx) {
	static auto original = __hooks->get_original(&is_hltv);
	return original(ecx, edx);
}

bool __fastcall is_connected(void* ecx, void* edx) {
	static auto original = __hooks->get_original(&is_connected);
	return original(ecx, edx);
}

int __fastcall list_leaves_in_box(void* ecx, void* edx, const vec3_t& mins, const vec3_t& maxs, unsigned short* list, int list_max) {
	static auto original = __hooks->get_original(&list_leaves_in_box);
	return original(ecx, edx, mins, maxs, list, list_max);
}

void __fastcall packet_start(c_client_state* ecx, void* edx, int incoming, int outgoing) {
	static auto original = __hooks->get_original(&packet_start);

	if (!__local_player || !__local_player->is_alive())
		return original(ecx, edx, incoming, outgoing);

	c_feature::exec_on_packet_start(ecx, edx, incoming, outgoing);
}

void __fastcall packet_end(c_client_state* ecx, void* edx) {
	static auto original = __hooks->get_original(&packet_end);

	if (__local_player && __local_player->is_alive()) {
		if (__interfaces->client_state->clock_drift_mgr.server_tick == __interfaces->client_state->delta_tick)
			c_feature::exec_on_packet_end(ecx->last_command_ack);
	}

	original(ecx, edx);
}

void __fastcall run_command(void* ecx, void* edx, c_cs_player* player, c_user_cmd* cmd, c_move_helper* move_helper) {
	static auto original = __hooks->get_original(&run_command);

	if (player != __local_player || !player->is_alive() || !cmd)
		return original(ecx, edx, player, cmd, move_helper);

	original(ecx, edx, player, cmd, move_helper);

	c_feature::exec_run_command(cmd);
}

void __fastcall process_movement(void* ecx, void* edx, c_cs_player* player, c_move_data* data) {
	static auto original = __hooks->get_original(&process_movement);

	// fix prediction error in air (by stop calculating some vars in movement)
	data->game_code_moved_player = false;

	original(ecx, edx, player, data);
}

void __fastcall on_screen_size_changed(void* ecx, void* edx, int old_w, int old_h) {
	static auto original = __hooks->get_original(&on_screen_size_changed);
	original(ecx, edx, old_w, old_h);

	c_render::exec_screen_change();
}

void __fastcall emit_sound(void* thisptr, void* edx, void* filter, int ent_index, int channel, const char* sound_entry, unsigned int sound_entry_hash,
	const char* sample, float volume, float attenuation, int seed, bits_t flags, int pitch, const vec3_t* origin, const vec3_t* direction,
	void* vec_origins, bool update_positions, float sound_time, int speaker_entity, int test) {

	static auto original = __hooks->get_original(&emit_sound);

	auto call_original = [&]() {
		original(thisptr, edx, filter, ent_index, channel, sound_entry, sound_entry_hash, sample, volume, attenuation, seed, flags,
			pitch, origin, direction, vec_origins, update_positions, sound_time, speaker_entity, test);
	};

	// don't replay same sound for weapon
	// TO-DO: figure out why events still plays even when you fixed data

	if (ent_index == __interfaces->engine->get_local_player()) {
		if (std::strstr(sound_entry, XOR("Draw"))
			|| std::strstr(sound_entry, XOR("Deploy"))
			|| std::strstr(sound_entry, XOR("Weapon"))) {

			auto& last_sound_name = __engine_prediction->get_last_sound_name();
			if (!last_sound_name.empty() && std::strstr(sound_entry, last_sound_name.c_str())) {
				flags.force(1 << 2);
				return;
			}

			call_original();

			last_sound_name = sound_entry;
			return;
		}
	}

	// anti client lagger
	if (std::strstr(sound_entry, CXOR("null")))
		flags.force((1 << 2) | (1 << 5));

	// don't play double jump sounds
	if (__engine_prediction->is_in_prediction()) {
		flags.force(1 << 2);
		return;
	}

	call_original();
}

#ifndef LEGACY
void* __fastcall alloc_key_values_memory(c_key_values_system* ecx, int edx, int size) {
	static auto original = __hooks->get_original(&alloc_key_values_memory);

	auto return_addr = reinterpret_cast<std::uintptr_t>(_ReturnAddress());
	if (return_addr == FIND_OFFSET(__modules->client, OFFSET_ALLOC_KEY_VALUES_CLIENT).pointer
		|| return_addr == FIND_OFFSET(__modules->engine, OFFSET_ALLOC_KEY_VALUES_ENGINE).pointer)
		return nullptr;

	return original(ecx, edx, size);
}
#endif

void c_hooks::init_vmt_hooks() {
	ADD_VMT(__interfaces->client, CL_CREATE_MOVE_IDX, cl_create_move_proxy);
	ADD_VMT(__interfaces->client, LEVEL_INIT_PRE_ENTITY_IDX, level_init_pre_entity);
	ADD_VMT(__interfaces->client, LEVEL_INIT_POST_ENTITY_IDX, level_init_post_entity);
	ADD_VMT(__interfaces->client, LEVEL_INIT_SHUTDOWN_IDX, level_shutdown);
	ADD_VMT(__interfaces->client, WRITE_USER_CMD_DELTA_TO_BUFFER_IDX, write_usercmd_to_delta_buffer);

	ADD_VMT(__interfaces->client, FSN_IDX, frame_stage_notify);

	ADD_VMT(__interfaces->d3d_device, PRESENT_IDX, present);
	ADD_VMT(__interfaces->d3d_device, RESET_IDX, reset);

	ADD_VMT(__interfaces->panel, PAINT_TRAVERSE_IDX, paint_traverse);

	ADD_VMT(__interfaces->client_mode, OVERRIDE_VIEW_IDX, override_view);
	ADD_VMT(__interfaces->client_mode, CREATE_MOVE_IDX, create_move);
	ADD_VMT(__interfaces->client_mode, DO_POST_SCREEN_EFFECTS_IDX, do_post_screen_effects);

	ADD_VMT(__interfaces->engine, IS_HLTV_IDX, is_hltv);
	ADD_VMT(__interfaces->engine, IS_CONNECTED_IDX, is_connected);

	ADD_VMT(__interfaces->prediction, RUN_COMMAND_IDX, run_command);
	ADD_VMT(__interfaces->game_movement, PROCESS_MOVEMENT_IDX, process_movement);
	ADD_VMT(__interfaces->surface, ON_SCREEN_SIZE_CHANGED, on_screen_size_changed);

#ifndef LEGACY
	ADD_VMT(__interfaces->key_values_system, ALLOC_KEY_VALUES_IDX, alloc_key_values_memory);
#endif

	auto bsp_query = __interfaces->engine->get_bsp_tree_query();
	auto client_state_vmt = reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(__interfaces->client_state) + XORN(0x8));

	ADD_VMT(bsp_query, LIST_LEAVES_IN_BOX_IDX, list_leaves_in_box);

	ADD_VMT(client_state_vmt, PACKET_START_IDX, packet_start);
	ADD_VMT(client_state_vmt, PACKET_END_IDX, packet_end);

	ADD_VMT(__interfaces->engine_sound, EMIT_SOUND, emit_sound);
}