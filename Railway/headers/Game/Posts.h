#pragma once
#include <string>
#include <vector>

enum class PostType {
	TOWN = 1,
	MARKET = 2,
	STORAGE = 3,
	DEFAULT = 4
};

struct Post {
	PostType type;
	std::string name;
	int idx, point_idx;
	virtual int GetRecourse() const { return 0; }
	virtual ~Post() = default;
};

struct Town : public Post {
	int armor, armor_capacity, level, next_level_price,
		population, population_capacity, product, product_capacity, train_cooldown;
	std::string player_idx;
};

struct Market : public Post {
	int product, product_capacity, replenishment;
	int GetRecourse() const override { return product; }
};

struct Storage : public Post {
	int armor, armor_capacity, replenishment;
	int GetRecourse() const override { return armor; }
};