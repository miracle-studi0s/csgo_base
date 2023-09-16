#include "../main_definitions.hpp"
#include "../security/security.hpp"
#include "memory.hpp"
#include "modules.hpp"
#include "interfaces.hpp"
#include "offsets.hpp"

#define GET_INTERFACE(handle, name, ptr) __memory->find_interface(handle, XOR(name)).cast<ptr*>();

void c_interfaces::store() {
	DEBUG_LOG("  [+] Store interfaces \n\n");

	client				= GET_INTERFACE(__modules->client, "VClient018", c_client);
	engine				= GET_INTERFACE(__modules->engine, "VEngineClient014", c_engine);
	panel				= GET_INTERFACE(__modules->vgui2, "VGUI_Panel009", c_panel);
	cvar				= GET_INTERFACE(__modules->vstdlib, "VEngineCvar007", c_engine_cvar);
	model_info			= GET_INTERFACE(__modules->engine, "VModelInfoClient004", c_model_info);
	prediction			= GET_INTERFACE(__modules->client, "VClientPrediction001", c_prediction);
	entity_list			= GET_INTERFACE(__modules->client, "VClientEntityList003", c_entity_list);
	render_view			= GET_INTERFACE(__modules->engine, "VEngineRenderView014", c_render_view);
	model_render		= GET_INTERFACE(__modules->engine, "VEngineModel016", c_model_render);
	debug_overlay		= GET_INTERFACE(__modules->engine, "VDebugOverlay004", c_debug_overlay);
	game_movement		= GET_INTERFACE(__modules->client, "GameMovement001", c_game_movement);
	material_system		= GET_INTERFACE(__modules->materialsystem, "VMaterialSystem080", c_material_system);
	game_event_manager	= GET_INTERFACE(__modules->engine, "GAMEEVENTSMANAGER002", c_game_event_manager2);
	phys_surface_props	= GET_INTERFACE(__modules->vphysics, "VPhysicsSurfaceProps001", c_phys_surface_props);
	engine_trace		= GET_INTERFACE(__modules->engine, "EngineTraceClient004", c_engine_trace);
	model_cache			= GET_INTERFACE(__modules->datacache, "MDLCache004", void);
	surface				= GET_INTERFACE(__modules->vguimatsurface, "VGUI_Surface031", c_surface);
	engine_sound		= GET_INTERFACE(__modules->engine, "IEngineSoundClient003", c_engine_sound);

	client_mode			= **__memory->get_virtual(client, XORN(10)).add(XORN(5)).cast<c_client_mode***>();
	input				= *FIND_OFFSET(__modules->client, OFFSET_INPUT_PTR).cast<c_input**>();
	client_state		= **FIND_OFFSET(__modules->engine, OFFSET_CLIENT_STATE).cast<c_client_state***>();
	move_helper			= **FIND_OFFSET(__modules->client, OFFSET_MOVE_HELPER).cast<c_move_helper***>();
	global_vars			= **FIND_OFFSET(__modules->client, OFFSET_GLOBAL_VARS).cast<c_global_vars***>();
	view_render			= **FIND_OFFSET(__modules->client, OFFSET_VIEW_RENDER).cast<c_view_render***>();
	glow_object_manager = *FIND_OFFSET(__modules->client, OFFSET_GLOW_OBJECT_MANAGER).cast<c_glow_object_manager**>();
	d3d_device			= **FIND_OFFSET(__modules->shaderapidx9, OFFSET_D3D_DEVICE).cast<c_d3d_device***>();
	weapon_system		= *FIND_OFFSET(__modules->client, OFFSET_WEAPON_SYSTEM).cast<c_weapon_system**>();
	key_values_system	= address_t{ GetProcAddress(reinterpret_cast<HMODULE>(__modules->vstdlib), CXOR("KeyValuesSystem")) }.cast<key_values_system_fn>()();

	game_rules			= **FIND_OFFSET(__modules->client, OFFSET_GAME_RULES).cast<c_game_rules***>();
}

void c_interfaces::map_init_post() {
	game_rules = **FIND_OFFSET(__modules->client, OFFSET_GAME_RULES).cast<c_game_rules***>();
}