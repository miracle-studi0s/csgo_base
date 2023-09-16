#include "../main_definitions.hpp"
#include "../security/security.hpp"

#include "memory.hpp"

address_t c_memory::find_pattern(std::uintptr_t module, std::string name, std::optional<std::size_t> length) {
	std::uint8_t* scan_start{}, *scan_end{};
	std::vector<pattern_byte_t> pattern{};
	std::stringstream stream{ name };
	std::string w{};

	auto len = 0;
	if (!length.has_value()) {
		auto nt_headers = ae86::get_image_nt_headers(module);
		if (!nt_headers)
			return 0u;

		len = nt_headers->OptionalHeader.SizeOfImage;
	}
	else
		len = *length;

	if (!module || !len || name.empty())
		return{};

	while (stream >> w) {
		if (w[0] == XORN('?'))
			pattern.push_back({ 0, true });
		else if (std::isxdigit(w[0]) && std::isxdigit(w[1]))
			pattern.push_back({ (std::uint8_t)std::strtoul(w.data(), 0, 16), false });
	}

	scan_start = reinterpret_cast<std::uint8_t*>(module);
	scan_end = scan_start + len;

	auto result = std::search(scan_start, scan_end, pattern.begin(), pattern.end(),
		[](const std::uint8_t b, const pattern_byte_t& p) {
			return b == p.first || p.second;
		});

	if (result == scan_end)
		return{};

	return reinterpret_cast<std::uintptr_t>(result);
}

address_t c_memory::find_interface(std::uintptr_t module, std::string name) {
	address_t fn = GetProcAddress(reinterpret_cast<HMODULE>(module), CXOR("CreateInterface"));
	if (fn.pointer == NULL)
		return 0u;

	return fn.cast<void*(*)(const char*, int)>()(name.c_str(), 0);
}

patterns_t c_memory::find_all_patterns(std::uintptr_t module, std::string name) {
	patterns_t out{};
	address_t result{};

	auto image_nt = ae86::get_image_nt_headers(module);
	if (!image_nt)
		return {};

	auto length = image_nt->OptionalHeader.SizeOfImage;

	for (;;) {
		result = find_pattern(module, name, length);
		if (!result.pointer)
			break;

		out.push_back(result);
		length = (module + length) - (result.pointer + 1);
		module = result.pointer + 1;
	}

	return out;
}