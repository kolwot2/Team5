#include "Controller.h"
#include "../JsonUtils/JsonParser.h"
#include "../JsonUtils/JsonWriter.h"
#include <chrono>

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
	auto& idx_to_post = game.GetPosts();
	idx_to_post = JsonParser::ParsePosts(map_layer1_response.data);

	connection.send(ActionMessage{ Action::MAP, JsonWriter::WriteMapLayer(10) });
	auto map_layer10_response = connection.recieve();
	auto coords = JsonParser::ParseCoordinates(map_layer10_response.data);
	for (const auto& i : coords) {
		graph.SetVertexCoordinates(i.first, i.second);
	}
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
	auto start = std::chrono::system_clock::now();

	UpdateGame();
	const auto& trains = game.GetPlayer().GetTrains();


	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	std::cout << elapsed.count() << std::endl;
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
