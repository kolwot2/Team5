#include "Controller.h"
#include "../Utils/JsonParser.h"
#include "../Utils/JsonWriter.h"
#include <chrono>
#include <memory>
#include <thread>
#include <sstream>

Controller::Controller() :logger("logs.txt") {}

void Controller::Init()
{
	Login login;
	connection.send(ActionMessage{ login });
	auto login_response = connection.recieve();
	logger << ige::FileLogger::e_logType::LOG_INFO << "Connected";

	connection.send(ActionMessage{ Action::GAMES,"" });
	auto recv = connection.recieve();

	auto& player = game.GetPlayer();
	player = JsonParser::ParsePlayer(login_response.data);

	connection.send(ActionMessage{ Action::MAP, JsonWriter::WriteMapLayer(0) });
	auto map_layer0_response = connection.recieve();
	auto& graph = game.GetGraph();
	graph = JsonParser::ParseGraph(map_layer0_response.data);

	connection.send(ActionMessage{ Action::MAP, JsonWriter::WriteMapLayer(1) });
	auto map_layer1_response = connection.recieve();
	auto& idx_to_post = game.GetPosts();
	auto [new_posts, new_trains, ratings] = JsonParser::ParseMapLayer1(map_layer1_response.data);
	idx_to_post = move(new_posts);

	connection.send(ActionMessage{ Action::MAP, JsonWriter::WriteMapLayer(10) });
	auto map_layer10_response = connection.recieve();
	auto coords = JsonParser::ParseCoordinates(map_layer10_response.data);
	for (const auto& i : coords) {
		graph.SetVertexCoordinates(i.first, i.second);
	}

	route_manager.Init(game);

	const auto& home = player.home_town;
	const auto& trains = player.trains;
}

void Controller::Disconnect()
{
	connection.send(ActionMessage(Action::LOGOUT, ""));
	//auto msg = connection.recieve();
	logger << ige::FileLogger::e_logType::LOG_INFO << "Disconnected";
}

const SynchronizedGame Controller::GetGame()
{
	return { game,std::lock_guard{game_mutex} };
}

void Controller::MakeTurn()
{
	auto start = std::chrono::steady_clock::now();
	UpdateGame();
	CheckTrainCrashed();
	CheckUpgrades();
	auto moves = route_manager.MakeMoves(game);
	SendMoveRequests(moves);
	int current_turn = GetTurnNumber();
	EndTurn();
	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	std::stringstream ss;
	ss << "Turn " << current_turn << ": " << elapsed_seconds.count() << " sec";
	logger << ige::FileLogger::e_logType::LOG_INFO;
	logger << ss.str();
}

int Controller::GetTurnNumber() const
{
	return turn_number;
}

GameState Controller::GetGameState() const
{
	return state;
}

void Controller::WaitForGameStart()
{
	while (state != GameState::RUN) {
		UpdateGameState();
	}
}

void Controller::UpdateGame()
{
	connection.send(ActionMessage{ Action::MAP, JsonWriter::WriteMapLayer(1) });
	auto map_layer1_response = connection.recieve();
	if (map_layer1_response.result != Result::OKEY) {
		CheckResponse(map_layer1_response);
	}
	auto& idx_to_post = game.GetPosts();
	auto& trains = game.GetPlayer().trains;
	auto [new_posts, new_trains, rating] = JsonParser::ParseMapLayer1(map_layer1_response.data);
	{
		auto guard = std::lock_guard{ game_mutex };
		idx_to_post = move(new_posts);
		for (auto& [idx, train] : trains) {
			train = new_trains[idx];
		}
		auto& player = game.GetPlayer();
		player.home_town = *std::dynamic_pointer_cast<Town>(idx_to_post[player.home.idx]);
		player.rating = rating;
	}
}

void Controller::UpdateGameState()
{
	connection.send(ActionMessage{ Action::GAMES,"" });
	auto recv = connection.recieve();
	state = JsonParser::ParseGameState(recv.data, login.game.value());

	switch (state)
	{
	case GameState::INIT:
		logger << ige::FileLogger::e_logType::LOG_INFO
			<< "Waiting for the game's start";
		break;
	case GameState::RUN:
		logger << ige::FileLogger::e_logType::LOG_INFO
			<< "Game started";
		break;
	case GameState::FINISHED:
		logger << ige::FileLogger::e_logType::LOG_INFO
			<< "Game finished";
		break;
	}
}

void Controller::CheckTrainCrashed()
{
	for (const auto& [idx, train] : game.GetPlayer().trains) {
		if (train.crashed) {
			route_manager.TrainCrashed(idx, train.cooldown);
			std::stringstream ss;
			ss << "Train " << idx << " crashed";
			logger << ige::FileLogger::e_logType::LOG_INFO << ss.str();
		}
	}
}

void Controller::SendMoveRequests(const std::vector<MoveRequest>& moves)
{
	for (const auto& request : moves) {
		connection.send(ActionMessage{ Action::MOVE, JsonWriter::WriteMove(request) });
		auto msg = connection.recieve();
		if (msg.result != Result::OKEY) {
			CheckResponse(msg);
		}
	}
}

void Controller::SendUpgradeRequest(std::vector<int> town_upgrades, std::vector<int> train_upgrades)
{
	connection.send(ActionMessage{ Action::UPGRADE,
		JsonWriter::WriteUpgrade(town_upgrades, train_upgrades) });
	auto msg = connection.recieve();
	if (msg.result != Result::OKEY) {
		CheckResponse(msg);
	}
}

void Controller::EndTurn()
{
	connection.send(ActionMessage{ Action::TURN, "" });
	auto msg = connection.recieve();
	if (msg.result != Result::OKEY) {
		CheckResponse(msg);
	}
	++turn_number;
}

void Controller::CheckUpgrades()
{
	std::vector<int> train_upgrades, town_upgrade;
	int current_armor = game.GetPlayer().home_town.armor;
	for (const auto& [train_idx, train] : game.GetPlayer().trains) {
		if (train.next_level_price.has_value() && current_armor > train.next_level_price.value() &&
			IsTrainAtHome(train_idx)) {
			train_upgrades.push_back(train_idx);
			current_armor -= train.next_level_price.value();
			route_manager.UpgradeTrain(train_idx, train.goods_capacity * 2);
		}
	}
	if (game.GetPlayer().home_town.next_level_price.has_value() &&
		current_armor > game.GetPlayer().home_town.next_level_price.value() + UPGRADE_COEFF) {
		town_upgrade.push_back(game.GetPlayer().home.post_idx);
		current_armor -= game.GetPlayer().home_town.next_level_price.value();
		route_manager.UpgradeHome();
	}

	if (town_upgrade.size() + train_upgrades.size() != 0) {
		SendUpgradeRequest(town_upgrade, train_upgrades);
	}
}

void Controller::CheckResponse(const ResposeMessage& msg)
{
	if (msg.result == Result::INAPPROPRIATE_GAME_STATE) {
		UpdateGameState();
	}

	if (state != GameState::FINISHED) {
		LogErrorRecieve(msg);
	}
}

bool Controller::IsTrainAtHome(int idx)
{
	const auto& train = game.GetPlayer().trains[idx];
	const auto& edge = game.GetGraph().GetIndices().at(train.line_idx);
	if (train.position != 0 && train.position != edge->length) {
		return false;
	}

	int vertex_idx = 0;
	if ((train.position == 0 && !edge->is_reversed) ||
		(train.position != 0 && edge->is_reversed)) {
		vertex_idx = edge->from;
	}
	else {
		vertex_idx = edge->to;
	}
	return vertex_idx == game.GetPlayer().home.idx;
}

void Controller::LogErrorRecieve(const ResposeMessage& response)
{
	std::stringstream ss;
	ss << "Error code: " << static_cast<int>(response.result) << '\n'
		<< "Error message: " << response.data;
	logger << ige::FileLogger::e_logType::LOG_ERROR;
	logger << ss.str();
}