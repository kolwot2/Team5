#pragma once
#include "../Game/Game.h"
#include "../ServerConnection/ServerConnection.h"

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
};

