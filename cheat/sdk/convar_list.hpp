#pragma once

class c_convar;

class c_convar_list : public c_sdk {
private:
	std::unordered_map<std::string_view, c_convar*> list{};

public:
	c_convar* get(std::string_view str) {
		return list[str];
	}

	virtual void store() override;
	virtual void map_init_post() override;
};

#ifdef _DEBUG
inline auto __convars = std::make_unique<c_convar_list>();
#else
CREATE_DUMMY_PTR(c_convar_list);
DECLARE_XORED_PTR(c_convar_list, GET_XOR_KEYUI32);

#define __convars XORED_PTR(c_convar_list)
#endif