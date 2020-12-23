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

using EdgeByVertexes = std::unordered_map <int, std::unordered_map <int, std::shared_ptr<Edge>>>;
using EdgeByIndex = std::unordered_map<int, std::shared_ptr<Edge>>;

class Graph
{
	EdgeByVertexes edges;
	EdgeByIndex idx_to_edge;
	std::unordered_map <int, Vertex> vertexes;

public:
	void AddEdge(std::shared_ptr<Edge>);
	void AddVertex(const Vertex&);
	const EdgeByVertexes& GetEdges() const;
	const EdgeByIndex& GetIndices() const;
	const std::unordered_map <int, Vertex>& GetVertexes() const;
	void SetVertexCoordinates(int, Point);
	std::unordered_map<int, std::unordered_map<int, int>> FloydWarshall() const;
	std::vector<int> Dijkstra(int, int) const;
};