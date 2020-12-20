#pragma once
#include "SFML/Graphics.hpp"
#include <unordered_map>
#include <vector>

struct Point
{
	float x;
	float y;
};

struct Vertex
{
	Vertex() = default;
	Vertex(int, int, Point);
	int index;
	int post_index;
	Point pos;
};


struct Edge
{
	int from;
	int to;
	int index;
	int length;
	bool is_reversed;
};

using Edg = std::unordered_map <int, std::unordered_map <int, std::shared_ptr<Edge>>>;

class Graph
{
	Edg edges;
	std::unordered_map<int, std::shared_ptr<Edge>> idx_to_edge;
	std::unordered_map <int, Vertex> vertexes;

public:
	void AddEdge(std::shared_ptr<Edge>);
	void AddVertex(const Vertex&);
	const Edg& GetEdges() const;
	const std::unordered_map<int, std::shared_ptr<Edge>>& GetIndices() const;
	std::unordered_map <int, Vertex>& GetVertexes();
	const std::unordered_map <int, Vertex>& GetVertexes() const;
	void SetVertexCoordinates(int, Point);
	std::unordered_map<int, std::unordered_map<int, int>> FloydWarshall();
	std::vector<int> Dijkstra(int, int);
	int GetVertexFromPosition(int, int);
};

void PlaceGraph(Graph &,const float&, const float&, const float&, const float&);