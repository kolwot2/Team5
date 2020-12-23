#include "RouteManager.h"
#include <iostream>

RouteManager::RouteManager(const Game& game)
	:idx_to_edge(game.GetGraph().GetIndices()),
	indices_to_distances(game.GetGraph().FloydWarshall())
{
	for (const auto& train : game.GetPlayer().trains) {
		train_to_route[train.idx].train_position.line_idx = train.line_idx;
		train_to_route[train.idx].train_position.position = train.position;
	}

	const auto& graph = game.GetGraph();
	const auto& vertexes = graph.GetVertexes();
	const auto& edges = graph.GetEdges();

	for (const auto& [idx, vertex] : vertexes) {
		switch (game.GetPostType(idx))
		{
		case PostType::MARKET:
			market_graph.AddVertex(vertex);
			break;
		case PostType::STORAGE:
			storage_graph.AddVertex(vertex);
			break;
		default:
			market_graph.AddVertex(vertex);
			storage_graph.AddVertex(vertex);
			break;
		}
	}

	for (const auto& [from, edge_map] : edges) {
		for (const auto& [to, edge] : edge_map) {
			if (game.GetPostType(from) == PostType::MARKET ||
				game.GetPostType(to) == PostType::MARKET) {
				market_graph.AddEdge(edge);
			}
			else if (game.GetPostType(from) == PostType::STORAGE ||
				game.GetPostType(to) == PostType::STORAGE) {
				storage_graph.AddEdge(edge);
			}
			else {
				market_graph.AddEdge(edge);
				storage_graph.AddEdge(edge);
			}
		}
	}

}

std::vector<MoveRequest> RouteManager::MakeMoves(const Game& game)
{
	std::vector<MoveRequest> moves;
	for (auto& [train_idx, route] : train_to_route) {
		if (!route.route_nodes.empty() && route.route_nodes.front().turns_left == 0) {
			route.route_nodes.pop_front();
			if (!route.route_nodes.empty()) {
				UpdatePosition(train_idx, route.route_nodes.front());
			}
			else {
				route.destination = 0;
			}
		}
		if (route.route_nodes.empty()) {
			const auto& home_town = game.GetPlayer().home_town;
			CreateRoute(train_idx, home_town.product / (home_town.population * 2), game.GetPosts());
		}

		if (route.waiting_for == 0) {
			--route.route_nodes.front().turns_left;
			route.train_position.position += route.route_nodes.front().speed;
			if (!route.route_nodes.front().is_sended) {
				moves.emplace_back(
					route.route_nodes.front().line_idx, route.route_nodes.front().speed, train_idx
				);
				route.route_nodes.front().is_sended = true;
			}
		}
		else {
			--route.waiting_for;
		}
		break;
	}
	return moves;
}

void RouteManager::CreateRoute(int train_idx, int max_turns, const PostMap& idx_to_post)
{
	TrainPosition& train_position = train_to_route[train_idx].train_position;
	auto edge = idx_to_edge.at(train_position.line_idx);
	int start_idx;
	if ((!edge->is_reversed && train_position.position == 0)
		|| (edge->is_reversed && train_position.position != 0)) {
		start_idx = edge->from;
	}
	else {
		start_idx = edge->to;
	}

	auto dest = CalculateDestination(start_idx, max_turns, 80, idx_to_post);
	train_to_route[train_idx].destination = dest;
	auto way = market_graph.Dijkstra(start_idx, dest);
	const auto edges = market_graph.GetEdges();

	for (int i = 0, from = start_idx, to; i < way.size(); ++i, from = to) {
		to = way[i];
		int edge_idx = edges.at(from).at(to)->index;
		int speed = edges.at(from).at(to)->is_reversed ? -1 : 1;
		auto node = RouteNode{ edge_idx , idx_to_edge.at(edge_idx)->length, speed, false };
		train_to_route[train_idx].route_nodes.push_back(node);
	}

	auto& route = train_to_route[train_idx].route_nodes;
	for (size_t i = route.size() - 1; i != SIZE_MAX; --i) {
		auto reversed_node = RouteNode{
			route[i].line_idx, route[i].turns_left, -route[i].speed, false
		};
		route.push_back(reversed_node);
	}
	UpdatePosition(train_idx, route.front());
}

int RouteManager::CalculateDestination(int start_idx, int max_turs, int capacity, const PostMap& idx_to_post)
{
	int max_idx = -1, length = INT_MAX, max_product = 0;

	for (const auto& [cur_idx, post] : idx_to_post) {
		if (post->type == PostType::MARKET && !IsDestinated(cur_idx) &&
			indices_to_distances[start_idx][cur_idx] * 2 < max_turs) {
			auto market = std::dynamic_pointer_cast<Market>(post);
			int current_product = std::min(capacity, market->product);
			if (current_product > max_product || (current_product == max_product &&
				indices_to_distances[start_idx][cur_idx] < length)) {
				max_idx = cur_idx;
				length = indices_to_distances[start_idx][cur_idx];
				max_product = current_product;
			}
		}
	}
	if (max_idx == -1) {
		return CalculateDestination(start_idx, max_turs * 2, capacity, idx_to_post);
	}

	return max_idx;
}

bool RouteManager::IsDestinated(int idx)
{
	for (const auto& [train, route_info] : train_to_route) {
		if (idx == route_info.destination) {
			return true;
		}
	}
	return false;
}

void RouteManager::UpdatePosition(int train_idx, const RouteNode& node)
{
	train_to_route[train_idx].train_position.line_idx = node.line_idx;
	train_to_route[train_idx].train_position.position =
		node.speed == 1 ? 0 : idx_to_edge.at(node.line_idx)->length;
}
