#include "ae86.hpp"

/*
	Made with LOVE by Soufiw & infirms
	07-06-23
 */
namespace ae86 {
	win::_TEB32* nt_current_teb() noexcept {
		auto teb = static_cast<std::uintptr_t>(0);
		auto offset = static_cast<std::uint32_t>(FIELD_OFFSET(NT_TIB, Self));
		teb = __readfsdword(offset);

		return reinterpret_cast<win::_TEB32*>(teb);
	}

	win::_PEB32* nt_current_peb() noexcept {
		const auto teb = nt_current_teb();

		if (teb) return teb->ProcessEnvironmentBlock;

		return nullptr;
	}

	const IMAGE_DOS_HEADER* get_image_dos_header(std::uintptr_t image) {
		if (!image) return nullptr;

		const auto image_dos_header =
			reinterpret_cast<const IMAGE_DOS_HEADER*>(image);

		if (image_dos_header->e_magic != IMAGE_DOS_SIGNATURE) return nullptr;

		return image_dos_header;
	}

	const IMAGE_NT_HEADERS* get_image_nt_headers(std::uintptr_t image) {
		const auto image_dos_header = get_image_dos_header(image);

		if (!image_dos_header) return nullptr;

		const auto image_nt_headers = reinterpret_cast<const IMAGE_NT_HEADERS*>(
			image + image_dos_header->e_lfanew);

		if (image_nt_headers->Signature != IMAGE_NT_SIGNATURE) return nullptr;

		return image_nt_headers;
	}

	const IMAGE_DATA_DIRECTORY* get_image_data_directory(std::uintptr_t image,
		std::uint16_t directory) {
		const auto image_nt_headers = get_image_nt_headers(image);

		if (!image_nt_headers) return nullptr;

		if (directory >= IMAGE_NUMBEROF_DIRECTORY_ENTRIES) {
			return nullptr;
		}

		return &image_nt_headers->OptionalHeader.DataDirectory[directory];
	}

	const win::_LDR_DATA_TABLE_ENTRY* get_ldr_data_table_entry(
		const LIST_ENTRY* list_head, hash_t name_hash) {
		for (auto list_entry = list_head->Flink; list_entry != list_head;
			list_entry = list_entry->Flink) {
			const auto ldr_data_table_entry = CONTAINING_RECORD(
				list_entry, win::_LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

			if (ldr_data_table_entry) {
				const auto& base_name = ldr_data_table_entry->BaseDllName;

				if (base_name.Length > 0) {
					if (HASH(base_name) == name_hash) {
						return ldr_data_table_entry;
					}
				}
			}
		}

		return nullptr;
	}

	std::uintptr_t get_image(hash_t name_hash) {
		const auto peb = nt_current_peb();

		if (!peb) return NULL;

		const auto peb_ldr = peb->Ldr;

		if (!peb_ldr) return NULL;

		const auto ldr_data_table_entry =
			get_ldr_data_table_entry(&peb_ldr->InLoadOrderModuleList, name_hash);

		if (!ldr_data_table_entry) return NULL;

		return reinterpret_cast<std::uintptr_t>(ldr_data_table_entry->DllBase);
	}

	void c_syscall_parser::parse() {
		const std::uintptr_t ntdll_addr = get_image(HASH_CT(L"ntdll.dll"));

		if (!ntdll_addr) return;

		const auto image_data_directory =
			get_image_data_directory(ntdll_addr, IMAGE_DIRECTORY_ENTRY_EXPORT);

		if (!image_data_directory) return;

		const auto image_export_directory =
			reinterpret_cast<const IMAGE_EXPORT_DIRECTORY*>(
				ntdll_addr + image_data_directory->VirtualAddress);

		if (!image_export_directory) return;

		if (!image_export_directory->Base ||
			!image_export_directory->AddressOfFunctions ||
			!image_export_directory->AddressOfNames ||
			!image_export_directory->AddressOfNameOrdinals ||
			!image_export_directory->NumberOfFunctions ||
			!image_export_directory->NumberOfNames) {
			return;
		}

		const auto functions = reinterpret_cast<const std::uint32_t*>(
			ntdll_addr + image_export_directory->AddressOfFunctions);
		const auto names = reinterpret_cast<const std::uint32_t*>(
			ntdll_addr + image_export_directory->AddressOfNames);
		const auto name_ordinals = reinterpret_cast<const std::uint16_t*>(
			ntdll_addr + image_export_directory->AddressOfNameOrdinals);

		for (std::size_t index = 0; index < image_export_directory->NumberOfFunctions;
			index++) {
			const auto export_ordinal = name_ordinals[index];
			const auto export_function = functions[export_ordinal];

			if (export_function) {
				const auto export_name =
					reinterpret_cast<char*>(ntdll_addr + names[index]);
				const auto export_addr = ntdll_addr + export_function;
				const auto is_syscall = *reinterpret_cast<BYTE*>(export_addr) == 0xB8;

				if (export_name[0] == 'N' && export_name[1] == 't') {
					if (is_syscall) {
						const auto syscall_index =
							*reinterpret_cast<int*>(export_addr + 1) & 0xFFF;
						const auto syscall_hash = HASH(export_name);

						syscalls_[syscall_hash] = syscall_index;
						/*std::cout << std::format("Name: {}\nIndex: {:#x}\nHash: {:#x}\n",
												 export_name, syscall_index, syscall_hash);*/
					}
				}
			}
		}
	}

	c_syscall_parser::c_syscall_parser() : count_(0), syscalls_() { }

	c_syscall_parser::~c_syscall_parser() {}
}  // namespace ae86