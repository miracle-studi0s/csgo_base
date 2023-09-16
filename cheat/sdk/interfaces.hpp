#pragma once

#include "../callbacks.hpp"
#include "valve.hpp"

class c_cs_player_resource;

using c_d3d_device = IDirect3DDevice9;

class c_interfaces : public c_sdk {
public:
    c_client* client{};
    c_client_mode* client_mode{};
    c_client_state* client_state{};
    c_engine* engine{};
    c_entity_list* entity_list{};
    c_panel* panel{};
    c_model_render* model_render{};
    c_debug_overlay* debug_overlay{};
    c_material_system* material_system{};
    c_game_movement* game_movement{};
    c_prediction* prediction{};
    c_move_helper* move_helper{};
    c_engine_cvar* cvar{};
    c_model_info* model_info{};
    c_input* input{};
    c_game_event_manager2* game_event_manager{};
    c_phys_surface_props* phys_surface_props{};
    c_render_view* render_view{};
    c_view_render* view_render{};
    c_glow_object_manager* glow_object_manager{};
    c_global_vars* global_vars{};
    c_weapon_system* weapon_system{};
    c_engine_trace* engine_trace{};
    c_d3d_device* d3d_device{};
    c_key_values_system* key_values_system{};
    void* model_cache{};
    c_game_rules* game_rules{};
    c_cs_player_resource* player_resource{};
    c_surface* surface{};
    c_engine_sound* engine_sound{};

	virtual void store() override;
	virtual void map_init_post() override;
};

#ifdef _DEBUG
inline auto __interfaces = std::make_unique<c_interfaces>();
#else
CREATE_DUMMY_PTR(c_interfaces);
DECLARE_XORED_PTR(c_interfaces, GET_XOR_KEYUI32);

#define __interfaces XORED_PTR(c_interfaces)
#endif