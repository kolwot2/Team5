#pragma once
#include <iostream>
#include <fstream>
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include <vector>
#include "../Game/Graph.h"
#include "../Game/Game.h"

Graph ParseGraph(std::string input);
Player ParsePlayer(std::string input);
std::vector<std::pair<int,Point>> ParseCoordinates(std::string input);
std::unordered_map<int, std::unique_ptr<Post>> ParsePosts(std::string input);