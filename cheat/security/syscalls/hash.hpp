#pragma once

#include <Windows.h>
#include <winternl.h>
#include <type_traits>

#include "ae86.hpp"

using hash_t = std::uint32_t;

namespace character {
	template <typename Type>
	constexpr bool is_upper(const Type character) {
		return (character >= static_cast<const Type>(65) &&
			character <= static_cast<const Type>(90));
	}

	template <typename Type>
	constexpr Type to_lower(const Type character) {
		if (is_upper(character)) {
			return (character + static_cast<const Type>(32));
		}

		return character;
	}

	template <typename Type>
	constexpr bool is_terminator(const Type character) {
		return (character == static_cast<const Type>(0));
	}

	template <typename Type>
	constexpr bool is_question(const Type character) {
		return (character == static_cast<const Type>(63));
	}

	template <typename Type>
	constexpr std::size_t get_length(const Type* const data) {
		std::size_t length = 0;

		while (true) {
			if (is_terminator(data[length])) {
				break;
			}

			length++;
		}

		return length;
	}

}  // namespace character


namespace hash {
	constexpr auto fnv1a32_prime = 0x1000193u;
	constexpr auto fnv1a32_basis = 0x811C9DC5u;

	template <typename Type>
	constexpr hash_t fnv1a32_compute(hash_t hash_basis, const Type* data,
		std::size_t size, bool ignore_case) {
		const auto element =
			static_cast<hash_t>(ignore_case ? character::to_lower(data[0]) : data[0]);
		return (size == 0) ? hash_basis
			: fnv1a32_compute((hash_basis ^ element) * fnv1a32_prime,
				data + 1, size - 1, ignore_case);
	}

	template <typename Type>
	constexpr hash_t fnv1a32(const Type* data, std::size_t size, bool ignore_case) {
		return fnv1a32_compute(fnv1a32_basis, data, size, ignore_case);
	}

	constexpr hash_t fnv1a32(const char* data, bool ignore_case) {
		const auto length = character::get_length(data);
		return fnv1a32(data, length, ignore_case);
	}

	constexpr hash_t fnv1a32(const wchar_t* data, bool ignore_case) {
		const auto length = character::get_length(data);
		return fnv1a32(data, length, ignore_case);
	}

	constexpr hash_t fnv1a32(const win::_UNICODE_STRING& data, bool ignore_case) {
		const auto length = data.Length / sizeof(wchar_t);
		return fnv1a32(data.Buffer, length, ignore_case);
	}

}  // namespace hash

#define HASH_CT(data)                                            \
  [&]() {                                                           \
    constexpr auto hash = hash::fnv1a32(data, true); \
    return hash;                                                    \
  }()

#define HASH(data) hash::fnv1a32(data, true)