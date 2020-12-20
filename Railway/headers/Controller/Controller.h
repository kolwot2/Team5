#pragma once
#include "../Game/Game.h"
#include "../ServerConnection/ServerConnection.h"
#include "../ServerConnection/Messages.h"
#include <queue>
#include <set>

class Controller
{
public:
	Controller();
	~Controller();
	const Game& GetGame();
	void MakeTurn();
private:
	void UpdateGame();
	void MoveTrains();
	void MakeMoveRequests(int, int, int);
	void SendMoveRequests();
	void EndTurn();
	std::set<std::pair<double, int>> CalculatePriority(int);

	Game game;
	ServerConnection connection;
	std::unordered_map<int, std::queue<MoveRequest>> moves;
	std::unordered_map<int, std::unordered_map<int, int>> indices_to_distances;
};

