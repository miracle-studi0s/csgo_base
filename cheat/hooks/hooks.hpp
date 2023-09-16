#include <MinHook.h>
#include "../main_definitions.hpp"
#include "../security/security.hpp"
#include "../callbacks.hpp"

constexpr auto MAX_HOOKS = 150;

class c_recv_prop;

class c_hooks : public c_sdk {
private: 
	struct hook_t {
		bool enabled = false;
		void* target = nullptr;
		void* original = nullptr;
		void* custom = nullptr;

		INLINE void enable() {
			MH_EnableHook(target);
			enabled = true;
		}

		INLINE void disable() {
			MH_DisableHook(target);
			enabled = false;
		}
	};

	std::vector<hook_t> hooks{};
public:
	WNDPROC old_wndproc{};

	c_hooks() { 
		hooks.reserve(MAX_HOOKS); 
	}

	template<typename fn = std::uintptr_t>
	INLINE bool add_hook(fn custom_func, void* original_func) {
		hook_t hook{};
		hook.target = original_func;
		hook.custom = custom_func;

		if (MH_CreateHook(original_func, custom_func, &hook.original) == MH_OK) {
			hooks.emplace_back(hook);
			return true;
		}
		return false;
	}

	template <typename T = void*>
	INLINE T get_original(T callback) {
		auto found = std::find_if(hooks.begin(), hooks.end(), [&](hook_t hook) { return hook.custom == callback; });
		if (found != hooks.end())
			return (T)found->original;

		return nullptr;
	}

	INLINE void hook_all() {
		for (auto& i : hooks)
			i.enable();
	}

	INLINE void unhook_all() {
		for (auto& i : hooks)
			i.disable();
	}

	void init_vmt_hooks();
	void init_detour_hooks();
	void init_proxy_hooks();
	void init_winapi_hooks();

	virtual void store() override;
	virtual void destroy() override;
};

#ifdef _DEBUG
inline auto __hooks = std::make_unique<c_hooks>();
#else
CREATE_DUMMY_PTR(c_hooks);
DECLARE_XORED_PTR(c_hooks, GET_XOR_KEYUI32);

#define __hooks XORED_PTR(c_hooks)
#endif