#pragma once
#include "../ServerConnection/Login.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include <string>
#include <vector>

std::string WriteLogin(const Login&);

std::string WriteMapLayer(int);

std::string WriteMove(int, int, int);

std::string WriteUpgrade(const std::vector<int>&, const std::vector<int>&);