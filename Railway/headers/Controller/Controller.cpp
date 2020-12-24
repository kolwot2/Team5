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
	connection.send(ActionMessage{ Login{} });
	auto login_response = connection.recieve();
	auto& player = game.GetPlayer();
	player = JsonParser::ParsePlayer(login_response.data);

	connection.send(ActionMessage{ Action::MAP, JsonWriter::WriteMapLayer(0) });
	auto map_layer0_response = connection.recieve();
	auto& graph = game.GetGraph();
	graph = JsonParser::ParseGraph(map_layer0_response.data);

	connection.send(ActionMessage{ Action::MAP, JsonWriter::WriteMapLayer(1) });
	auto map_layer1_response = connection.recieve();
	auto& idx_to_post = game.GetPosts();
	idx_to_post = JsonParser::ParsePosts(map_layer1_response.data);

	connection.send(ActionMessage{ Action::MAP, JsonWriter::WriteMapLayer(10) });
	auto map_layer10_response = connection.recieve();
	auto coords = JsonParser::ParseCoordinates(map_layer10_response.data);
	for (const auto& i : coords) {
		graph.SetVertexCoordinates(i.first, i.second);
	}

	route_manager.Init(game);

	const auto& home = player.home_town;
	const auto& trains = player.trains;
	for (size_t i = 2; i <= Upgrade::MAX_LEVEL; ++i) {
		for (const auto& train : trains) {
			upgrade_queue.emplace(UpgradeType::TRAIN, train.idx, i);
			break;
		}
		upgrade_queue.emplace(UpgradeType::TOWN, home.idx, i);
	}
}

void Controller::Disconnect()
{
	connection.send(ActionMessage(Action::LOGOUT, ""));
	auto msg = connection.recieve();
}

const Game& Controller::GetGame()
{
	return game;
}

void Controller::MakeTurn()
{
	auto start = std::chrono::steady_clock::now();
	UpdateGame();
	CheckNeededRecourse();
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

void Controller::UpdateGame()
{
	connection.send(ActionMessage{ Action::PLAYER,"" });
	auto player_response = connection.recieve();
	if (player_response.result != Result::OKEY) {
		LogErrorRecieve(player_response, "PLAYER RESPONSE");
	}
	auto& player = game.GetPlayer();
	player = JsonParser::ParsePlayer(player_response.data);

	connection.send(ActionMessage{ Action::MAP, JsonWriter::WriteMapLayer(1) });
	auto map_layer1_response = connection.recieve();
	if (map_layer1_response.result != Result::OKEY) {
		LogErrorRecieve(map_layer1_response, "MAP1 RESPONSE");
	}
	auto& idx_to_post = game.GetPosts();
	idx_to_post = JsonParser::ParsePosts(map_layer1_response.data);
}

void Controller::SendMoveRequests(const std::vector<MoveRequest>& moves)
{
	for (const auto& request : moves) {
		connection.send(ActionMessage{ Action::MOVE, JsonWriter::WriteMove(request) });
		auto msg = connection.recieve();
		if (msg.result != Result::OKEY) {
			LogErrorRecieve(msg, "MOVE RESPONSE");
		}
	}
}

void Controller::SendUpgradeRequest(Upgrade upgrade)
{
	if (upgrade.type == UpgradeType::TOWN) {
		connection.send(ActionMessage{ Action::UPGRADE, JsonWriter::WriteUpgrade({upgrade.idx},{}) });
	}
	else {
		connection.send(ActionMessage{ Action::UPGRADE, JsonWriter::WriteUpgrade({},{upgrade.idx}) });
	}
	auto msg = connection.recieve();
	if (msg.result != Result::OKEY) {
		LogErrorRecieve(msg, "UPGRADE RESPONSE");
	}
}

void Controller::EndTurn()
{
	connection.send(ActionMessage{ Action::TURN, "" });
	auto msg = connection.recieve();
	if (msg.result != Result::OKEY) {
		LogErrorRecieve(msg, "TURN RESPONSE");
	}
	++turn_number;
}

void Controller::CheckUpgrades()
{
	if (!upgrade_queue.empty())
	{
		const auto& upgrade = upgrade_queue.front();
		if (game.GetPlayer().home_town.armor - UPGRADE_COEFF
			>= UpradePrice(upgrade.type, upgrade.level)) {
			SendUpgradeRequest(upgrade);
			upgrade_queue.pop();
		}
	}
}

void Controller::CheckNeededRecourse()
{
	GoodsType needed_type = static_cast<double>(game.GetPlayer().home_town.product) >=
		game.GetPlayer().home_town.product_capacity * RECOURSE_COEFF ?
		GoodsType::ARMOR : GoodsType::PRODUCT;
	route_manager.SetNeededRecourse(needed_type);
}

void Controller::LogErrorRecieve(const ResposeMessage& response, const std::string& str)
{
	std::stringstream ss;
	ss << str << '\n' << "Error code: " << static_cast<int>(response.result) << '\n' 
		<< "Error message: " << response.data;
	logger << ige::FileLogger::e_logType::LOG_ERROR;
	logger << ss.str();
}
