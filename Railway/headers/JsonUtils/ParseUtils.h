#pragma once
#include <iostream>
#include <fstream>
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include <vector>
#include "../View/Graph.h"

Graph ParseGraph(std::istream& input);
