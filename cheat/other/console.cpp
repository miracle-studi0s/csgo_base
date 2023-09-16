#include "console.hpp"
#include "../main_definitions.hpp"

#ifdef _DEBUG

namespace console {
	FILE* stream = NULL;

	void create() {
		AllocConsole();
		freopen_s(&stream, ("CONIN$"), ("r"), stdin);
		freopen_s(&stream, ("CONOUT$"), ("w"), stdout);
		freopen_s(&stream, ("CONOUT$"), ("w"), stderr);
	}

	void destroy() {
		HWND console = GetConsoleWindow();
		FreeConsole();
		PostMessage(console, WM_CLOSE, 0, 0);
		fclose(stream);
	}
}

#endif