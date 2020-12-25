#pragma once
#include "../Game/Game.h"
#include "../ServerConnection/ServerConnection.h"
#include "../ServerConnection/Messages.h"
#include "../Utils/FileLogger.h"
#include "RouteManager.h"
#include "Upgrade.h"
#include <queue>
#include <set>
#include <mutex>

struct SynchronizedGame {
	Game& game;
	std::lock_guard<std::mutex> guard;
};

class Controller
{
public:
	Controller();
	void Init();
	void Disconnect();
	const SynchronizedGame GetGame();
	void MakeTurn();
	int GetTurnNumber() const;
private:
	void UpdateGame();
	void SendMoveRequests(const std::vector<MoveRequest>&);
	void SendUpgradeRequest(Upgrade);
	void EndTurn();
	void CheckUpgrades();
	void LogErrorRecieve(const ResposeMessage&, const std::string&);

	int turn_number = 1;
	Game game;
	ServerConnection connection;
	RouteManager route_manager;
	std::queue<Upgrade> upgrade_queue;
	const int UPGRADE_COEFF = 10;
	ige::FileLogger logger;
	std::mutex game_mutex;
};

