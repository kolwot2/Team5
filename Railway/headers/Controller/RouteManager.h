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
	int waiting_for;
	int train_capacity;
	Route route_nodes;
	Position train_position;
};

class RouteManager
{
public:
	RouteManager(const Game&);
	std::vector<MoveRequest> MakeMoves(const Game&);
	void UpgradeTrain(int, int);
	void SetNeededRecourse(GoodsType recourse_type);
private:
	void CreateRoute(int, int, const PostMap&);
	int CalculateDestination(int, int, int, const PostMap&);
	bool IsDestinated(int);

	Graph market_graph, storage_graph;
	const std::unordered_map<int, std::shared_ptr<Edge>>& idx_to_edge;
	std::unordered_map<int, RouteInfo> train_to_route;
	std::unordered_map<int, std::unordered_map<int, int>> indices_to_distances;
	GoodsType needed_recourse = GoodsType::PRODUCT;
};

