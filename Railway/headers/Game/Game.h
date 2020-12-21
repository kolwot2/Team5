#pragma once
#include <unordered_map>
#include "Graph.h"
#include "Posts.h"
#include "Player.h"
#include "../ServerConnection/ServerConnection.h"
#include "../ServerConnection/Login.h"

using PostMap = std::unordered_map<int, std::shared_ptr<Post>>;

class Game
{
public:
	Game() = default;
	Graph& GetGraph();
	PostMap& GetPosts();
	Player& GetPlayer();
	const Graph& GetGraph() const;
	const PostMap& GetPosts() const;
	const Player& GetPlayer() const;
	std::string GetPostInfo(const int &post_index) const;
	PostType GetPostType(int) const;
private:
	Graph graph;
	PostMap idx_to_post;
	Player player;
};


