#include "../../callbacks.hpp"
#include "../../main_definitions.hpp"
#include "../../security/security.hpp"
#include "../../sdk/sdk.hpp"
#include "game_information.hpp"
#include "engine_prediction.hpp"
#include "../../hooks/hooks.hpp"

// nothing to say.. just usual engine prediction 
// required to process current commands and calculate input logic at time when we modify commands
// also i pasted from skeet netvar compression & viewmodel repredict fix

void c_engine_prediction::update_ingame_prediction() {
	__interfaces->prediction->update(__interfaces->client_state->delta_tick,
		__interfaces->client_state->delta_tick > 0,
		__interfaces->client_state->last_command_ack,
		__interfaces->client_state->last_outgoing_command + __interfaces->client_state->choked_commands);
}

void c_engine_prediction::update_predicted_tickbase(c_user_cmd* cmd) {
	if (cmd != nullptr) {
		static c_user_cmd* last_cmd = nullptr;

		// if command was not predicted - increment tickbase
		if (last_cmd == nullptr || last_cmd->has_been_predicted)
			predicted_tickbase = __local_player->tickbase();
		else
			predicted_tickbase++;

		last_cmd = cmd;
	}
}

void c_engine_prediction::update_viewmodel_info(c_user_cmd* cmd) {
	auto viewmodel = __local_player->viewmodel().get().cast<c_cs_player*>();
	if (!viewmodel)
		return;

	auto& current_viewmodel = viewmodel_info[cmd->command_number % 150];
	current_viewmodel.store(cmd, viewmodel);

	if (cmd->weapon_select) {
		auto previous_command = (cmd->command_number - 1);
		auto previous_viewmodel = viewmodel_info[previous_command % 150];

		if (previous_viewmodel.cmd_tick == previous_command && previous_viewmodel.model_index != viewmodel->model_index()) {
			auto round_sequence = (previous_viewmodel.new_sequence_parity + 1) & 7;
			if (((round_sequence + 1) & 7) == current_viewmodel.new_sequence_parity) {
				current_viewmodel.new_sequence_parity = round_sequence;
				viewmodel->new_sequence_parity() = round_sequence;
			}
		}
	}
}

void c_engine_prediction::pre_predict(c_user_cmd* cmd) {
	update_ingame_prediction();
	update_predicted_tickbase(cmd);

	if (!prediction_random_seed)
		prediction_random_seed = *FIND_OFFSET(__modules->client, OFFSET_PREDICTION_RANDOM_SEED).cast<int**>();

	if (!prediction_player)
		prediction_player = *FIND_OFFSET(__modules->client, OFFSET_PREDICTION_PLAYER).cast<int**>();

	if (!__local_player->is_alive())
		return;

	in_prediction = true;

	unpred_vars.store();

	unpred_networked_vars[cmd->command_number % 150].store(cmd);

	__interfaces->global_vars->curtime = TICKS_TO_TIME(predicted_tickbase);
	__interfaces->global_vars->frametime = __local_player->flags().has(FL_FROZEN) ? 0.f : __interfaces->global_vars->interval_per_tick;

#ifdef LEGACY
	*reinterpret_cast<c_user_cmd**>(reinterpret_cast<std::uintptr_t>(__local_player) + XORN(0x3314)) = cmd;
	*reinterpret_cast<c_user_cmd*>(reinterpret_cast<std::uintptr_t>(__local_player) + XORN(0x326C)) = *cmd;
#else
	*reinterpret_cast<c_user_cmd**>(reinterpret_cast<std::uintptr_t>(__local_player) + XORN(0x3348)) = cmd;
	*reinterpret_cast<c_user_cmd*>(reinterpret_cast<std::uintptr_t>(__local_player) + XORN(0x3298)) = *cmd;
#endif

	*prediction_random_seed = MD5_PseudoRandom(cmd->command_number) & 0x7FFFFFFF;
	*prediction_player = reinterpret_cast<int>(__local_player);

	__interfaces->prediction->in_prediction = true;
	__interfaces->prediction->is_first_time_predicted = false;

	auto old_tickbase = __local_player->tickbase();

	std::memset(&move_data, 0, sizeof(c_move_data));

#ifndef LEGACY
	cmd->buttons.force(__local_player->button_forced());
	cmd->buttons.remove(__local_player->button_disabled());
#endif

	__interfaces->game_movement->start_track_prediction_errors(__local_player);

	auto& new_vars = networked_vars[cmd->command_number % 150];
	new_vars.ground_entity.set_value(__local_player->ground_entity().get_value());

	const auto buttons = cmd->buttons;
	const int local_buttons = *__local_player->buttons();
	const int buttons_changed = buttons.bits ^ local_buttons;

	__local_player->button_last() = local_buttons;
	*__local_player->buttons() = buttons.bits;
	__local_player->button_pressed() = buttons_changed & buttons.bits;
	__local_player->button_released() = buttons_changed & (~buttons.bits);

	__interfaces->prediction->check_moving_ground(__local_player, __interfaces->global_vars->frametime);
	__interfaces->prediction->set_local_view_angles(cmd->viewangles);

	__local_player->run_pre_think();
	__local_player->run_think();

	__interfaces->move_helper->set_host(__local_player);
	__interfaces->prediction->setup_move(__local_player, cmd, __interfaces->move_helper, &move_data);
	__interfaces->game_movement->process_movement(__local_player, &move_data);
	__interfaces->prediction->finish_move(__local_player, cmd, &move_data);

	__interfaces->move_helper->process_impacts();

	__local_player->run_post_think();

	__interfaces->game_movement->finish_track_prediction_errors(__local_player);
	__interfaces->move_helper->set_host(nullptr);

	if (__local_weapon)
		__local_weapon->update_accuracy_penalty();

	new_vars.store(cmd, true);

	__local_player->tickbase() = old_tickbase;
}

void c_engine_prediction::repredict(c_user_cmd* cmd) {
	if (!__local_weapon)
		return;

	if (!__local_player->is_alive())
		return;

	std::memset(&move_data, 0, sizeof(c_move_data));

	__interfaces->game_movement->start_track_prediction_errors(__local_player);

	move_data.forwardmove = cmd->forwardmove;
	move_data.sidemove = cmd->sidemove;
	move_data.upmove = cmd->upmove;
	move_data.buttons = cmd->buttons;
	move_data.view_angles = cmd->viewangles;
	move_data.angles = cmd->viewangles;

	__interfaces->move_helper->set_host(__local_player);
	__interfaces->prediction->setup_move(__local_player, cmd, __interfaces->move_helper, &move_data);
	__interfaces->game_movement->process_movement(__local_player, &move_data);
	__interfaces->prediction->finish_move(__local_player, cmd, &move_data);
	__interfaces->game_movement->finish_track_prediction_errors(__local_player);
	__interfaces->move_helper->set_host(nullptr);
}

void c_engine_prediction::post_predict(c_user_cmd* cmd) {
	if (!__local_player->is_alive())
		return;

	in_prediction = false;

#ifdef LEGACY
	*reinterpret_cast<c_user_cmd**>(reinterpret_cast<std::uintptr_t>(__local_player) + XORN(0x3314)) = nullptr;
#else
	*reinterpret_cast<c_user_cmd**>(reinterpret_cast<std::uintptr_t>(__local_player) + XORN(0x3348)) = nullptr;
#endif

	*prediction_random_seed = -1;
	*prediction_player = 0;

	unpred_vars.restore();

	__interfaces->game_movement->reset();
}

void c_engine_prediction::run_command(c_user_cmd* cmd) {
	if (!__local_player->is_alive())
		return;

	sent_vars[cmd->command_number % 150].store(cmd);
	update_viewmodel_info(cmd);
}

void c_engine_prediction::map_init_pre() {
	reset_all();
}

void c_engine_prediction::map_init_post() {
	reset_all();
}

void c_engine_prediction::map_shutdown() {
	reset_all();
}

void c_engine_prediction::tick_local_death() {
	if (__local_player->is_alive())
		return;

	reset_all();
}

// always send to server information even when you choke
void c_engine_prediction::tick_end(bool* send_packet, int sequence_number) {
	auto cmd = __interfaces->input->get_user_cmd(sequence_number);
	if (__interfaces->client_state && cmd) {
		auto net_channel = __interfaces->client_state->net_channel;
		if (net_channel) {
			if (!send_packet && net_channel->choked_packets > 0) {
				RESTORE(net_channel->choked_packets);

				net_channel->choked_packets = 0;
				net_channel->send_datagram();
				--net_channel->out_sequence_nr;
			}
			else
				outgoing_commands.emplace_back(cmd->command_number);
		}
	}
}

// fix issue when game attempts to override server viewmodel values to predicted
// and plays animations & sound twice
void c_engine_prediction::frame(int stage) {
	if (stage != XORN(FRAME_NET_UPDATE_POSTDATAUPDATE_START))
		return;

	if (!__game_info->connected)
		return;

	if (!__local_player || !__local_player->is_alive())
		return;

	auto viewmodel = __local_player->viewmodel().get().cast<c_cs_player*>();
	if (!viewmodel)
		return;

	//printf("%s \n", viewmodel->get_client_class()->network_name);

	auto command = __interfaces->client_state->command_ack;
	auto current_viewmodel = &viewmodel_info[command % 150];
	if (current_viewmodel->cmd_tick != command)
		return;

	if (viewmodel->anim_time() != viewmodel->old_anim_time()
		&& viewmodel->cycle() != viewmodel->old_cycle()
		&& viewmodel->cycle() == 0.f && viewmodel->anim_time() == __interfaces->global_vars->curtime) {
		if (current_viewmodel->active_weapon == __local_player->active_weapon().get().cast<c_base_entity*>()
			&& current_viewmodel->sequence == viewmodel->sequence()
			&& current_viewmodel->animation_parity == viewmodel->animation_parity()
			&& current_viewmodel->new_sequence_parity == viewmodel->new_sequence_parity()
			&& current_viewmodel->model_index == viewmodel->model_index()
			|| current_viewmodel->looking_at_weapon == __local_player->looking_at_weapon())
		{
			viewmodel->anim_time() = viewmodel->old_anim_time();
			viewmodel->cycle() = viewmodel->old_cycle();
		}
	}
}

void c_engine_prediction::on_packet_start(c_client_state* ecx, void* edx, int incoming, int outgoing) {
	static auto original = __hooks->get_original(&packet_start);

	if (std::find(outgoing_commands.begin(), outgoing_commands.end(), outgoing) != outgoing_commands.end())
		original(ecx, edx, incoming, outgoing);

	std::erase_if(outgoing_commands, [&](int command) {
		return std::abs(command - outgoing) >= 150 || command < outgoing;
	});
}

void c_engine_prediction::on_packet_end(int command) {
	should_repredict = false;

	if (__local_weapon) {
		auto sequence = __local_weapon->get_sequence_activity(__local_weapon->sequence());
		if (sequence >= 0 && sequence == ACT_VM_DRAW)
			__local_weapon->activity() = ACT_VM_DRAW;
	}

	auto sent_var = &sent_vars[command % 150];
	if (sent_var->command_number != command)
		return;

	auto aim_punch_diff = sent_var->aimpunch - __local_player->aim_punch_angle();
	auto view_punch_diff = sent_var->viewpunch - __local_player->view_punch_angle();
	auto aim_punch_vel_diff = sent_var->aimpunch_vel - __local_player->aim_punch_angle_vel();
	auto view_offset_diff = sent_var->viewoffset - __local_player->view_offset();
	auto fall_velocity_diff = sent_var->fall_velocity - __local_player->fall_velocity();
	auto velocity_modifier_diff = sent_var->velocity_modifier - __local_player->velocity_modifier();
	auto base_velocity_diff = sent_var->base_velocity - __local_player->base_velocity();
	auto velocity_diff = sent_var->velocity - __local_player->velocity();
	auto network_origin_diff = sent_var->network_origin - __local_player->network_origin();

	{
		if (std::abs(aim_punch_diff.x) <= 0.03125f && std::abs(aim_punch_diff.y) <= 0.03125f && std::abs(aim_punch_diff.z) <= 0.03125f)
			__local_player->aim_punch_angle() = sent_var->aimpunch;
		else
			should_repredict = true;

		if (std::abs(view_punch_diff.x) <= 0.03125f && std::abs(view_punch_diff.y) <= 0.03125f && std::abs(view_punch_diff.z) <= 0.03125f)
			__local_player->view_punch_angle() = sent_var->viewpunch;
		else
			should_repredict = true;

		if (std::abs(aim_punch_vel_diff.x) <= 0.03125f && std::abs(aim_punch_vel_diff.y) <= 0.03125f && std::abs(aim_punch_vel_diff.z) <= 0.03125f)
			__local_player->aim_punch_angle_vel() = sent_var->aimpunch_vel;
		else
			should_repredict = true;

		if (std::abs(view_offset_diff.z) <= 0.065f)
			__local_player->view_offset() = sent_var->viewoffset;
		else
			should_repredict = true;

		if (std::abs(fall_velocity_diff) <= 0.5f)
			__local_player->fall_velocity() = sent_var->fall_velocity;
		else
			should_repredict = true;
	}

	if (__local_player->stamina() == std::clamp(sent_var->stamina, 0.f, 100.f))
		__local_player->stamina() = sent_var->stamina;
}