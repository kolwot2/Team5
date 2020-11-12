#include "Graph.h"

void Graph::AddEdge(const Edge& edge, int x, int y)
{
	edges[x][y] = edge;
}

void Graph::AddVertex(const Vertex& item)
{
	vertexes.push_back(item);
}

const Edg& Graph::GetEdges()
{
	return edges;
}

std::vector <Vertex>& Graph::GetVertex()
{
	return vertexes;
}