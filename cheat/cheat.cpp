#include "cheat.hpp"
#include "other/console.hpp"
#include <thread>
#include "sdk/sdk.hpp"
#include "render/renderer.hpp"

void threaded_init(LPVOID reserved) {
	DEBUG_LOG("  [+] Thread created, complete initializing.. \n\n");

	c_sdk::exec_store();

#ifdef _DEBUG
	while (!GetAsyncKeyState(VK_F11))
		std::this_thread::sleep_for(std::chrono::milliseconds(200));

	__cheat->unload = true;

	c_sdk::exec_destroy();
	c_dllmain::exec_destroy();

	FreeLibraryAndExitThread(__cheat->module, 0);
#endif
}

void c_cheat::start(LPVOID reserved) {
#ifdef _DEBUG
	console::create();
#endif
	DEBUG_LOG("Welcome to debug mode \n");
	DEBUG_LOG("Build date: %s \n\n", __DATE__);

	DEBUG_LOG("  [+] Create thread for cheat init \n\n");
	CreateThread(0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(threaded_init), reserved, 0, 0);
}

void c_cheat::destroy() {
#ifdef _DEBUG
	console::destroy();
#endif
}