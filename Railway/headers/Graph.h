#pragma once
#include <unordered_map>
#include <vector>

struct Point
{
	double x;
	double y;
};

struct Vertex
{
	int index;
	int post_index;
	Point pos;
};


struct Edge
{
	int index;
	int length;
};

using Edg = std::unordered_map <int, std::unordered_map <int, Edge>>;

class Graph
{
	Edg edges;
	std::vector <Vertex> vertexes;

public:
	void AddEdge(const Edge&, int, int);
	void AddVertex(const Vertex&);
	const Edg& GetEdges();
	std::vector <Vertex>& GetVertex();
};