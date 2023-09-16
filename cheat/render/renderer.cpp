#include "../main_definitions.hpp"
#include "../callbacks.hpp"
#include "../sdk/sdk.hpp"
#include "renderer.hpp"

#include "../features/helpers/game_information.hpp"

void c_renderer::begin_scene() {
	if (!initialized)
		return;

	clear_drawlist();

	draw_list_act->_ResetForNewFrame();
	draw_list_act->PushClipRect({}, { screen_size.x, screen_size.y });
	draw_list_act->Flags = 0;
}

void c_renderer::end_scene() {
	if (!initialized)
		return;

	draw_list_act->PopClipRect();

	render_mutex.lock();
	*draw_list_rendering = *draw_list_act;
	draw_list_rendering->Flags = 0;
	render_mutex.unlock();
}

void c_renderer::clear_drawlist() {
	draw_data.Valid = false;
}

const matrix3x4_t& c_renderer::world_to_screen_matrix() {
	static auto view_matrix = *FIND_OFFSET(__modules->client, OFFSET_WORLD_TO_SCREEN_MATRIX).add(XORN(0x3)).cast<std::uintptr_t*>();
	static auto ptr = view_matrix + 176;
	return *reinterpret_cast<matrix3x4_t*>(ptr);
}

bool c_renderer::screen_transform(const vec3_t& source, vec2_t& output) {
	const matrix3x4_t& world_to_screen = world_to_screen_matrix();

	output.x = world_to_screen.mat[0][0] * source.x + world_to_screen.mat[0][1]
		* source.y + world_to_screen.mat[0][2] * source.z + world_to_screen.mat[0][3];

	output.y = world_to_screen.mat[1][0] * source.x + world_to_screen.mat[1][1]
		* source.y + world_to_screen.mat[1][2] * source.z + world_to_screen.mat[1][3];

	float w = world_to_screen.mat[3][0] * source.x + world_to_screen.mat[3][1]
		* source.y + world_to_screen.mat[3][2] * source.z + world_to_screen.mat[3][3];

	bool behind = false;
	if (w < 0.001f) {
		behind = true;

		float invw = -1.0f / w;
		output.x *= invw;
		output.y *= invw;
	}
	else {
		behind = false;

		float invw = 1.0f / w;
		output.x *= invw;
		output.y *= invw;
	}

	return behind;
}

bool c_renderer::world_to_screen(const vec3_t& src, vec2_t& out, bool offscreen) {
	bool st = this->screen_transform(src, out);

	float x = screen_size.x * 0.5f;
	float y = screen_size.y * 0.5f;

	x += 0.5f * out.x * screen_size.x + 0.5f;
	y -= 0.5f * out.y * screen_size.y + 0.5f;

	out.x = x;
	out.y = y;

	if (!offscreen && (out.x > screen_size.x || out.x < 0 || out.y > screen_size.y) || out.y < 0 || st)
		return false;

	return true;
}

void c_renderer::store() {
	DEBUG_LOG("  [+] Store screen size \n\n");

	int w{}, h{};
	__interfaces->engine->get_screen_size(w, h);

	screen_size = vec2_t(static_cast<float>(w), static_cast<float>(h));
}

void c_renderer::screen_change() {
	int w{}, h{};
	__interfaces->engine->get_screen_size(w, h);

	screen_size = vec2_t(static_cast<float>(w), static_cast<float>(h));
}

void c_renderer::init() {
	if (initialized)
		return;

	DEBUG_LOG("  [+] Init renderer \n\n");

	ImGui::CreateContext();

	ImGui_ImplDX9_Init(__interfaces->d3d_device);
	ImGui_ImplWin32_Init(__modules->window);

	_data = ImDrawListSharedData();

	draw_list_act = new ImDrawList(&_data);
	draw_list_rendering = new ImDrawList(&_data);

	auto& io = ImGui::GetIO();

	ImFontConfig cfg{};
	cfg.OversampleH = cfg.OversampleV = 1;
	cfg.PixelSnapH = true;

	io.Fonts->TexGlyphPadding = 2;

	test_font.initalize_path(io, "C:\\Windows\\Fonts\\consolab.ttf", 13.f, &cfg);

	ImGuiFreeType::BuildFontAtlas(io.Fonts);

	ImGui_ImplDX9_CreateDeviceObjects();

	initialized = true;
}

void c_renderer::start() {
	if (!initialized)
		return;

	ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX9_NewFrame();
	ImGui::NewFrame();
}

void c_renderer::end() {
	if (!initialized)
		return;

	ImGui::EndFrame();
	ImGui::Render();

	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	if (draw_list_rendering->VtxBuffer.Size > 0) {
		render_mutex.lock();

		ImDrawList* dl = draw_list_rendering;

		draw_data.Valid = true;
		draw_data.CmdLists = &dl;
		draw_data.CmdListsCount = 1;
		draw_data.TotalVtxCount = dl->VtxBuffer.size();
		draw_data.TotalIdxCount = dl->IdxBuffer.size();

		draw_data.DisplayPos = ImVec2{ 0.0f, 0.0f };
		draw_data.DisplaySize = ImVec2{ screen_size.x, screen_size.y };
		draw_data.FramebufferScale = ImVec2{ 1.0f, 1.0f };

		ImGui_ImplDX9_RenderDrawData(&draw_data);

		render_mutex.unlock();
	}
}

void c_renderer::reset() {
	if (render_mutex.try_lock())
		render_mutex.unlock();

	ImGui_ImplDX9_InvalidateDeviceObjects();
	ImGui_ImplDX9_CreateDeviceObjects();
}

void c_renderer::destroy() {
	if (!initialized)
		return;

	if (render_mutex.try_lock())
		render_mutex.unlock();

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();

	ImGui::DestroyContext();

	delete draw_list_act;
	delete draw_list_rendering;

	initialized = false;
}

void c_renderer::frame_post(int stage) {
	if (!initialized)
		return;

	if (stage != XORN(FRAME_RENDER_END))
		return;

	begin_scene(); {
		if (__game_info->connected) {
			__entlistener->for_each_player([&](c_cs_player* player) {
				if (!player->is_alive())
					return;

				auto player_max = player->get_render_origin() + vec3_t{0.f, 0.f, player->bb_maxs().z};

				vec2_t screen{};
				if (!world_to_screen(player_max, screen))
					return;

				auto& font = test_font.font;
				auto& size = test_font.size;

				draw_list_act->PushTextureID(font->ContainerAtlas->TexID);
				draw_list_act->AddText(font, size, { screen.x - 7, screen.y + 1.f }, ImColor{ 0, 0, 0, 255 }, player->get_name().c_str());
				draw_list_act->AddText(font, size, { screen.x, screen.y }, ImColor{ 255, 255, 255, 255 }, player->get_name().c_str());
				draw_list_act->PopTextureID();

				//draw_list_act->AddLine({ screen.x, screen.y }, { screen.x, screen.y + 100.f }, ImColor{ 255, 255, 255, 255 });

				//	draw_list_act->AddRect({ screen.x, screen.y }, { screen.x + 70.f, screen.y + 70.f }, ImColor{ 255, 255, 255, 255 });
			});
		}
	}
	end_scene();
}