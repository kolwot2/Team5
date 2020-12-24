#include "RouteManager.h"
#include <iostream>

void RouteManager::Init(const Game& game)
{
	const auto& graph = game.GetGraph();
	const auto& player = game.GetPlayer();
	const auto& vertexes = graph.GetVertexes();
	const auto& edges = graph.GetEdges();

	for (const auto& train : player.trains) {
		train_to_route[train.idx].train_position = vertexes.at(player.home.idx);
		train_to_route[train.idx].destination = 0;
		train_to_route[train.idx].train_capacity = 80;
	}

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

	indices_to_distances = game.GetGraph().FloydWarshall();
}

std::vector<MoveRequest> RouteManager::MakeMoves(const Game& game)
{
	std::vector<MoveRequest> moves;
	const auto& vertexes = game.GetGraph().GetVertexes();
	for (auto& [train_idx, route] : train_to_route) {
		if (!route.route_nodes.empty() && route.route_nodes.front().turns_left == 0) {
			route.train_position = vertexes.at(route.route_nodes.front().to);
			route.destination = 0;
			route.route_nodes.pop_front();
		}

		if (route.route_nodes.empty()) {
			const auto& home_town = game.GetPlayer().home_town;
			int max_turns = home_town.product == 0 ? 
				INT_MAX : home_town.product / ((home_town.population + 5));
			CreateRoute(train_idx, max_turns, game.GetPosts());
		}

		if (route.waiting_for == 0) {
			auto& current_node = route.route_nodes.front();
			--current_node.turns_left;
			if (!current_node.is_sended) {
				const auto& edge = game.GetGraph().GetEdges().at(current_node.from).at(current_node.to);
				int speed = edge->is_reversed ? -1 : 1;
				route.train_position = edge;
				moves.emplace_back(edge->index, speed, train_idx);
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

void RouteManager::UpgradeTrain(int train_idx, int new_capacity)
{
	train_to_route[train_idx].train_capacity = new_capacity;
}

void RouteManager::CreateRoute(int train_idx, int max_turns, const PostMap& idx_to_post)
{
	int start_idx = std::get<Vertex>(train_to_route[train_idx].train_position).index;
	auto dest = CalculateDestination(start_idx, max_turns,
		train_to_route[train_idx].train_capacity, idx_to_post);
	train_to_route[train_idx].destination = dest;

	const auto& needed_graph = needed_recourse == GoodsType::PRODUCT ?
		market_graph : storage_graph;
	auto way = needed_graph.Dijkstra(start_idx, dest);
	const auto edges = needed_graph.GetEdges();

	int from = start_idx;
	for (size_t i = 0; i < way.size(); from = way[i++]) {
		int to = way[i];
		train_to_route[train_idx].route_nodes.push_back(
			RouteNode{ from, to, edges.at(from).at(to)->length, false }
		);
	}

	for (size_t i = way.size() - 1; i + 1 > 0; --i) {
		const auto& current = train_to_route[train_idx].route_nodes[i];
		train_to_route[train_idx].route_nodes.push_back(
			RouteNode{ current.to, current.from, current.turns_left, false }
		);
	}
}

int RouteManager::CalculateDestination(int start_idx, int max_turs, int capacity, const PostMap& idx_to_post)
{
	int max_idx = -1, length = INT_MAX, max_recourse = 0;
	PostType needed_post_type = needed_recourse == GoodsType::PRODUCT ?
		PostType::MARKET : PostType::STORAGE;

	for (const auto& [cur_idx, post] : idx_to_post) {
		if (post->type == needed_post_type && !IsDestinated(cur_idx) &&
			indices_to_distances[start_idx][cur_idx] * 2 < max_turs) {
			int current_recourse = std::min(capacity, post->GetRecourse());
			if (current_recourse > max_recourse || (current_recourse == max_recourse &&
				indices_to_distances[start_idx][cur_idx] < length)) {
				max_idx = cur_idx;
				length = indices_to_distances[start_idx][cur_idx];
				max_recourse = current_recourse;
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

void RouteManager::SetNeededRecourse(GoodsType recourse_type)
{
	needed_recourse = recourse_type;
}
