#pragma once

#include "../../imgui/imgui.h"
#include "../../imgui/imgui_impl_dx9.h"
#include "../../imgui/imgui_impl_win32.h"
#include "../../imgui/imgui_internal.h"
#include "../../imgui/imgui_freetype.h"

#include <mutex>

struct render_font_t {
	ImFont* font{};
	float size{};

	INLINE void initalize_path(ImGuiIO& io, const char* path, float font_size, ImFontConfig* cfg) {
		font = io.Fonts->AddFontFromFileTTF(path, font_size, cfg, io.Fonts->GetGlyphRangesCyrillic());
		size = font_size;
	}
};

class c_renderer : public c_render, c_sdk {
private:
	ImDrawList* draw_list_act{};
	ImDrawList* draw_list_rendering{};
	ImDrawData draw_data{};
	ImDrawListSharedData _data{};
	
	render_font_t test_font{};

	std::mutex render_mutex{};
public:
	bool initialized = false;
	vec2_t screen_size{};

	void begin_scene();
	void end_scene();
	void clear_drawlist();

	const matrix3x4_t& world_to_screen_matrix();
	bool screen_transform(const vec3_t& source, vec2_t& output);
	bool world_to_screen(const vec3_t& src, vec2_t& out, bool offscreen = false);

	virtual void store() override;
	virtual void screen_change() override;
	virtual void init() override;
	virtual void start() override;
	virtual void end() override;
	virtual void reset() override;
	virtual void destroy() override;
	virtual void frame_post(int stage) override;
};

#ifdef _DEBUG
inline auto __render = std::make_unique<c_renderer>();
#else
CREATE_DUMMY_PTR(c_renderer);
DECLARE_XORED_PTR(c_renderer, GET_XOR_KEYUI32);

#define __render XORED_PTR(c_renderer)
#endif