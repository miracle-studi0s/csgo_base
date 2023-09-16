#pragma once

class c_netvars : public c_sdk {
private:
	struct netvar_data_t {
		bool datamap_var;
		c_recv_prop* prop_ptr;
		std::size_t offset;

		netvar_data_t() : datamap_var{}, prop_ptr{}, offset{} { }
	};

	std::unordered_map<hash_t, std::unordered_map<hash_t, netvar_data_t>> offsets{};

	void store_data_table(const std::string& name, c_recv_table* table, std::size_t offset = 0);
	void store_netvars();
	void store_datamaps();
public:
	INLINE std::size_t get(hash_t table, hash_t prop) {
		return offsets[table][prop].offset;
	}

	INLINE c_recv_prop* get_prop_ptr(hash_t table, hash_t prop) {
		return offsets[table][prop].prop_ptr;
	}

	virtual void store() override;
	virtual void destroy() override;
};

#ifdef _DEBUG
inline auto __netvars = std::make_unique<c_netvars>();
#else
CREATE_DUMMY_PTR(c_netvars);
DECLARE_XORED_PTR(c_netvars, GET_XOR_KEYUI32);

#define __netvars XORED_PTR(c_netvars)
#endif