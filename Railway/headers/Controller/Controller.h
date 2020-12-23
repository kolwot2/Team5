#pragma once
#include "../Game/Game.h"
#include "../ServerConnection/ServerConnection.h"
#include "../ServerConnection/Messages.h"
#include "RouteManager.h"
#include <queue>
#include <set>

class Controller
{
public:
	Controller();
	~Controller();
	const Game& GetGame();
	void MakeTurn();
	int GetTurnNumber() const;
private:
	void UpdateGame();
	void SendMoveRequests(const std::vector<MoveRequest>&);
	void EndTurn();

	int turn_number = 1;
	Game game;
	ServerConnection connection;
	std::unique_ptr<RouteManager> route_manager;
};

