#include "ServerConnection.h"
#include <iostream>

ServerConnection::ServerConnection() {
	sf::Socket::Status status = socket.connect("wgforge-srv.wargaming.net", 443);
	if (status != sf::Socket::Done) {
		std::cout << "Couldn't connect to the server";
	}
}

ServerConnection::~ServerConnection() {
	socket.disconnect();
}

void ServerConnection::send(const ActionMessage &actionMessage) {
	int type = static_cast<int>(actionMessage.actionCode);
	socket.send(reinterpret_cast<char*>(&type), sizeof(type));
	int length = actionMessage.data.size();
	socket.send(reinterpret_cast<char*>(&length), sizeof(length));
	if (length) {
		socket.send(actionMessage.data.c_str(), (size_t)length);
	}
}

ResposeMessage ServerConnection::recieve() {
	int type, length;
	std::string str_data = "";
	size_t recieved;
	socket.receive(&type, sizeof(type), recieved);
	socket.receive(&length, sizeof(length), recieved);
	size_t sum_recieved = 0;
	char *data = new char[recieveMaxSize + 1];
	while (sum_recieved < length) {
		socket.receive(data, recieveMaxSize, recieved);
		sum_recieved += recieved;
		data[recieved] = 0;
		std::string part_data(data);
		str_data += part_data;
	}
	delete [] data;
	ResposeMessage response((Result)type, str_data);
	return response;
}