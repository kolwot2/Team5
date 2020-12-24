#pragma once
#include <iostream>
#include <fstream>
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include <vector>
#include "../Game/Graph.h"
#include "../Game/Game.h"

class JsonParser {
public:
	JsonParser() = delete;
	static Graph ParseGraph(std::string& input);
	static Player ParsePlayer(std::string& input);
	static std::vector<std::pair<int, Point>> ParseCoordinates(std::string& input);
	static std::unordered_map<int, std::shared_ptr<Post>> ParsePosts(std::string& input);
private:
	static std::vector<Train> ParseTrains(const rapidjson::GenericArray<false, rapidjson::Value>& array);
	static std::shared_ptr<Town> ParseTown(const rapidjson::Value& town_item);
	static std::shared_ptr<Market> ParseMarket(const rapidjson::Value& market_item);
	static std::shared_ptr<Storage> ParseStorage(const rapidjson::Value& storage_item);
};