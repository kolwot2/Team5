#pragma once
#include <unordered_map>
#include "Graph.h"
#include "Posts.h"
#include "Player.h"
#include "../ServerConnection/ServerConnection.h"
#include "../ServerConnection/Login.h"

class Game
{
public:
	Game();
	~Game();
	const Graph& GetGraph();
	const std::unordered_map<int, std::shared_ptr<Post>> &getPosts();
private:
	Graph graph;
	std::unordered_map<int, std::shared_ptr<Post>> idx_to_post;
	Player player;
	ServerConnection connection;
};

