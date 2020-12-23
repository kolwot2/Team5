#include "Controller.h"
#include "../JsonUtils/JsonParser.h"
#include "../JsonUtils/JsonWriter.h"
#include <chrono>
#include <memory>

Controller::Controller()
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
	std::cout << map_layer1_response.data << std::endl;
	auto& idx_to_post = game.GetPosts();
	idx_to_post = JsonParser::ParsePosts(map_layer1_response.data);

	connection.send(ActionMessage{ Action::MAP, JsonWriter::WriteMapLayer(10) });
	auto map_layer10_response = connection.recieve();
	auto coords = JsonParser::ParseCoordinates(map_layer10_response.data);
	for (const auto& i : coords) {
		graph.SetVertexCoordinates(i.first, i.second);
	}
	route_manager = std::make_unique<RouteManager>(game);

	connection.send(ActionMessage{ Action::UPGRADE, JsonWriter::WriteUpgrade({}, {1}) });
	auto msg = connection.recieve();
}

Controller::~Controller()
{
	connection.send(ActionMessage(Action::LOGOUT, ""));
}

const Game& Controller::GetGame()
{
	return game;
}

void Controller::MakeTurn()
{
	//auto start = std::chrono::system_clock::now();

	UpdateGame();
	auto moves = route_manager->MakeMoves(game);
	SendMoveRequests(moves);
	std::cout << i++ << std::endl;
	EndTurn();

	/*auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	std::cout << elapsed.count() << std::endl;*/
}

void Controller::UpdateGame()
{
	connection.send(ActionMessage{ Action::PLAYER,"" });
	auto player_response = connection.recieve();
	auto& player = game.GetPlayer();
	player = JsonParser::ParsePlayer(player_response.data);

	connection.send(ActionMessage{ Action::MAP, JsonWriter::WriteMapLayer(1) });
	auto map_layer1_response = connection.recieve();
	auto& idx_to_post = game.GetPosts();
	idx_to_post = JsonParser::ParsePosts(map_layer1_response.data);
}

void Controller::SendMoveRequests(const std::vector<MoveRequest>& moves)
{
	for (const auto& request : moves) {
		connection.send(ActionMessage{ Action::MOVE, JsonWriter::WriteMove(request) });
		auto msg = connection.recieve();
	}
}

void Controller::EndTurn()
{
	connection.send(ActionMessage{ Action::TURN, "" });
	auto msg = connection.recieve();
}
