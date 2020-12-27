#pragma once

#include <unordered_map>
#include <string>
#include "Train.h"
#include "Posts.h"

struct Home {
	int idx, post_idx;
};

struct Player
{
	Home home;
	std::string idx;
	int rating;
	Town home_town;
	std::unordered_map<int, Train> trains;
};

