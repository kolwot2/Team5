#pragma once
#include "SFML/Network.hpp"
#include <string>
#include "Messages.h"


class ServerConnection {
	sf::TcpSocket socket;
	const size_t recieveMaxSize = 100;
public:
	ServerConnection();
	~ServerConnection();
	ServerConnection(const ServerConnection&) = delete;
	void send(const ActionMessage &actionMessage);
	ResposeMessage recieve();
};