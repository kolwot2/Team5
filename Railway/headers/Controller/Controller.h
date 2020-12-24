#pragma once
#include "../Game/Game.h"
#include "../ServerConnection/ServerConnection.h"
#include "../ServerConnection/Messages.h"
#include "RouteManager.h"
#include "Upgrade.h"
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
	void SendUpgradeRequest(Upgrade);
	void EndTurn();
	void CheckUpgrades();
	void CheckNeededRecourse();

	int turn_number = 1;
	Game game;
	ServerConnection connection;
	std::unique_ptr<RouteManager> route_manager;
	std::queue<Upgrade> upgrade_queue;
	const int UPGRADE_COEFF = 10;
	const double RECOURSE_COEFF = 0.9;
};

