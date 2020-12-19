#include "Game.h"
#include "../JsonUtils/JsonParser.h"
#include "../JsonUtils/JsonWriter.h"

Game::Game()
{
	connection.send(ActionMessage{ Login{} });
	auto login_response = connection.recieve();
	player = JsonParser::ParsePlayer(login_response.data);

	connection.send(ActionMessage{ Action::MAP, JsonWriter::WriteMapLayer(0) });
	auto map_layer0_response = connection.recieve();
	graph = JsonParser::ParseGraph(map_layer0_response.data);

	connection.send(ActionMessage{ Action::MAP, JsonWriter::WriteMapLayer(1) });
	auto map_layer1_response = connection.recieve();
	idx_to_post = JsonParser::ParsePosts(map_layer1_response.data);

	connection.send(ActionMessage{ Action::MAP, JsonWriter::WriteMapLayer(10) });
	auto map_layer10_response = connection.recieve();
	auto coords = JsonParser::ParseCoordinates(map_layer10_response.data);
	for (const auto& i : coords) {
		graph.SetVertexCoordinates(i.first, i.second);
	}
}

Game::~Game()
{
	connection.send(ActionMessage(Action::LOGOUT, ""));
}

const Graph& Game::GetGraph()
{
	return graph;
}

const std::unordered_map<int, std::shared_ptr<Post>>& Game::getPosts()
{
	return idx_to_post;
}
