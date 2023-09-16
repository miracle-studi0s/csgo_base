#include "../main_definitions.hpp"
#include "../security/security.hpp"

#include "math.hpp"
#include "memory.hpp"
#include "interfaces.hpp"
#include "valve_misc.hpp"

#pragma region ENTITIES
address_t c_base_handle::get() {
	return __interfaces->entity_list->get_client_entity_handle(value);
}
#pragma endregion