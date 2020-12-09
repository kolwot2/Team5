#include "Player.h"
#include <utility>

using namespace std;

Player::Player(Home home, string idx, bool in_Player, string name, int rating, Town home_town, vector<Train> trains) :
	home(home), idx(move(idx)), in_Player(in_Player), name(move(name)),
	rating(rating), home_town(move(home_town)), trains(move(trains))
{
}
