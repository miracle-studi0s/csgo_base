#include "hooks.hpp"
#include "../sdk/modules.hpp"

LRESULT WINAPI wnd_proc(HWND wnd, std::uint32_t msg, WPARAM wp, LPARAM lp) {
	return CallWindowProcA(__hooks->old_wndproc, wnd, msg, wp, lp);
}

void c_hooks::init_winapi_hooks() {
	old_wndproc = (WNDPROC)SetWindowLongA(__modules->window, GWL_WNDPROC, (LONG_PTR)wnd_proc);
}