#pragma once
#include "../Game/Graph.h"
#include "../Game/Train.h"
#include "../Game/Player.h"
#include "../Game/Game.h"
#include <deque>
#include <set>

struct TrainPosition {
	int line_idx;
	int position;
};

struct RouteNode {
	int line_idx;
	int turns_left;
	int speed;
};

using Route = std::deque<RouteNode>;

struct RouteInfo {
	int destination;
	int waiting_for;
	Route route_nodes;
	TrainPosition train_position;
};

class RouteManager
{
public:
	RouteManager(const Game&);
	std::vector<MoveRequest> MakeMoves(const Game&);
private:
	void CreateRoute(int, int, const PostMap&);
	int CalculateDestination(int, int, const PostMap&);
	bool IsDestinated(int);

	Graph market_graph, storage_graph;
	const std::unordered_map<int, std::shared_ptr<Edge>>& idx_to_edge;
	std::unordered_map<int, RouteInfo> train_to_route;
	std::unordered_map<int, std::unordered_map<int, int>> indices_to_distances;
};

