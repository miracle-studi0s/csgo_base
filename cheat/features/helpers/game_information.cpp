#include "../../callbacks.hpp"
#include "../../main_definitions.hpp"
#include "../../security/security.hpp"
#include "../../sdk/sdk.hpp"
#include "engine_prediction.hpp"
#include "game_information.hpp"
#include "backtracking.hpp"

void c_game_information::pre_predict(c_user_cmd* cmd) {
}

void c_game_information::predict(c_user_cmd* cmd) {
	predicted_time = TICKS_TO_TIME(__engine_prediction->get_tickbase());
}

void c_game_information::post_predict(c_user_cmd* cmd) {
}

void c_game_information::map_init_pre() {
	reset();
}

void c_game_information::map_shutdown() {
	reset();
}

void c_game_information::frame(int stage) {
	auto netchan = __interfaces->engine->get_net_channel();
	if (!netchan)
		return;

	static auto cl_lagcompensation = __convars->get(XOR("cl_lagcompensation"));
	static auto sv_maxunlag = __convars->get(XOR("sv_maxunlag"));

	tickrate = static_cast<int>(1.f / __interfaces->global_vars->interval_per_tick);
	connected = __interfaces->client_state->signon_state == SIGNONSTATE_FULL;
	cl_lagcomp0 = cl_lagcompensation->get_int() == 0;

	lerp_time = __backtracking->get_lerp_time();
	max_unlag = sv_maxunlag->get_float();

	ping = netchan->get_latency(FLOW_OUTGOING) + netchan->get_latency(FLOW_INCOMING);
}

void c_game_information::tick(bool* send_packet) {
	this->send_packet = send_packet;
}