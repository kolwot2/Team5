#pragma once
#include <vector>
#include <string>
#include "Goods.h"
#include <optional>

struct Train {
	int cooldown, goods, goods_capacity,
		idx, level, line_idx, position, speed;
	std::optional<int> next_level_price;
	std::string player_idx;
	GoodsType goods_type;
	bool crashed = false;
};