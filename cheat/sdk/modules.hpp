#pragma once

#include "../callbacks.hpp"
#include "../main_definitions.hpp"
#include "../security/security.hpp"

#include <memory>

class c_modules : public c_sdk {
public:
	std::uintptr_t client{};
	std::uintptr_t server{};
	std::uintptr_t engine{};
	std::uintptr_t materialsystem{};
	std::uintptr_t tier0{};
	std::uintptr_t shaderapidx9{};
	std::uintptr_t vgui2{};
	std::uintptr_t inputsystem{};
	std::uintptr_t vstdlib{};
	std::uintptr_t localize{};
	std::uintptr_t datacache{};
	std::uintptr_t studiorender{};
	std::uintptr_t vguimatsurface{};
	std::uintptr_t vphysics{};
	std::uintptr_t gameoverlayrenderer{};
	std::uintptr_t ntdll{};

	HWND window{};

	virtual void store() override;
};

#ifdef _DEBUG
inline auto __modules = std::make_unique<c_modules>();
#else
CREATE_DUMMY_PTR(c_modules);
DECLARE_XORED_PTR(c_modules, GET_XOR_KEYUI32);

#define __modules XORED_PTR(c_modules)
#endif