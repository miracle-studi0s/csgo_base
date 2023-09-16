#pragma once

#include "main_definitions.hpp"
#include "callbacks.hpp"
#include "security/security.hpp"

class c_cheat : public c_dllmain {
private:
	ae86::c_syscall_parser syscall_parser{};

public:
#ifdef _DEBUG
	bool unload = false;

	HMODULE module{};
#endif

	virtual void start(LPVOID reserved) override;
	virtual void destroy() override;
};

#ifdef _DEBUG
inline auto __cheat = std::make_unique<c_cheat>();
#else
CREATE_DUMMY_PTR(c_cheat);
DECLARE_XORED_PTR(c_cheat, GET_XOR_KEYUI32);

#define __cheat XORED_PTR(c_cheat)
#endif