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
		if (route.cooldown != 0) {
			--route.cooldown;
			continue;
		}

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
			++route.position;
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

void RouteManager::TrainCrashed(int train_idx, int cooldown)
{
	if (train_to_route[train_idx].cooldown == 0) {
		train_to_route[train_idx].cooldown = cooldown;
		train_to_route[train_idx].position = home_idx;
		train_to_route[train_idx].route_nodes.clear();
		train_to_route[train_idx].waiting = 0;
		train_to_route[train_idx].waiting_for_recourse = 0;
		if (home_upgrader.has_value() && home_upgrader.value() == train_idx) {
			home_upgrader = std::nullopt;
		}
		if (auto it = std::find(synchronized_market_routes.begin(), synchronized_market_routes.end(), train_idx);
			it != synchronized_market_routes.end()) {
			synchronized_market_routes.erase(it, synchronized_market_routes.end());
		}
		if (auto it = std::find(synchronized_storage_routes.begin(), synchronized_storage_routes.end(), train_idx);
			it != synchronized_storage_routes.end()) {
			synchronized_storage_routes.erase(it);
		}
	}
}

void RouteManager::UpgradeHome()
{
	++home_level;
}

void RouteManager::CreateRoute(PostType type, Graph graph, const PostMap& posts)
{
	auto& route = type == PostType::MARKET ? market_route : storage_route;
	auto idx = type == PostType::MARKET ? market_idx : storage_idx;
	int& length = type == PostType::MARKET ? market_route_length : storage_route_length;

	length = 2 * indices_to_distances[home_idx][idx];
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

void RouteManager::InitRoute(int train_idx, const PostMap& posts)
{
	if (auto it = std::find(synchronized_storage_routes.begin(), synchronized_storage_routes.end(), train_idx);
		it != synchronized_storage_routes.end()) {
		synchronized_storage_routes.erase(it);
	}

	auto& route_info = train_to_route[train_idx];
	train_to_route[train_idx].position = 0;
	if (primary) {
		InitPrimaryRoutes(posts);
		primary = false;
	} else if (synchronized_market_routes.size() != 0 &&
		(!home_upgrader.has_value() || home_upgrader.value() == train_idx) && 
		home_level != MaxLevels::MAX_TOWN_LEVEL) {
		home_upgrader = train_idx;
		route_info.route_nodes = storage_route;
		route_info.destination = storage_idx;
		auto dest = std::dynamic_pointer_cast<Storage>(posts.at(storage_idx));
		int current_capacity = synchronized_storage_routes.size() == 0 ?
			route_info.train_capacity - dest->armor_capacity : route_info.train_capacity;
		route_info.waiting_for_recourse = (current_capacity + dest->replenishment - 1) /
			dest->replenishment + SYNCHRONIZE_COEFF;
		SynchronizeStorageRoute(train_idx);
	}
	else {
		if (home_upgrader.has_value() && home_upgrader.value() == train_idx) {
			home_upgrader = std::nullopt;
		}
		route_info.route_nodes = market_route;
		SynchronizeMarketRoutes(train_idx);
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

void RouteManager::SynchronizeMarketRoutes(int train_idx)
{
	if (std::find(synchronized_market_routes.begin(), synchronized_market_routes.end(), train_idx)
		== synchronized_market_routes.end()) {
		if (!synchronized_market_routes.empty()) {
			int last_synchronized = synchronized_market_routes.back();
			int route_interval = market_route_length / train_to_route.size();

			if (train_to_route[last_synchronized].waiting != 0) {
				train_to_route[train_idx].waiting =
					train_to_route[last_synchronized].waiting + route_interval;
			}
			else {
				int route_interval = (market_route_length + train_to_route.size() - 1) /
					train_to_route.size();
				int last_position = train_to_route[last_synchronized].position;
				int current_wait = last_position < route_interval + SYNCHRONIZE_COEFF ?
					route_interval - last_position
					: market_route_length - last_position + route_interval;
				train_to_route[train_idx].waiting = current_wait;
			}

		}
		synchronized_market_routes.push_back(train_idx);
	}
}

void RouteManager::SynchronizeStorageRoute(int train_idx)
{
	if (std::find(synchronized_storage_routes.begin(), synchronized_storage_routes.end(), train_idx)
		== synchronized_storage_routes.end()) {
		if (!synchronized_storage_routes.empty()) {
			auto& current_route = train_to_route[train_idx];
			const auto& last_synchronized = train_to_route[synchronized_storage_routes.back()];

			if (last_synchronized.waiting != 0) {
				current_route.waiting =	last_synchronized.waiting +
					last_synchronized.waiting_for_recourse + SYNCHRONIZE_COEFF;
			}
			else {
				current_route.waiting = std::max(last_synchronized.waiting_for_recourse -
					last_synchronized.position + SYNCHRONIZE_COEFF, 0);
			}

		}
		synchronized_storage_routes.push_back(train_idx);
	}
}

void RouteManager::InitPrimaryRoutes(const PostMap& posts)
{
	for (auto& [train_idx, route_info] : train_to_route) {
		route_info.route_nodes = storage_route;
		route_info.destination = storage_idx;
		auto dest = std::dynamic_pointer_cast<Storage>(posts.at(storage_idx));
		int current_capacity = synchronized_storage_routes.size() == 0 ?
			route_info.train_capacity - dest->armor_capacity : route_info.train_capacity;
		route_info.waiting_for_recourse = (current_capacity + dest->replenishment - 1) /
			dest->replenishment;
		SynchronizeStorageRoute(train_idx);
	}
}
