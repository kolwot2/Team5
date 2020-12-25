#include "Upgrade.h"

int UpradePrice(UpgradeType type, int level)
{
	if (type == UpgradeType::TRAIN) {
		switch (level)
		{
		case 2:
			return 30;
		case 3:
			return 60;
		default:
			return -1;
		}
	}
	else {
		switch (level)
		{
		case 2:
			return 75;
		case 3:
			return 150;
		default:
			return -1;
		}
	}
}
