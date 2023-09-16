#pragma once

constexpr int EVENT_DEBUG_ID_INIT = 42;
constexpr int EVENT_DEBUG_ID_SHUTDOWN = 13;

class c_event_listener : public c_game_event_listener2, c_sdk {
private:
	bool registered_events{};
	int debug_id{};

public:
	INLINE c_event_listener() : registered_events(false), debug_id(EVENT_DEBUG_ID_INIT) {}
	INLINE ~c_event_listener() {
		debug_id = EVENT_DEBUG_ID_SHUTDOWN;
	}

	virtual void store() override {
		DEBUG_LOG("  [+] Create event listener \n\n");

		__interfaces->game_event_manager->add_listener_global(this, false);
	}

	virtual void destroy() override {
		__interfaces->game_event_manager->remove_listener(this);
	}

	virtual void fire_game_event(c_game_event* event) override;

	virtual int get_event_debug_id() {
		return debug_id;
	}
};

#ifdef _DEBUG
inline auto __eventlistener = std::make_unique<c_event_listener>();
#else
CREATE_DUMMY_PTR(c_event_listener);
DECLARE_XORED_PTR(c_event_listener, GET_XOR_KEYUI32);

#define __eventlistener XORED_PTR(c_event_listener)
#endif