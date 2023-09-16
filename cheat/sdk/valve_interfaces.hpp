#pragma once
#include "color.hpp"

class c_key_values_system;
using create_interface_fn = void* (*)(const char* name, int* code);

class c_app_system {
public:
    virtual bool connect(create_interface_fn factory) = 0;
    virtual void disconnect() = 0;
    virtual void* query_interface(const char* p_interface_name) = 0;
    virtual int init() = 0;
    virtual void shutdown() = 0;
    virtual const void* dependencies() = 0;
    virtual int tier() = 0;
    virtual void reconnect(create_interface_fn factory, const char* p_interface_name) = 0;
    virtual void unk_func() = 0;
};

class c_client {
public:
    VFUNC(get_client_classes(), c_client_class* (__thiscall*)(decltype(this)), 8);
};

class c_client_mode {
public:
};

class c_client_state {
public:
    class c_clock_drift_manager {
    public:
        float clock_offsets[0x10];
        int cur_clock_offset;
        int server_tick;
        int client_tick;
    };

    PAD(0x9C);
    c_net_channel* net_channel;
    int challenge_nr;
    PAD(0x4);
    double connect_time;
    int retry_number;
    PAD(0x54);
    int signon_state;
    PAD(0x4);
    double next_cmd_time;
    int server_count;
    int current_sequence;
    PAD(0x8);
    c_clock_drift_manager clock_drift_mgr;
    int delta_tick;
#ifdef LEGACY
    PAD(19104);
    int max_clients;
    PAD(140);
#else
    PAD(0x4);
    int view_entry;
    int player_slot;
    bool paused;
    PAD(0x3);
    char level_name[260];
    char level_name_short[40];
    PAD(0xD4);
    int max_clients;
    PAD(0x4994);
    int old_tickcount;
    float tick_remainder;
#endif
    float frame_time;
    int last_outgoing_command;
    int choked_commands;
    int last_command_ack;
#ifdef LEGACY
    PAD(308);
#else
    int last_server_tick;
    int command_ack;
    int sound_sequence;
    PAD(76);
    vec3_t viewangles;
    PAD(196);
#endif
    c_event_info* events;
};

class c_engine {
public:
    VFUNC(get_screen_size(int& w, int& h), void(__thiscall*)(decltype(this), int&, int&), 5, w, h);
    VFUNC(get_player_info(int index, player_info_t* pinfo), bool(__thiscall*)(decltype(this), int, player_info_t*), 8, index, pinfo);
    VFUNC(is_console_open(), bool(__thiscall*)(decltype(this)), 11);
    VFUNC(get_local_player(), int(__thiscall*)(decltype(this)), 12);
    VFUNC(get_last_time_stamp(), float(__thiscall*)(decltype(this)), 14);
    VFUNC(get_view_angles(vec3_t& angle), void(__thiscall*)(decltype(this), vec3_t&), 18, angle);
    VFUNC(set_view_angles(vec3_t& angle), void(__thiscall*)(decltype(this), vec3_t&), 19, angle);
    VFUNC(get_max_clients(), int(__thiscall*)(decltype(this)), 20);
    VFUNC(is_in_game(), bool(__thiscall*)(decltype(this)), 26);
    VFUNC(is_connected(), bool(__thiscall*)(decltype(this)), 27);
    VFUNC(get_net_channel(), i_net_channel_info* (__thiscall*)(decltype(this)), 78);
    VFUNC(get_bsp_tree_query(), void* (__thiscall*)(decltype(this)), 43);
    VFUNC(is_hltv(), bool(__thiscall*)(decltype(this)), 93);
    VFUNC(execute_client_cmd(const char* cmd, const char* flag = 0), void(__thiscall*)(decltype(this), const char*, const char*), 114, cmd, flag);
    VFUNC(get_last_command_acknowledged(), int(__thiscall*)(decltype(this)), 205);
    VFUNC(fire_events(), void(__thiscall*)(decltype(this)), 59);
};

class c_entity_listener {
public:
    virtual void on_entity_created(c_base_entity* entity) {};
    virtual void on_entity_deleted(c_base_entity* entity) {};
};

class c_entity_list {
private:
    c_utl_vector<c_entity_listener*> listeners = {};
public:
    virtual void fn0() = 0;
    virtual void fn1() = 0;
    virtual void fn2() = 0;

    virtual void* get_client_entity(int idx) = 0;
    virtual void* get_client_entity_handle(std::uint32_t handle) = 0;
    virtual int number_of_entities(bool include_non_networkable) = 0;
    virtual int get_highest_entity_index() = 0;

    INLINE void add_listener(c_entity_listener* listener) {
        listeners.add_to_tail(listener);
    }

    INLINE void remove_listener(c_entity_listener* listener) {
        listeners.find_and_remove(listener);
    }
};

class c_panel {
public:
    VFUNC(set_mouse_input_enabled(unsigned int panel, bool state), void(__thiscall*)(decltype(this), unsigned int, bool), 32, panel, state);
    VFUNC(get_name(int index), const char* (__thiscall*)(decltype(this), int), 36, index);
};

class c_model_render {
public:
    VFUNC(forced_material_override(i_material* mat, int type = 0, int overrides = 0),
        void(__thiscall*)(decltype(this), i_material*, int, int), 1, mat, type, overrides);

    VFUNC(draw_model_execute(void* ctx, const draw_model_state_t& state, const model_render_info_t& info, matrix3x4_t* bone_to_world = NULL),
        void(__thiscall*)(decltype(this), void*, const draw_model_state_t&, const model_render_info_t&, matrix3x4_t*),
        21, ctx, state, info, bone_to_world);
};

class c_debug_overlay {
public:
    virtual void add_entity_text_overlay(int ent_index, int line_offset, float duration, int r, int g, int b, int a, const char* format, ...) = 0;
    virtual void add_box_overlay(const vec3_t& origin, const vec3_t& mins, const vec3_t& max, vec3_t const& orientation, int r, int g, int b, int a, float duration) = 0;
    virtual void add_sphere_overlay(const vec3_t& origin, float radius, int theta, int phi, int r, int g, int b, int a, float duration) = 0;
    virtual void add_triangle_overlay(const vec3_t& p1, const vec3_t& p2, const vec3_t& p3, int r, int g, int b, int a, bool no_depth_test, float duration) = 0;
    virtual void add_line_overlay(const vec3_t& origin, const vec3_t& dest, int r, int g, int b, bool no_depth_test, float duration) = 0;
    virtual void add_text_overlay(const vec3_t& origin, float duration, const char* format, ...) = 0;
    virtual void add_text_overlay(const vec3_t& origin, int line_offset, float duration, const char* format, ...) = 0;
    virtual void add_screen_text_overlay(float x, float y, float duration, int r, int g, int b, int a, const char* text) = 0;
    virtual void add_swept_box_overlay(const vec3_t& start, const vec3_t& end, const vec3_t& mins, const vec3_t& max, const vec3_t& angles, int r, int g, int b, int a, float duration) = 0;
    virtual void add_grid_overlay(const vec3_t& origin) = 0;
    virtual void add_coord_flame_overlay(const matrix3x4_t& frame, float scale, int color_table[3][3] = 0) = 0;
    virtual int screen_position(const vec3_t& point, vec3_t& screen) = 0;
    virtual int screen_position(float x, float y, vec3_t& screen) = 0;
    virtual void* get_first(void) = 0;
    virtual void* get_next(void* current) = 0;
    virtual void clear_dead_overlays(void) = 0;
    virtual void clear_all_overlays(void) = 0;
    virtual void add_text_overlay_rgb(const vec3_t& origin, int line_offset, float duration, float r, float g, float b, float alpha, const char* format, ...) = 0;
    virtual void add_text_overlay_rgb(const vec3_t& origin, int line_offset, float duration, int r, int g, int b, int a, const char* format, ...) = 0;
    virtual void add_line_overlay_alpha(const vec3_t& origin, const vec3_t& dest, int r, int g, int b, int a, bool no_depth_test, float duration) = 0;
    virtual void add_box_overlay_alt(const vec3_t& origin, const vec3_t& mins, const vec3_t& max, vec3_t const& orientation, const c_color face_color, const c_color edge_color, float duration) = 0;
    virtual void add_line_overlay(const vec3_t& origin, const vec3_t& dest, int r, int g, int b, int a, float, float) = 0;
    virtual void purge_text_overlays() = 0;
    virtual void add_capsule_overlay(const vec3_t& mins, const vec3_t& max, float& radius, int r, int g, int b, int a, float duration, char unknown, char ignorez) = 0;
};

class c_material_system {
public:
    VFUNC(create_material(const char* name, c_key_values* key),
        i_material* (__thiscall*)(decltype(this), const char*, c_key_values*), 83, name, key);

    VFUNC(find_material(const char* material_name, const char* group_name, bool complain = true, const char* complain_prefix = NULL),
        i_material* (__thiscall*)(decltype(this), const char*, const char*, bool, const char*), 84,
        material_name, group_name, complain, complain_prefix);

    VFUNC(first_material(), unsigned short(__thiscall*)(decltype(this)), 86);
    VFUNC(next_material(unsigned short h), unsigned short(__thiscall*)(decltype(this), unsigned short h), 87, h);
    VFUNC(invalid_material(), unsigned short(__thiscall*)(decltype(this)), 88);
    VFUNC(get_material(unsigned short h), i_material* (__thiscall*)(decltype(this), unsigned short), 89, h);

    VFUNC(get_render_context(), void* (__thiscall*)(decltype(this)), 115);
};

class c_model_info {
public:
    VFUNC(get_model(int index), void* (__thiscall*)(void*, int), 1, index);
    VFUNC(get_model_index(const char* name), int(__thiscall*)(void*, const char*), 2, name);
    VFUNC(get_model_name(const char* mod), char* (__thiscall*)(void*, const char*), 3, mod);
    VFUNC(get_studio_model(const model_t* mod), studiohdr_t* (__thiscall*)(void*, const model_t* mod), GET_MODEL_VFUNC, mod);
};

class c_game_movement {
public:
    virtual ~c_game_movement(void) {}

    virtual void process_movement(c_cs_player* player, c_move_data* move_data) = 0;
    virtual void reset(void) = 0;
    virtual void start_track_prediction_errors(c_cs_player* player) = 0;
    virtual void finish_track_prediction_errors(c_cs_player* player) = 0;
    virtual void diff_print(char const* fmt, ...) = 0;

    virtual const vec3_t& get_player_mins(bool ducked) const = 0;
    virtual const vec3_t& get_player_maxs(bool ducked) const = 0;
    virtual const vec3_t& get_player_view_offset(bool ducked) const = 0;
};

class c_prediction {
public:
    std::uint32_t ground_handle;
    bool in_prediction;
    bool old_in_prediction;
    PAD(2);
    std::int32_t prev_start_frame;
    std::int32_t incoming_packet_number;
    float time_stamp;
    bool is_first_time_predicted;
    PAD(3);
    std::int32_t commands_predicted;
    std::int32_t server_commands_acknowledged;
    std::int32_t prev_ack_had_errors;
    float ideal_pitch;
    std::uint32_t last_cmd_acknowledged;
    bool trigger_latch_reset;

    virtual ~c_prediction(void) = 0;

    virtual void init(void) = 0;
    virtual void shutdown(void) = 0;

    virtual void update(int startframe, bool validframe, int incoming_acknowledged, int outgoing_command) = 0;

    virtual void pre_entity_packet_received(int commands_acknowledged, int current_world_update_packet) = 0;
    virtual void post_entity_packet_received(void) = 0;
    virtual void post_network_data_received(int commands_acknowledged) = 0;

    virtual void on_received_compressed_packet(void) = 0;

    virtual void get_view_origin(vec3_t& org) = 0;
    virtual void set_view_origin(vec3_t& org) = 0;
    virtual void get_view_angles(vec3_t& ang) = 0;
    virtual void set_view_angles(vec3_t& ang) = 0;

    virtual void get_local_view_angles(vec3_t& ang) = 0;
    virtual void set_local_view_angles(vec3_t& ang) = 0;

    virtual bool in_prediciton(void) const = 0;
    virtual bool is_first_time_predicited(void) const = 0;

    virtual int get_last_acknowledged_command_number(void) const = 0;

    virtual int get_incoming_packet_number(void) const = 0;

    virtual void check_moving_ground(c_base_entity* player, double frametime) = 0;
    virtual void run_command(c_base_entity* player, c_user_cmd* cmd, void* moveHelper) = 0;

    virtual void setup_move(c_base_entity* player, c_user_cmd* cmd, void* pHelper, void* move) = 0;
    virtual void finish_move(c_base_entity* player, c_user_cmd* cmd, void* move) = 0;
    virtual void set_ideal_pitch(int slot, c_base_entity* player, const vec3_t& origin, const vec3_t& angles, const vec3_t& viewheight) = 0;

    virtual void check_error(int slot, c_base_entity* player, int commands_acknowledged) = 0;
};

class c_move_helper {
private:
    virtual void pad0() = 0;
public:
    virtual void set_host(c_base_entity* host) = 0;
private:
    virtual void pad1() = 0;
    virtual void pad2() = 0;
public:
    virtual void process_impacts() = 0;
};

using cvar_dll_identifier_fn = int;
class c_con_command;
class c_con_command_base {
public:
    VFUNC(get_name(), const char* (__thiscall*)(decltype(this)), 6);

public:
    void* thisptr;
    c_con_command_base* next;
    bool registered;
    const char* name;
    const char* help_string;
    int flags;
    c_con_command_base* con_command_base;
    void* accessor;
};

class i_cvar_iterator_internal {
public:
    virtual void set_first() = 0;
    virtual void next() = 0;
    virtual	bool is_valid() = 0;
    virtual c_con_command_base* get() = 0;
};

class c_engine_cvar : c_app_system {
public:
    template <typename... args>
    VFUNC(print_console_color(const c_color& clr, const char* format, args... arg),
        void(*)(decltype(this), const c_color&, const char*, ...), 25, clr, format, arg...);

    VFUNC(find_convar(const char* name), c_convar* (__thiscall*)(decltype(this), const char*), 16, name);
    VFUNC(factory_internal_iterator(), i_cvar_iterator_internal* (__thiscall*)(decltype(this)), FACTORY_INTERNAL_ITERATOR_VFUNC);
};

class c_input {
public:
#ifdef LEGACY
    void* vtable;
    bool trackir;
    bool mouse_init;
    bool mouse_active;
    bool joystick_adv_init;

    PAD(0x2C);

    void* keys;
    PAD(0x6C);

    bool camera_intercepting_mouse;
#else
    PAD(0xC);

    bool trackir_available;
    bool mouse_initialized;
    bool mouse_active;

    PAD(0x9A);
#endif
    bool camera_in_third_person;

    PAD(0x2);

    vec3_t camera_offset;

#ifdef LEGACY
    bool camera_distance_move;
    int camera_old_x;
    int camera_old_y;
    int camera_x;
    int camera_y;
    bool camera_is_orthographic;
    vec3_t previous_view_angles;
    vec3_t previous_view_angles_tilt;
    float last_forward_move;
    int clear_input_state;
#else
    PAD(0x38);
#endif
    c_user_cmd* commands;
    verified_cmd_t* verified_commands;

    INLINE c_user_cmd* get_user_cmd(int seq) {
        return &commands[seq % 150];
    }

    INLINE verified_cmd_t* get_verified_user_cmd(int sequence_number) {
        return &verified_commands[sequence_number % 150];
    }

    VFUNC(get_user_cmd(int slot, int seq), c_user_cmd* (__thiscall*)(decltype(this), int, int), 8, slot, seq);
};

class c_game_event_listener2 {
public:
    virtual ~c_game_event_listener2() {}
    virtual void fire_game_event(c_game_event* event) = 0;
    virtual int get_event_debug_id() = 0;
};

class c_game_event_manager2 {
public:
    virtual ~c_game_event_manager2();
    virtual int load_events_from_file(const char* file_name) = 0;
    virtual void reset() = 0;
    virtual bool add_listener(c_game_event_listener2* listener, const char* name, bool server_side) = 0;
    virtual bool find_listener(c_game_event_listener2* listener, const char* name) = 0;
    virtual void remove_listener(c_game_event_listener2* listener) = 0;
    virtual bool add_listener_global(c_game_event_listener2* listener, bool server_side) = 0;
    virtual c_game_event* create_event(const char* name, bool force = false, int* cookie = NULL) = 0;
    virtual bool fire_event(c_game_event* event, bool dont_broadcast = false) = 0;
    virtual bool fire_event_client_side(c_game_event* event) = 0;
};

class c_phys_surface_props {
public:
    VFUNC(get_surface_data(int surface_index), surface_data_t* (__thiscall*)(decltype(this), int), 5, surface_index);
};

class c_render_view {
public:
    VFUNC(set_blend(float value), void(__thiscall*)(decltype(this), float), 4, value);
    VFUNC(get_blend(), float(__thiscall*)(decltype(this)), 5);
    VFUNC(set_color_modulation(float const* blend), void(__thiscall*)(decltype(this), float const*), 6, blend);
    VFUNC(get_color_modulation(float* blend), void(__thiscall*)(decltype(this), float*), 7, blend);
};

class c_view_render {
public:
    PAD(4);
    c_view_setup view;
};

class c_glow_object_manager {
public:
    class c_glow_object_definition {
    public:
        c_glow_object_definition() {
            std::memset(this, 0, sizeof(*this));
        }

        std::int32_t next_slot;
        c_base_entity* entity;

        union {
            vec3_t color;
            struct {
                float r;
                float g;
                float b;
            };
        };

        float alpha;
        bool custoalpha;
        float alpha_velocity;
        float alpha_max;
        float glow_pulse;
        bool occlued_render;
        bool unocclued_render;
        bool bloom;
        int full_bloom_stencil_test_value;
        std::int32_t glow_style;
        std::int32_t screen_slot;

        INLINE bool is_unused() const {
            return next_slot != ENTRY_IN_USE;
        }

        static constexpr int END_OF_FREE_LIST = -1;
        static constexpr int ENTRY_IN_USE = -2;
    };

    c_utl_vector< c_glow_object_definition > glow_objects;
    int first_slot;

    struct glow_box_definition_t {
        vec3_t pos;
        vec3_t ang;
        vec3_t mins;
        vec3_t maxs;
        float birth_time;
        float end_time;
        c_color clr;
    };

    c_utl_vector< glow_box_definition_t > glow_boxes;
};

class c_global_vars {
public:
    float realtime;
    int framecount;
    float absoluteframetime;
    float absoluteframestarttimestddev;
    float curtime;
    float frametime;
    int max_clients;
    int tickcount;
    float interval_per_tick;
    float interpolation_amount;
    int sim_ticks_this_frame;
    int network_protocol;
    void* save_data;
    bool client;
    int time_stamp_networking_base;
    int time_stamp_randomize_window;

    INLINE int get_network_base(int tick, int entity)
    {
        int entity_mod = entity % time_stamp_randomize_window;
        int base_tick = time_stamp_networking_base * (int)((tick - entity_mod) / time_stamp_networking_base);
        return base_tick;
    }
};

class c_weapon_system {
protected:
    ~c_weapon_system() = default;

private:
    virtual void pad0() = 0;
    virtual void pad1() = 0;

public:
    virtual weapon_info_t* get_weapon_data(uint32_t item_definition_index) = 0;
};

class c_engine_trace {
public:
    virtual int get_point_contents(const vec3_t& position, int contents_mask = MASK_ALL, c_base_entity** p_entity = nullptr) = 0;
    virtual int get_point_contents_world_only(const vec3_t& position, int contents_mask = MASK_ALL) = 0;
    virtual int get_point_contents_collideable(c_collideable* collide, const vec3_t& position) = 0;
    virtual void clip_ray_to_entity(const ray_t& ray, unsigned int mask, c_base_entity* e, c_game_trace* trace) = 0;
    virtual void clip_ray_to_collideable(const ray_t& ray, unsigned int mask, c_collideable* collide, c_game_trace* trace) = 0;
    virtual void trace_ray(const ray_t& ray, unsigned int mask, i_trace_filter* trace_filter, c_game_trace* trace) = 0;
};

using key_values_system_fn = c_key_values_system * (__cdecl*)();

class c_key_values_system {
public:
    virtual void register_sizeof_key_values(int size) = 0;
    virtual void* alloc_key_values_memory(int size) = 0;
    virtual void free_key_values_memory(void* memory) = 0;
    virtual int get_symbol_for_string(const char* name, bool bCreate = true) = 0;
    virtual const char* get_string_for_symbol(int symbol) = 0;
    virtual void add_key_values_to_memory_leak_list(void* memory, int name) = 0;
    virtual void remove_key_values_to_memory_leak_list(void* pMem) = 0;
};

class c_game_rules {
public:
    INLINE bool is_freeze_time() {
        return *reinterpret_cast<bool*>(reinterpret_cast<std::uintptr_t>(this) + XORN(0x20));
    }

    INLINE bool is_valve_ds() {
        return *reinterpret_cast<bool*>(reinterpret_cast<std::uintptr_t>(this) + XORN(0x7C));
    }

    VFUNC(get_view_vectors(), view_vectors_t* (__thiscall*)(void*), 30);
};

class c_surface : public c_app_system {
public:
    virtual void run_frame() = 0;
    virtual unsigned int get_embedded_panel() = 0;
    virtual void set_embedded_panel(unsigned int pPanel) = 0;
    virtual void push_make_current(unsigned int panel, bool useInsets) = 0;
    virtual void pop_make_current(unsigned int panel) = 0;
    virtual void draw_set_color(int r, int g, int b, int a) = 0;
    virtual void draw_set_color(c_color col) = 0;
    virtual void draw_filled_rect(int x0, int y0, int x1, int y1) = 0;
    virtual void draw_filled_rect_array(void* pRects, int numRects) = 0;
    virtual void draw_outlined_rect(int x0, int y0, int x1, int y1) = 0;
    virtual void draw_line(int x0, int y0, int x1, int y1) = 0;
    virtual void draw_poly_line(int* px, int* py, int numPoints) = 0;
    virtual void draw_sent_apparent_depth(float f) = 0;
    virtual void draw_clear_apparent_depth(void) = 0;
    virtual void draw_set_text_font(unsigned long font) = 0;
    virtual void draw_set_text_color(int r, int g, int b, int a) = 0;
    virtual void draw_set_text_color(c_color col) = 0;
    virtual void draw_set_text_pos(int x, int y) = 0;
    virtual void draw_get_text_pos(int& x, int& y) = 0;
    virtual void draw_print_text(const wchar_t* text, int textLen, int drawType = 0) = 0;
    virtual void draw_unicode_char(wchar_t wch, int drawType = 0) = 0;
    virtual void draw_flush_text() = 0;
    virtual void* create_html_window(void* events, unsigned int context) = 0;
    virtual void paint_html_window(void* htmlwin) = 0;
    virtual void delete_html_window(void* htmlwin) = 0;
    virtual int draw_get_texture_id(char const* filename) = 0;
    virtual bool draw_get_texture_file(int id, char* filename, int maxlen) = 0;
    virtual void draw_set_texture_file(int id, const char* filename, int hardwareFilter, bool forceReload) = 0;
    virtual void draw_set_texture_rgba(int id, const unsigned char* rgba, int wide, int tall) = 0;
    virtual void draw_set_texture(int id) = 0;
    virtual void delete_texture_by_id(int id) = 0;
    virtual void draw_set_texture_size(int id, int& wide, int& tall) = 0;
    virtual void draw_textured_rect(int x0, int y0, int x1, int y1) = 0;
    virtual bool is_texture_id_valid(int id) = 0;
    virtual int create_new_texture_id(bool procedural = false) = 0;
    virtual void get_screen_size(int& wide, int& tall) = 0;
    virtual void set_as_top_most(unsigned int panel, bool state) = 0;
    virtual void bring_to_font(unsigned int panel) = 0;
    virtual void set_foreground_window(unsigned int panel) = 0;
    virtual void set_panel_visible(unsigned int panel, bool state) = 0;
    virtual void set_minimized(unsigned int panel, bool state) = 0;
    virtual bool is_minimized(unsigned int panel) = 0;
    virtual void flash_window(unsigned int panel, bool state) = 0;
    virtual void set_title(unsigned int panel, const wchar_t* title) = 0;
    virtual void set_as_tool_bar(unsigned int panel, bool state) = 0;
    virtual void create_popup(unsigned int panel, bool minimised, bool showTaskbarIcon = true, bool disabled = false, bool mouseInput = true, bool kbInput = true) = 0;
    virtual void swap_buffers(unsigned int panel) = 0;
    virtual void invalidate(unsigned int panel) = 0;
    virtual void set_cursor(unsigned long cursor) = 0;
    virtual bool is_cursor_visible() = 0;
    virtual void apply_changes() = 0;
    virtual bool is_within(int x, int y) = 0;
    virtual bool has_focus() = 0;
    virtual bool supports_feature(int feature) = 0;
    virtual void restrict_panel_to_single(unsigned int panel, bool bForceAllowNonModalSurface = false) = 0;
    virtual void set_modal_panel(unsigned int) = 0;
    virtual unsigned int get_modal_panel() = 0;
    virtual void unlock_cursor() = 0;
    virtual void lock_cursor() = 0;
    virtual void set_translate_extended_keys(bool state) = 0;
    virtual unsigned int get_topmost_popup() = 0;
    virtual void set_top_level_focus(unsigned int panel) = 0;
    virtual unsigned long font_create() = 0;
    virtual bool set_font_glyph_set(unsigned long font, const char* windowsFontName, int tall, int weight, int blur, int scanlines, unsigned int flags, int nRangeMin = 0, int nRangeMax = 0) = 0;
    virtual bool add_custom_font_file(const char* fontFileName) = 0;
    virtual int get_font_tall(unsigned long font) = 0;
    virtual int get_font_ascent(unsigned long font, wchar_t wch) = 0;
    virtual bool is_font_additive(unsigned long font) = 0;
    virtual void get_char_wide(unsigned long font, int ch, int& a, int& b, int& c) = 0;
    virtual int get_character_width(unsigned long font, int ch) = 0;
    virtual void get_text_size(unsigned long font, const wchar_t* text, int& wide, int& tall) = 0;
    virtual unsigned int get_notify_panel() = 0;
    virtual void set_notify_icon(unsigned int context, unsigned long icon, unsigned int panelToReceiveMessages, const char* text) = 0;
    virtual void play_sound(const char* fileName) = 0;
    virtual int get_popup_count() = 0;
    virtual unsigned int get_popup(int index) = 0;
    virtual bool should_paint_child_panel(unsigned int childPanel) = 0;
    virtual bool recreate_context(unsigned int panel) = 0;
    virtual void add_panel(unsigned int panel) = 0;
    virtual void release_panel(unsigned int panel) = 0;
    virtual void move_popup_to_front(unsigned int panel) = 0;
    virtual void move_popup_to_back(unsigned int panel) = 0;
    virtual void solve_traverse(unsigned int panel, bool forceApplySchemeSettings = false) = 0;
    virtual void paint_traverse(unsigned int panel) = 0;
    virtual void enable_mouse_capture(unsigned int panel, bool state) = 0;
    virtual void get_workspace_bounds(int& x, int& y, int& wide, int& tall) = 0;
    virtual void get_abs_window_bounds(int& x, int& y, int& wide, int& tall) = 0;
    virtual void get_proportional_base(int& width, int& height) = 0;
    virtual void calc_mouse_vis() = 0;
    virtual bool net_kbi_input() = 0;
    virtual bool has_cursor_pos_funcs() = 0;
    virtual void surface_get_cursor_pos(int& x, int& y) = 0;
    virtual void surface_set_cursor_pos(int x, int y) = 0;
    virtual void draw_textured_line(const vertex_t& a, const vertex_t& b) = 0;
    virtual void draw_outlined_circle(int x, int y, int radius, int segments) = 0;
    virtual void add_text_poly_line(const vertex_t* p, int n) = 0;
    virtual void add_text_sub_rect(int x0, int y0, int x1, int y1, float texs0, float text0, float texs1, float text1) = 0;
    virtual void add_text_poly(int n, const vertex_t* pVertice, bool bClipVertices = true) = 0;
};

class c_engine_sound {
public:

};