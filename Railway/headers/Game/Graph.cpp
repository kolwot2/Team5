#include "Graph.h"
#include <queue>
#include <algorithm>
#include <set>

void Graph::AddEdge(std::shared_ptr<Edge> edge)
{
	edges[edge->from][edge->to] = edge;
	idx_to_edge[edge->index] = edge;
}

void Graph::AddVertex(const Vertex& item)
{
	vertexes[item.index] = item;
}

const Edg& Graph::GetEdges() const
{
	return edges;
}

const std::unordered_map<int, std::shared_ptr<Edge>>& Graph::GetIndices() const
{
	return idx_to_edge;
}

std::unordered_map <int, Vertex>& Graph::GetVertexes()
{
	return vertexes;
}

const std::unordered_map <int, Vertex>& Graph::GetVertexes() const
{
	return vertexes;
}

void Graph::SetVertexCoordinates(int idx, Point coordinates)
{
	vertexes[idx].pos = coordinates;
}

std::unordered_map<int, std::unordered_map<int, int>> Graph::FloydWarshall() {
	std::unordered_map<int, std::unordered_map<int, int>> result;
	for (const auto& [i, vertex_i] : vertexes) {
		for (const auto& [j, vertex_j] : vertexes) {
			if (i == j) {
				result[i][i] = 0;
			}
			else if (edges[i].find(j) != edges[i].end()) {
				result[i][j] = edges[i][j]->length;
			}
			else {
				result[i][j] = INT_MAX;
			}
		}
	}
	for (const auto& [k, vertex_k] : vertexes) {
		for (const auto& [i, vertex_i] : vertexes) {
			for (const auto& [j, vertex_j] : vertexes) {
				if (result[i][k] < INT_MAX && result[k][j] < INT_MAX) {
					result[i][j] = std::min(result[i][j], result[i][k] + result[k][j]);
				}
			}
		}
	}
	return result;
}

std::vector<int> Graph::Dijkstra(int start_idx, int end_idx)
{
	const int INF = INT_MAX;
	std::unordered_map<int, std::pair<int, int>> idx_to_dist_prev;
	for (const auto& [idx, vertex] : vertexes) {
		idx_to_dist_prev[idx] = { INF, idx };
	}
	idx_to_dist_prev[start_idx] = { 0,start_idx };

	std::set<std::pair<int, int>> distances;
	distances.emplace(0, start_idx);
	while (!distances.empty()) {
		auto [current_dist, current_idx] = *distances.begin();
		distances.erase(distances.begin());

		for (const auto& [idx, edge] : edges[current_idx]) {
			if (int dist = current_dist + edge->length;
				dist < idx_to_dist_prev[idx].first) {
				distances.erase({ idx_to_dist_prev[idx].first, idx });
				idx_to_dist_prev[idx].first = dist;
				idx_to_dist_prev[idx].second = current_idx;
				distances.emplace(dist, idx);
			}
		}
	}
	
	std::vector<int> way = { end_idx };
	for (int current_idx = idx_to_dist_prev[end_idx].second; current_idx != start_idx;
		current_idx = idx_to_dist_prev[current_idx].second) {
		way.push_back(current_idx);
	}
	std::reverse(way.begin(), way.end());
	return way;
}

int Graph::GetVertexFromPosition(int line_idx, int pos)
{
	for (const auto& [from, edges_map]:edges) {
		for (const auto& [to, edge] : edges_map) {
			if (line_idx == edge->index) {
				return pos == 0 ? from : to;
			}
		}
	}
}


Vertex::Vertex(int ind, int post_ind, Point pt)
	: index{ ind }, post_index{ post_ind }, pos{ pt } {}

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
