#pragma once
#include "../Game/Graph.h"
#include "../Game/Train.h"
#include "../Game/Player.h"
#include "../Game/Game.h"
#include <deque>
#include <set>
#include <variant>

struct RouteNode {
	int from;
	int to;
	int turns_left;
	bool is_sended;
};

using Position = std::variant<Vertex, std::shared_ptr<Edge>>;
using Route = std::deque<RouteNode>;

struct RouteInfo {
	int destination;
	int waiting;
	int waiting_for_recourse;
	int train_capacity;
	int level = 1;
	Route route_nodes;
	Position train_position;
};

class RouteManager
{
public:
	void Init(const Game&);
	std::vector<MoveRequest> MakeMoves(const Game&);
	void UpgradeTrain(int, int);
private:
	void CreateRoute(PostType, Graph, const PostMap&);
	int CalculateDestination(PostType, const PostMap&);
	void InitPrimaryRoutes(int, int);
	void InitRoute(int, const PostMap&);
	std::pair<Graph, Graph> GenerateGraphs(const Game&);

	int home_idx, market_idx, storage_idx;
	Route market_route, storage_route;
	std::unordered_map<int, RouteInfo> train_to_route;
	std::unordered_map<int, std::unordered_map<int, int>> indices_to_distances;
	bool primary = false;
	const int MAX_TRAIN_LEVEL = 3;
};