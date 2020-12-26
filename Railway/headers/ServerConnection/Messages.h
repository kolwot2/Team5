#pragma once
#include <string>
#include "../Utils/JsonWriter.h"
#include "Login.h"

enum class Action {
	LOGIN = 1,
	LOGOUT = 2,
	MOVE = 3,
	UPGRADE = 4,
	TURN = 5,
	PLAYER = 6,
	GAMES = 7,
	MAP = 10
};

enum class Result {
	OKEY = 0,
	BAD_COMMAND = 1,
	RESOURCE_NOT_FOUND = 2,
	ACCESS_DENIED = 3,
	INAPPROPRIATE_GAME_STATE = 4,
	TIMEOUT = 5,
	INTERNAL_SERVER_ERROR = 500
};

enum class GameState
{
	INIT = 1,
	RUN = 2,
	FINISHED = 3
};

struct ActionMessage {
	Action actionCode;
	std::string data;
	ActionMessage(const Login& login) : actionCode(Action::LOGIN), data(JsonWriter::WriteLogin(login)) {}
	ActionMessage(const Action& actionCode, const std::string& data) : actionCode(actionCode), data(data) {}
};

struct ResposeMessage {
	Result result;
	std::string data;
	ResposeMessage() : result(Result::ACCESS_DENIED), data("") {}
	ResposeMessage(const Result& result, const std::string& data) : result(result), data(data) {}
};

struct MoveRequest {
	MoveRequest(int line_idx, int speed, int train_idx) :
		line_idx(line_idx), speed(speed), train_idx(train_idx) {}
	int line_idx, speed, train_idx;
};