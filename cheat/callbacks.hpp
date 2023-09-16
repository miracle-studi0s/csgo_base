#pragma once
#include <vector>
#include <Windows.h>

class c_client_state;
class c_cs_player;
class c_user_cmd;
struct animation_record_t;
struct animation_info_t;

#define INIT_CALLBACK(class_name)\
inline static std::vector<class_name*> _callbacks{};\
public:\
    class_name() { _callbacks.push_back(this); }\

#define DEFINE_CALLBACK( func_name, ... ) \
  private: \
    virtual void func_name( __VA_ARGS__ ) { } \
  public: \
    template< class... Ts >                     \
    static __forceinline void exec_##func_name(Ts... args)     \
    {                               \
      for (auto& _call : _callbacks)               \
      {                             \
        _call->func_name(args...);             \
      }                          \
    }

// callbacks for cheat main

class c_dllmain {
    INIT_CALLBACK(c_dllmain);
public:
    DEFINE_CALLBACK(start, LPVOID);
    DEFINE_CALLBACK(destroy);
};

class c_sdk {
    INIT_CALLBACK(c_sdk);
public:
    DEFINE_CALLBACK(destroy);
    DEFINE_CALLBACK(store);
    DEFINE_CALLBACK(map_init_pre);
    DEFINE_CALLBACK(map_init_post);
};

class c_feature {
    INIT_CALLBACK(c_feature);
public:
    DEFINE_CALLBACK(frame, int);
    DEFINE_CALLBACK(frame_post, int);
    DEFINE_CALLBACK(on_packet_start, c_client_state*, void*, int, int);
    DEFINE_CALLBACK(on_packet_end, int);
    DEFINE_CALLBACK(tick, bool*);
    DEFINE_CALLBACK(tick_end, bool*, int);
    DEFINE_CALLBACK(pre_predict, c_user_cmd*);
    DEFINE_CALLBACK(predict, c_user_cmd*);
    DEFINE_CALLBACK(post_predict, c_user_cmd*);
    DEFINE_CALLBACK(run_command, c_user_cmd*);
    DEFINE_CALLBACK(tick_local_death);
    DEFINE_CALLBACK(update_client_side_animation, c_cs_player*);
    DEFINE_CALLBACK(map_init_pre);
    DEFINE_CALLBACK(map_init_post);
    DEFINE_CALLBACK(map_shutdown);
    DEFINE_CALLBACK(paint);
};

class c_animations {
    INIT_CALLBACK(c_animations);
public:
    DEFINE_CALLBACK(collect, c_cs_player*, animation_info_t*, animation_record_t*);
};

class c_render {
    INIT_CALLBACK(c_render);
public:
    DEFINE_CALLBACK(init);
    DEFINE_CALLBACK(start);
    DEFINE_CALLBACK(end);
    DEFINE_CALLBACK(reset);
    DEFINE_CALLBACK(screen_change);
    DEFINE_CALLBACK(frame_post, int);
};