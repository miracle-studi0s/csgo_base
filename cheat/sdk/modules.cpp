#include "modules.hpp"

#include <thread>

INLINE std::uintptr_t get_module_recursive(hash_t name) {
	std::uintptr_t out{};
	while (true) {
		out = ae86::get_image(name);
		if (out)
			break;

		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}
	return out;
}

void c_modules::store() {
	DEBUG_LOG("  [+] Store modules \n\n");

	while (!(window = FindWindowA(CXOR("Valve001"), NULL)))
		std::this_thread::sleep_for(std::chrono::milliseconds(200));

	while (!(gameoverlayrenderer = ae86::get_image(HASH_CT("gameoverlayrenderer.dll"))))
		std::this_thread::sleep_for(std::chrono::milliseconds(200));

	client				= get_module_recursive(HASH_CT("client.dll"));
	server				= get_module_recursive(HASH_CT("server.dll"));
	engine				= get_module_recursive(HASH_CT("engine.dll"));
	materialsystem		= get_module_recursive(HASH_CT("materialsystem.dll"));
	tier0				= get_module_recursive(HASH_CT("tier0.dll"));
	shaderapidx9		= get_module_recursive(HASH_CT("shaderapidx9.dll"));
	vgui2				= get_module_recursive(HASH_CT("vgui2.dll"));
	inputsystem			= get_module_recursive(HASH_CT("inputsystem.dll"));
	vstdlib				= get_module_recursive(HASH_CT("vstdlib.dll"));
	localize			= get_module_recursive(HASH_CT("localize.dll"));
	datacache			= get_module_recursive(HASH_CT("datacache.dll"));
	studiorender		= get_module_recursive(HASH_CT("studiorender.dll"));
	vguimatsurface		= get_module_recursive(HASH_CT("vguimatsurface.dll"));
	vphysics			= get_module_recursive(HASH_CT("vphysics.dll"));
	ntdll				= get_module_recursive(HASH_CT("ntdll.dll"));
}