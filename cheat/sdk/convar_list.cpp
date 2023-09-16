#include "../callbacks.hpp"
#include "../main_definitions.hpp"
#include "../security/security.hpp"

#include "memory.hpp"
#include "convar_list.hpp"
#include "interfaces.hpp"

void c_convar_list::store() {
	DEBUG_LOG("  [+] Store convars list \n\n");

	const auto iterator = __interfaces->cvar->factory_internal_iterator();
	for (iterator->set_first(); iterator && iterator->is_valid(); iterator->next()) {
		auto cvar = iterator->get();
		if (!cvar)
			continue;

		list[cvar->name] = reinterpret_cast<c_convar*>(cvar);
	}
}

void c_convar_list::map_init_post() {
	store();
}