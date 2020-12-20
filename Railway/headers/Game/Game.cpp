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

std::string Game::GetPostInfo(const int& post_index) const {
	std::string post_info = "";
	if (idx_to_post.find(post_index) != idx_to_post.end()) {
		const Post& post = *idx_to_post.at(post_index);
		if (post.type == PostType::TOWN) {
			auto town = std::dynamic_pointer_cast<Town>(idx_to_post.at(post_index));
			post_info += "Name: " + town->name + "\n";
			post_info += "Point Index: " + std::to_string(town->point_idx) + "\n";
			post_info += "Type: Town\n";
			post_info += "Level: " + std::to_string(town->level) + "\n";
			post_info += "Population: " + std::to_string(town->population) + "\n";
			post_info += "Population capacity: " + std::to_string(town->population_capacity) + "\n";
			post_info += "Armor: " + std::to_string(town->armor) + "\n";
			post_info += "Armor capacity: " + std::to_string(town->armor_capacity) + "\n";
			post_info += "Product: " + std::to_string(town->product) + "\n";
			post_info += "Product capacity: " + std::to_string(town->product_capacity) + "\n";
		}
		if (post.type == PostType::MARKET) {
			auto market = std::dynamic_pointer_cast<Market>(idx_to_post.at(post_index));
			post_info += "Name: " + market->name + "\n";
			post_info += "Point Index: " + std::to_string(market->point_idx) + "\n";
			post_info += "Type: Market\n";
			post_info += "Product: " + std::to_string(market->product) + "\n";
			post_info += "Product capacity: " + std::to_string(market->product_capacity) + "\n";
		}
		if (post.type == PostType::STORAGE) {
			auto storage = std::dynamic_pointer_cast<Storage>(idx_to_post.at(post_index));
			post_info += "Name: " + storage->name + "\n";
			post_info += "Point Index: " + std::to_string(storage->point_idx) + "\n";
			post_info += "Type: Storage\n";
			post_info += "Armor: " + std::to_string(storage->armor) + "\n";
			post_info += "Armor capacity: " + std::to_string(storage->armor_capacity) + "\n";
		}
	}
	else {
		if (post_index != -1) {
			post_info += "Point Index: " + std::to_string(post_index) + "\n";
		}
	}
	return post_info;
}