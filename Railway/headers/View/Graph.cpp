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
	: index{ ind }, post_index{ post_ind }, pos{ pt }, shape{ sf::CircleShape(10) } {}

void PlaceGraph(Graph& graph, const float& q_force_coeff, const float& k_force_coeff,
	const float& end_epsilon, const float& edge_len) {
	auto& vertexes = graph.GetVertexes();
	const auto& edges = graph.GetEdges();
	int cur_x = 0;
	int cur_y = 0;
	int sq_side1 = std::sqrt(vertexes.size());
	int sq_side2 = vertexes.size() / sq_side1;
	for (auto& vertex_pair : vertexes) {
		Point& pt = vertex_pair.second.pos;
		pt.x = static_cast<float>(cur_x++);
		pt.y = static_cast<float>(cur_y);
		if (cur_x == sq_side1) {
			cur_y++;
			cur_x = 0;
		}
	}
	float cur_max_diff = 0.f;
	float next_max_diff = 0.f;
	do {
		cur_max_diff = next_max_diff;
		for (auto& vertex_pair1 : vertexes) {
			Vertex& vertex1 = vertex_pair1.second;
			Point& pt1 = vertex1.pos;
			float force_x = 0.f;
			float force_y = 0.f;
			for (auto& vertex_pair2 : vertexes) {
				Vertex& vertex2 = vertex_pair2.second;
				Point& pt2 = vertex2.pos;
				if (pt1.x != pt2.x || pt1.y != pt2.y) {
					float x_diff = pt1.x - pt2.x;
					float y_diff = pt1.y - pt2.y;
					float q_force = q_force_coeff / (x_diff * x_diff + y_diff * y_diff);
					force_x += x_diff * q_force;
					force_y += y_diff * q_force;
					if (edges.at(vertex1.index).find(vertex2.index) != edges.at(vertex1.index).end()) {
						float x_edge_diff = pt2.x - pt1.x;
						float y_edge_diff = pt2.y - pt1.y;
						float edge_diff = (x_edge_diff * x_edge_diff + y_edge_diff * y_edge_diff) - k_force_coeff * edge_len;
						float edge_coeff = ((0.f < edge_diff) - (0.f > edge_diff)) * std::log(std::abs(edge_diff)) * 0.01;
						force_x += x_edge_diff * edge_coeff;
						force_y += y_edge_diff * edge_coeff;
					}
				}
			}
			pt1.x += force_x;
			pt1.y += force_y;
			next_max_diff = std::max(fabs(force_x), fabs(force_y));
		}
	} while (std::fabs(next_max_diff) > end_epsilon);
}
