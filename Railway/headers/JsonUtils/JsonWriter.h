#pragma once
#include "../ServerConnection/Login.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include <string>
#include <vector>


class JsonWriter {
public:
	JsonWriter() = delete;
	static std::string WriteLogin(const Login&);
	static std::string WriteMapLayer(int);
	static std::string WriteMove(int, int, int);
	static std::string WriteUpgrade(const std::vector<int>&, const std::vector<int>&);
};