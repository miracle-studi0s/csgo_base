#include "../callbacks.hpp"
#include "../main_definitions.hpp"
#include "../security/security.hpp"

#include "memory.hpp"
#include "interfaces.hpp"
#include "netvars.hpp"
#include "modules.hpp"

void c_netvars::store_data_table(const std::string& name, c_recv_table* table, std::size_t offset) {
	hash_t var, base{ HASH(name.c_str()) };
	c_recv_prop* prop;
	c_recv_table* child;

	for (int i = 0; i < table->props_count; ++i) {
		prop = &table->props[i];
		child = prop->data_table;

		if (child && child->props)
			store_data_table(name, child, prop->offset + offset);

		var = HASH(prop->prop_name);

		if (!offsets[base][var].offset) {
			offsets[base][var].datamap_var = false;
			offsets[base][var].prop_ptr = prop;
			offsets[base][var].offset = (std::size_t)(prop->offset + offset);
		}
	}
}

void c_netvars::store_netvars() {
	DEBUG_LOG("  [+] Store netvars \n\n");

	auto list = __interfaces->client->get_client_classes();
	if (!list)
		return;

	for (; list != nullptr; list = list->next_ptr)
		store_data_table(list->recvtable_ptr->table_name, list->recvtable_ptr);
}

void c_netvars::store_datamaps() {
	DEBUG_LOG("  [+] Store datamaps \n\n");

	auto matches = __memory->find_all_patterns(__modules->client, XOR("C7 05 ? ? ? ? ? ? ? ? C7 05 ? ? ? ? ? ? ? ? C3 CC"));
	if (matches.empty())
		return;

	for (std::size_t i = 0; i < matches.size(); ++i) {
		if (i == XORN(255) || i == XORN(108) || i == XORN(239))
			continue;

		auto& match = matches[i];

		auto map = match.at(XORN(2)).sub(XORN(4)).cast<datamap_t*>();
		if (!map || !map->size || map->size > XORN(200) || !map->desc || !map->name)
			continue;

		hash_t base{}, var{};
		typedescription_t* entry{};

		base = HASH(map->name);

		for (int j = 0; j < map->size; ++j) {
			auto entry = map->desc[j];
			if (!entry.name)
				continue;

			var = HASH(entry.name);

			if (!offsets[base][var].offset) {
				offsets[base][var].datamap_var = true;
				offsets[base][var].offset = (std::size_t)entry.offset;
				offsets[base][var].prop_ptr = nullptr;
			}
		}
	}
}

void c_netvars::store() {
	store_netvars();
	store_datamaps();
}

void c_netvars::destroy() {
	offsets.clear();
}