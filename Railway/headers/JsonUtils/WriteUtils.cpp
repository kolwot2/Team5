#include "WriteUtils.h"

using namespace rapidjson;
using namespace std;

string WriteLogin(const Login& login) {
	Document document;
	document.SetObject();
	auto& alloc = document.GetAllocator();

	document.AddMember("name", Value(login.name.c_str(), login.name.size(), alloc), alloc);
	if (login.parrword.has_value()) {
		document.AddMember("password", Value(login.parrword.value().c_str(), alloc), alloc);
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

string WriteMapLayer(int layer)
{
	return "{\"layer\":" + to_string(layer) + "}";
}

string WriteMove(int line_idx, int speed, int train_idx)
{
	Document document;
	document.SetObject();
	auto& alloc = document.GetAllocator();

	document.AddMember("line_idx", line_idx, alloc);
	document.AddMember("speed", speed, alloc);
	document.AddMember("train_idx", train_idx, alloc);

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	document.Accept(writer);

	return move(buffer.GetString());
}

string WriteUpgrade(const vector<int>& posts, const vector<int>& trains)
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
