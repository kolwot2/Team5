#pragma once

#include <vector>
#include <string>
#include "Train.h"
#include "Posts.h"

struct Home {
	int idx, post_idx;
};

class Player
{
public:
	Player() = default;
	Player(Home, std::string, bool, std::string, int, Town, std::vector<Train>);
	Home& GetHome();
	std::string& GetIdx();
	bool IsInGame();
	std::string GetName();
	int& GetRating();
	Town& GetHomeTown();
	std::vector<Train>& GetTrains();
	const Home& GetHome() const;
	const std::string& GetIdx() const;
	const bool IsInGame() const;
	const std::string GetName() const;
	const int& GetRating() const;
	const Town& GetHomeTown() const;
	const std::vector<Train>& GetTrains() const;
private:
	Home home;
	std::string idx;
	bool in_game;
	std::string name;
	int rating;
	Town home_town;
	std::vector<Train> trains;
};

