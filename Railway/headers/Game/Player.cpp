#include "Player.h"
#include <utility>

using namespace std;

Player::Player(Home home, string idx, bool game, string name, int rating, Town home_town, vector<Train> trains) :
	home(home), idx(move(idx)), in_game(in_game), name(move(name)),
	rating(rating), home_town(move(home_town)), trains(move(trains))
{
}

Home& Player::GetHome()
{
	return home;
}

std::string& Player::GetIdx()
{
	return idx;
}

bool Player::IsInGame()
{
	return in_game;
}

std::string Player::GetName()
{
	return name;
}

int& Player::GetRating()
{
	return rating;
}

Town& Player::GetHomeTown()
{
	return home_town;
}

std::vector<Train>& Player::GetTrains()
{
	return trains;
}
