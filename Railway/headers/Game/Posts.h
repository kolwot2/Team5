#pragma once
#include <string>
#include <vector>
#include "Event.h"

enum class PostType {
	TOWN = 1,
	MARKET = 2,
	STORAGE = 3,
	DEFAULT = 4
};

struct Post {
	PostType type;
	std::vector<Event> events;
	std::string name;
	int idx, point_idx;
	virtual ~Post() = default;
};

struct Town : public Post {
	int armor, armor_capacity, level, next_level_price,
		population, population_capacity, product, product_capacity, train_cooldown;
	std::string player_idx;
};

struct Market : public Post {
	int product, product_capacity, replenishment;
};

struct Storage : public Post {
	int armor, armor_capacity, replenishment;
};