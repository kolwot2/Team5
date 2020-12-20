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
	auto& idx_to_post = game.GetPosts();
	idx_to_post = JsonParser::ParsePosts(map_layer1_response.data);

	connection.send(ActionMessage{ Action::MAP, JsonWriter::WriteMapLayer(10) });
	auto map_layer10_response = connection.recieve();
	auto coords = JsonParser::ParseCoordinates(map_layer10_response.data);
	for (const auto& i : coords) {
		graph.SetVertexCoordinates(i.first, i.second);
	}
	moves.reserve(game.GetPlayer().GetTrains().size());
	indices_to_distances = game.GetGraph().FloydWarshall();
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
	MoveTrains();
	SendMoveRequests();
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

void Controller::MoveTrains()
{
	for (const auto& train : game.GetPlayer().GetTrains()) {
		int vertex_idx = game.GetGraph().GetVertexFromPosition(train.line_idx, train.position);
		if (moves[train.idx].empty()) {
			auto priority = CalculatePriority(vertex_idx);
			MakeMoveRequests(train.idx, vertex_idx, priority.begin()->second);
			MakeMoveRequests(train.idx, priority.begin()->second, vertex_idx);
		}
	}
}

void Controller::MakeMoveRequests(int train_idx, int start, int end)
{
	const auto& edges = game.GetGraph().GetEdges();
	auto way = game.GetGraph().Dijkstra(start, end);
	for (int from = start, to, i = 0; i < way.size(); from = to, ++i) {
		to = way[i];
		auto& edge = edges.at(from).at(to);
		int speed = edge->is_reversed ? -1 : 1;
		moves[train_idx].push({ edge->index, speed, train_idx });
	}
}

void Controller::SendMoveRequests()
{
	const auto& edges = game.GetGraph().GetEdges();
	std::unordered_map<int, Train> idx_to_train;
	for (const auto& train : game.GetPlayer().GetTrains()) {
		idx_to_train[train.idx] = train;
	}
	std::unordered_map<int, int> idx_to_length;
	for (const auto& [i,m] : game.GetGraph().GetEdges()) {
		for (const auto& [j, edge] : m) {
			idx_to_length[edge->index] = edge->length;
		}
	}
	for (auto& [idx, request_queue] : moves) {
		if (idx_to_train[idx].position == idx_to_length[idx_to_train[idx].line_idx]
			|| idx_to_train[idx].position == 0) {
			auto request = request_queue.front();
			request_queue.pop();
			connection.send(ActionMessage{ Action::MOVE, JsonWriter::WriteMove(request) });
			auto msg = connection.recieve();
		}
		break;
	}
}

void Controller::EndTurn()
{
	connection.send(ActionMessage{ Action::TURN, "" });
	auto msg = connection.recieve();
}

std::set<std::pair<double, int>> Controller::CalculatePriority(int start_idx)
{
	std::set<std::pair<double, int>> priority;
	for (const auto& [idx, post] : game.GetPosts()) {
		if (post->type == PostType::MARKET) {
			auto market = std::dynamic_pointer_cast<Market>(post);
			priority.insert({ static_cast<double>(market->product) /
				indices_to_distances[start_idx][idx], idx });
		}
	}
	return priority;
}
