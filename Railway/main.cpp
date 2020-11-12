#include <fstream>
#include "headers/ParseGraph.h"
#include "headers/Graph.h"

int main() 
{	
	std::ifstream fin("tests\\small_graph.json");
	auto graph = ParseGraph(fin);
	return 0;
}