#include "Upgrade.h"

int UpradePrice(UpgradeType type, int level)
{
	if (type == UpgradeType::TRAIN) {
		switch (level)
		{
		case 2:
			return 40;
		case 3:
			return 80;
		default:
			return -1;
		}
	}
	else {
		switch (level)
		{
		case 2:
			return 100;
		case 3:
			return 200;
		default:
			return -1;
		}
	}
}
