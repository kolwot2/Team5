#include "RouteManager.h"
#include <iostream>
#include <algorithm>

void RouteManager::Init(const Game& game)
{
	const auto& vertexes = game.GetGraph().GetVertexes();
	const auto& player = game.GetPlayer();
	home_idx = player.home.idx;

	for (const auto& [idx, train] : player.trains) {
		train_to_route[idx].train_position = vertexes.at(home_idx);
		train_to_route[idx].destination = 0;
		train_to_route[idx].train_capacity = train.goods_capacity;
	}

	auto [market_graph, storage_graph] = GenerateGraphs(game);
	
	indices_to_distances = game.GetGraph().FloydWarshall();

	market_idx = CalculateDestination(PostType::MARKET, game.GetPosts());
	storage_idx = CalculateDestination(PostType::STORAGE, game.GetPosts());

	CreateRoute(PostType::STORAGE, std::move(storage_graph), game.GetPosts());
	CreateRoute(PostType::MARKET, std::move(market_graph), game.GetPosts());
}

std::vector<MoveRequest> RouteManager::MakeMoves(const Game& game)
{
	std::vector<MoveRequest> moves;
	const auto& vertexes = game.GetGraph().GetVertexes();
	for (auto& [train_idx, route] : train_to_route) {
		if (!route.route_nodes.empty() && route.route_nodes.front().turns_left == 0) {
			route.train_position = vertexes.at(route.route_nodes.front().to);
			route.route_nodes.pop_front();
		}

		if (route.route_nodes.empty()) {
			InitRoute(train_idx, game.GetPosts());
		}

		if (std::holds_alternative<Vertex>(route.train_position) &&
			std::get<Vertex>(route.train_position).index == route.destination &&
			route.waiting_for_recourse != 0) {
			std::swap(route.waiting, route.waiting_for_recourse);
		}

		if (route.waiting == 0) {
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
			--route.waiting;
		}
	}
	return moves;
}

void RouteManager::UpgradeTrain(int train_idx, int new_capacity)
{
	train_to_route[train_idx].train_capacity = new_capacity;
	++train_to_route[train_idx].level;
}

void RouteManager::CreateRoute(PostType type, Graph graph, const PostMap& posts)
{
	auto& route = type == PostType::MARKET ? market_route : storage_route;
	auto idx = type == PostType::MARKET ? market_idx : storage_idx;

	auto way_to = graph.Dijkstra(home_idx, idx);
	auto& edges = graph.GetEdges();

	for (size_t i = 0; i < way_to.size() - 1; ++i) {
		int from = way_to[i];
		int to = way_to[i + 1];
		route.push_back(
			{ from, to, edges.at(from).at(to)->length, false }
		);
	}

	for (size_t i = 0; i < way_to.size() - 1; ++i) {
		int from = way_to[i];
		int to = way_to[i + 1];
		auto prev_edge = edges.at(from).at(to);
		graph.AddEdge(std::shared_ptr<Edge>(
			new Edge{ prev_edge->from, prev_edge->to,
			prev_edge->index, INT_MAX, prev_edge->is_reversed }
		));
	}
	auto way_from = graph.Dijkstra(idx, home_idx);

	for (size_t i = 0; i < way_to.size() - 1; ++i) {
		int from = way_from[i];
		int to = way_from[i + 1];
		route.push_back(
			{ from, to, edges.at(from).at(to)->length, false }
		);
	}
}

int RouteManager::CalculateDestination(PostType type, const PostMap& posts)
{
	int idx = 0, distance = INT_MAX;
	for (const auto& [post_idx, post] : posts) {
		if (post->type == type && indices_to_distances[home_idx][post_idx] < distance) {
			idx = post_idx;
			distance = indices_to_distances[home_idx][post_idx];
		}
	}
	return idx;
}

void RouteManager::InitPrimaryRoutes(int capacity, int replenishment)
{
	int wait_time = 0;
	for (auto& [idx, route] : train_to_route) {
		route.route_nodes = storage_route;
		route.waiting = wait_time;
		int current_capacity = wait_time == 0 ?
			route.train_capacity - capacity : route.train_capacity;
		route.waiting_for_recourse = current_capacity / replenishment;
		route.destination = storage_idx;
		wait_time += route.waiting_for_recourse + 3;
	}
}

void RouteManager::InitRoute(int train_idx, const PostMap& posts)
{
	if (!primary) {
		primary = true;
		auto primary = std::dynamic_pointer_cast<Storage>(posts.at(storage_idx));
		InitPrimaryRoutes(primary->armor_capacity, primary->replenishment);
	}
	else if (train_to_route[train_idx].level != MAX_TRAIN_LEVEL) {
		auto& route = train_to_route[train_idx];
		auto dest = std::dynamic_pointer_cast<Storage>(posts.at(storage_idx));
		route.waiting_for_recourse = (route.train_capacity -dest->armor_capacity)
			/ dest->replenishment;
		route.route_nodes = storage_route;
	}
	else {
		auto& route = train_to_route[train_idx];
		route.route_nodes = market_route;
	}
}

std::pair<Graph, Graph> RouteManager::GenerateGraphs(const Game& game)
{
	Graph market_graph, storage_graph;
	const auto& vertexes = game.GetGraph().GetVertexes();
	const auto& edges = game.GetGraph().GetEdges();
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
	return { std::move(market_graph), std::move(storage_graph) };
}