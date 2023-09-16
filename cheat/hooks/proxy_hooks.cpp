#include "../sdk/sdk.hpp"
#include "../render/renderer.hpp"
#include "../cheat.hpp"
#include "../features/features.hpp"
#include "hooks.hpp"

recv_var_proxy_fn original_simulation_time{};

void __cdecl simulation_time(c_recv_proxy_data* data, void* entity, void* out) {
	auto base_entity = (c_base_entity*)entity;
	if (base_entity && base_entity->is_player()) {
		
		printf("recv addt-> %d \n", data->value.int_);

		original_simulation_time(data, entity, out);
	}
	else
		original_simulation_time(data, entity, out);
}

void c_hooks::init_proxy_hooks() {
	auto m_flSimulationTime = __netvars->get_prop_ptr(HASH_CT("DT_CSPlayer"), HASH_CT("m_flSimulationTime"));
	if (m_flSimulationTime->proxy_fn) {
		original_simulation_time = m_flSimulationTime->proxy_fn;
		m_flSimulationTime->proxy_fn = (recv_var_proxy_fn)simulation_time;
	}
}