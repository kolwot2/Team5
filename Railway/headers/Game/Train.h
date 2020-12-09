#pragma once
#include <vector>
#include <string>
#include "Event.h"
#include "Goods.h"

struct Train {
	int cooldown, fuel, fuel_capacity, fuel_consumption, goods, goods_capacity,
		idx, level, line_idx, next_level_price, position, speed;
	std::vector<Event> events;
	std::string player_idx;
	GoodsType goods_type;
};