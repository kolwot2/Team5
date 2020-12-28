#include "JsonWriter.h"
#include "../ServerConnection/Messages.h"

using namespace rapidjson;
using namespace std;

string JsonWriter::WriteLogin(const Login& login) {
	Document document;
	document.SetObject();
	auto& alloc = document.GetAllocator();

	document.AddMember("name", Value(login.name.c_str(), login.name.size(), alloc), alloc);
	if (login.password.has_value()) {
		document.AddMember("password", Value(login.password.value().c_str(), alloc), alloc);
	}
	if (login.game.has_value()) {
		document.AddMember("game", Value(login.game.value().c_str(), alloc), alloc);
	}
	if (login.num_players.has_value()) {
		document.AddMember("num_players", login.num_players.value(), alloc);
	}
	if (login.num_turns.has_value()) {
		document.AddMember("num_turns", login.num_turns.value(), alloc);
	}

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	document.Accept(writer);

	return move(buffer.GetString());
}

string JsonWriter::WriteMapLayer(int layer)
{
	return "{\"layer\":" + to_string(layer) + "}";
}

string JsonWriter::WriteMove(MoveRequest move_request)
{
	Document document;
	document.SetObject();
	auto& alloc = document.GetAllocator();

	document.AddMember("line_idx", move_request.line_idx, alloc);
	document.AddMember("speed", move_request.speed, alloc);
	document.AddMember("train_idx", move_request.train_idx, alloc);

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	document.Accept(writer);
	std::string s = buffer.GetString();
	return move(s);
}

string JsonWriter::WriteUpgrade(const vector<int>& posts, const vector<int>& trains)
{
	Document document;
	document.SetObject();
	auto& alloc = document.GetAllocator();

	Value posts_array(kArrayType);
	for (const auto& i : posts) {
		posts_array.PushBack(i, alloc);
	}

	Value trains_array(kArrayType);
	for (const auto& i : trains) {
		trains_array.PushBack(i, alloc);
	}

	document.AddMember("posts", posts_array, alloc);
	document.AddMember("trains", trains_array, alloc);

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	document.Accept(writer);

	return move(buffer.GetString());
}
