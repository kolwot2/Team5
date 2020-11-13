#include "Graph.h"

void Graph::AddEdge(const Edge& edge, int x, int y)
{
	edges[x][y] = edge;
}

void Graph::AddVertex(const Vertex& item)
{
	vertexes[item.index] = item;
}

const Edg& Graph::GetEdges() const
{
	return edges;
}

std::unordered_map <int, Vertex>& Graph::GetVertexes()
{
	return vertexes;
}

const std::unordered_map <int, Vertex>& Graph::GetVertexes() const
{
	return vertexes;
}

Vertex::Vertex(int ind, int post_ind, Point pt)
	: index{ind}, post_index{post_ind}, pos{pt}, shape{sf::CircleShape(10)} {}
