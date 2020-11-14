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
	sf::CircleShape shape;
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
	std::unordered_map <int, Vertex> vertexes;

public:
	void AddEdge(const Edge&, int, int);
	void AddVertex(const Vertex&);
	const Edg& GetEdges() const;
	std::unordered_map <int, Vertex>& GetVertexes();
	const std::unordered_map <int, Vertex>& GetVertexes() const;
};

void PlaceGraph(Graph &,const float&, const float&, const float&, const float&);