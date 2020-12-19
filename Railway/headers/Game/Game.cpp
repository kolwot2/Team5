#include "Game.h"
#include "../JsonUtils/JsonParser.h"
#include "../JsonUtils/JsonWriter.h"

using PostMap = std::unordered_map<int, std::shared_ptr<Post>>;

Graph& Game::GetGraph()
{
	return graph;
}

PostMap& Game::GetPosts()
{
	return idx_to_post;
}

Player& Game::GetPlayer()
{
	return player;
}

const Graph& Game::GetGraph() const
{
	return graph;
}

const PostMap& Game::GetPosts() const
{
	return idx_to_post;
}

const Player& Game::GetPlayer() const
{
	return player;
}
