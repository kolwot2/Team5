#pragma once
#include "../Game/Game.h"
#include "../ServerConnection/ServerConnection.h"
#include "../ServerConnection/Messages.h"

class Controller
{
public:
	Controller();
	~Controller();
	const Game& GetGame();
	void MakeTurn();
private:
	void UpdateGame();


	Game game;
	ServerConnection connection;
	std::unordered_map<int, MoveRequest> moves;
	std::unordered_map<int, std::unordered_map<int, int>> indices_to_distances;
};

