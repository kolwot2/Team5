#pragma once
#include <iostream>
#include <fstream>
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include <vector>
#include "Graph.h"

Graph ParseGraph(std::istream& input);