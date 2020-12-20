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

const Home& Player::GetHome() const
{
	return home;
}

const std::string& Player::GetIdx() const
{
	return idx;
}

const bool Player::IsInGame() const
{
	return in_game;
}

const std::string Player::GetName() const
{
	return name;
}

const int& Player::GetRating() const
{
	return rating;
}

const Town& Player::GetHomeTown() const
{
	return home_town;
}

const std::vector<Train>& Player::GetTrains() const
{
	return trains;
}
