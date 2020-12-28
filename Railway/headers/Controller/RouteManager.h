#pragma once
#include "../Game/Graph.h"
#include "../Game/Train.h"
#include "../Game/Player.h"
#include "../Game/Game.h"
#include "MaxLevels.h"
#include <deque>
#include <set>
#include <variant>
#include <list>

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
	int waiting_for_recourse = 0;;
	int train_capacity;
	int level = 1;
	int position = 0;
	int cooldown = 0;;
	Route route_nodes;
	Position train_position;
};

class RouteManager
{
public:
	void Init(const Game&);
	std::vector<MoveRequest> MakeMoves(const Game&);
	void UpgradeTrain(int, int);
	void TrainCrashed(int,int);
	void UpgradeHome();
private:
	void CreateRoute(PostType, Graph, const PostMap&);
	int CalculateDestination(PostType, const PostMap&);
	void InitRoute(int, const PostMap&);
	std::pair<Graph, Graph> GenerateGraphs(const Game&);
	void SynchronizeMarketRoutes(int);
	void SynchronizeStorageRoute(int);
	void InitPrimaryRoutes(const PostMap&);

	int home_idx, market_idx, storage_idx, market_route_length, storage_route_length;
	std::optional<int> home_upgrader;
	std::list<int> synchronized_market_routes, synchronized_storage_routes;
	Route market_route, storage_route;
	std::unordered_map<int, RouteInfo> train_to_route;
	std::unordered_map<int, std::unordered_map<int, int>> indices_to_distances;
	bool primary = true;
	const int SYNCHRONIZE_COEFF = 2;
	int home_level = 1;
};