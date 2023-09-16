#pragma once

#include "../security/security.hpp"

#define IS_IN_RANGE(value, max, min) (value >= max && value <= min)
#define GET_BITS(value) (IS_IN_RANGE(value, '0', '9') ? (value - '0') : ((value & (~0x20)) - 'A' + 0xA))
#define GET_BYTE(value) (GET_BITS(value[0]) << 4 | GET_BITS(value[1]))

#define PAD(size) std::byte MARCO_CONCAT(_pad, __COUNTER__) [size]

#define VFUNC(name, type, index, ...) \
	inline auto name { \
		using fn_t = type; \
		return __memory->get_virtual(address_t(this), XORN(index)).cast<fn_t>()(this, __VA_ARGS__); \
	}

#define PFUNC(name, type, address, ...) \
	inline auto name { \
		static auto fn = address.cast<type>(); \
		return fn(this, __VA_ARGS__); \
	}

#define PFUNC2(name, type, address, ...) \
	inline auto name { \
		static auto fn = address.cast<type>(); \
		return fn(__VA_ARGS__); \
	}

#define OFFSET(name, type, offset) \
inline type name() { \
	return *(std::remove_reference_t<type>*)((std::uintptr_t)this + XORN(offset)); \
}

#define OFFSET_PTR(name, type, offset) \
inline type* name() { \
	return (type*)((std::uintptr_t)this + XORN(offset)); \
}

#define NETVAR(name, type, table, prop) \
inline type name() { \
	static std::uintptr_t offset = __netvars->get(HASH_CT(table), HASH_CT(prop)); \
	return *(std::remove_reference_t<type>*)((std::uintptr_t)this + offset); \
}

#define NETVAR_PTR(name, type, table, prop) \
inline type* name() { \
	static std::uintptr_t offset = __netvars->get(HASH_CT(table), HASH_CT(prop)); \
	return (type*)((std::uintptr_t)this + offset); \
}

#define NETVAR_OFFSET(name, type, table, prop, offset) \
inline type name() { \
	static std::uintptr_t _offset = __netvars->get(HASH_CT(table), HASH_CT(prop)) + XORN(offset); \
	return *(std::remove_reference_t<type>*)((std::uintptr_t)this + _offset); \
}

#define NETVAR_OFFSET_PTR(name, type, table, prop, offset) \
inline type* name() { \
	static std::uintptr_t _offset = __netvars->get(HASH_CT(table), HASH_CT(prop)) + XORN(offset); \
	return (std::remove_reference_t<type>*)((std::uintptr_t)this + _offset); \
}

#define DATAMAP(func_name, type, table, prop) \
inline type func_name() { \
	static std::uintptr_t offset = __netvars->get(HASH_CT(table), HASH_CT(prop)); \
	return *(std::remove_reference_t<type>*)((std::uintptr_t)this + offset); \
}

#define DATAMAP_PTR(func_name, type, table, prop) \
inline type* func_name() { \
	static std::uintptr_t offset = __netvars->get(HASH_CT(table), HASH_CT(prop)); \
	return (type*)((std::uintptr_t)this + offset); \
}

struct address_t {
	std::uintptr_t pointer{};

	INLINE constexpr address_t() : pointer(0) {}
	INLINE constexpr address_t(const std::uintptr_t& ptr) : pointer(ptr) {}
	INLINE constexpr address_t(const void* ptr) : pointer((std::uintptr_t)ptr) {}

	template <typename T = address_t>
	INLINE T cast() {
		if (pointer == 0)
			return {};

		return reinterpret_cast<T>(pointer);
	}

	template <typename T = address_t>
	INLINE T add(const std::size_t& amt) {
		if (pointer == 0)
			return {};

		return static_cast<T>(pointer + amt);
	}

	template <typename T = address_t>
	INLINE T sub(const std::size_t& amt) {
		if (pointer == 0)
			return {};

		return static_cast<T>(pointer - amt);
	}

	template <typename T = address_t>
	INLINE T deref() {
		if (pointer == 0)
			return {};

		return *reinterpret_cast<T*>(pointer);
	}

	template <typename T = address_t>
	INLINE T relative(const std::size_t& amt) {
		if (!pointer)
			return {};

		auto out = pointer + amt;
		auto r = *reinterpret_cast<std::uint32_t*>(out);

		if (!r)
			return {};

		out = (out + 4) + r;

		return (T)out;
	}

	template <typename T = address_t>
	INLINE T at(std::size_t n) {
		return *reinterpret_cast<T*>(pointer + n);
	}

	template <typename T = address_t>
	INLINE T get(std::size_t n = 1) {
		std::uintptr_t out{};

		if (!pointer)
			return T{ };

		out = pointer;

		for (std::size_t i = n; i > 0; --i) {
			if (!valid(out))
				return T{ };

			out = *reinterpret_cast<std::uintptr_t*>(out);
		}

		return (T)out;
	}

	static INLINE bool valid(std::uintptr_t addr) {
		MEMORY_BASIC_INFORMATION mbi{};

		if (!addr)
			return false;

		if (!VirtualQuery((const void*)addr, &mbi, sizeof(mbi)))
			return false;

		if ((mbi.Protect & PAGE_NOACCESS) || (mbi.Protect & PAGE_GUARD))
			return false;

		return true;
	}
};

struct bits_t {
	std::uint32_t bits{};

	bits_t() : bits(0) {}
	bits_t(const std::uint32_t& bit) : bits(bit) {}
	bits_t(const int& bit) : bits(static_cast<std::uint32_t>(bit)) {}

	INLINE bool has(const std::uint32_t& bit) {
		return bits & bit;
	}

	INLINE void force(const std::uint32_t& bit) {
		bits |= bit;
	}

	INLINE void remove(const std::uint32_t& bit) {
		bits &= ~bit;
	}

	INLINE bool band(const std::uint32_t& bit) {
		return bits & bit;
	}
};

template <typename T>
class c_restore {
private:
	T* stored{};
	T backup{};
public:
	INLINE explicit c_restore(T* orig) {
		stored = orig;
		backup = *orig;
	}

	INLINE ~c_restore() {
		*stored = std::move(backup);
	}
};

using patterns_t = std::vector<address_t>;
using pattern_byte_t = std::pair<std::uint8_t, bool>;

class c_memory {
public:
	address_t find_pattern(std::uintptr_t module, std::string name, std::optional<std::size_t> length = {});
	address_t find_interface(std::uintptr_t module, std::string name);

	patterns_t find_all_patterns(std::uintptr_t module, std::string name);

	inline address_t get_virtual(address_t ptr, int idx) {
		return ((*ptr.cast<void***>())[idx]);
	}
};

#ifdef _DEBUG
inline auto __memory = std::make_unique<c_memory>();
#else
CREATE_DUMMY_PTR(c_memory);
DECLARE_XORED_PTR(c_memory, GET_XOR_KEYUI32);

#define __memory XORED_PTR(c_memory)
#endif

#define RESTORE(original) auto MARCO_CONCAT( backup, __COUNTER__ ) = c_restore{ &original }