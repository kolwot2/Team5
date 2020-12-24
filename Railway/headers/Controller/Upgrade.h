#pragma once

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

int UpradePrice(UpgradeType, int);