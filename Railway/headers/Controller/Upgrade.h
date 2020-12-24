#pragma once
#include <unordered_map>
#include <vector>

enum class UpgradeType {
	TOWN,
	TRAIN
};

struct Upgrade {
	UpgradeType type;
	int idx;
	int level;
	const static int MAX_LEVEL = 3;

	Upgrade(UpgradeType type, int idx, int level)
		:type(type), idx(idx), level(level) {}
};

const static std::unordered_map<UpgradeType, std::vector<int>> UPGRADES_PRICES{
	{ UpgradeType::TRAIN , { 0 , 0 , 40 , 80 } },
	{ UpgradeType::TOWN , { 0 , 0 , 100 , 200 } }
};