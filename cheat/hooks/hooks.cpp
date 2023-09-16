#include "hooks.hpp"
#include "../sdk/modules.hpp"

void c_hooks::store() {
	DEBUG_LOG("  [+] Create hooks \n\n");

	MH_Initialize();

	init_vmt_hooks();
	init_detour_hooks();
	//init_proxy_hooks();

	hook_all();

	init_winapi_hooks();
}

void c_hooks::destroy() {
	unhook_all();

	SetWindowLongPtrA(__modules->window, GWL_WNDPROC, (LONG_PTR)old_wndproc);

	MH_Uninitialize();
	hooks.clear();
}