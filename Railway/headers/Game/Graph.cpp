#include "Graph.h"
#include <queue>
#include <algorithm>
#include <set>


Vertex::Vertex(int ind, int post_ind, Point pt)
	: index{ ind }, post_index{ post_ind }, pos{ pt } {}

void Graph::AddEdge(std::shared_ptr<Edge> edge)
{
	edges[edge->from][edge->to] = edge;
	idx_to_edge[edge->index] = edge;
}

void Graph::AddVertex(const Vertex& item)
{
	vertexes[item.index] = item;
}

const EdgeByVertexes& Graph::GetEdges() const
{
	return edges;
}

const std::unordered_map<int, std::shared_ptr<Edge>>& Graph::GetIndices() const
{
	return idx_to_edge;
}

const std::unordered_map <int, Vertex>& Graph::GetVertexes() const
{
	return vertexes;
}

void Graph::SetVertexCoordinates(int idx, Point coordinates)
{
	vertexes[idx].pos = coordinates;
}

std::unordered_map<int, std::unordered_map<int, int>> Graph::FloydWarshall() const {
	std::unordered_map<int, std::unordered_map<int, int>> result;
	for (const auto& [i, vertex_i] : vertexes) {
		for (const auto& [j, vertex_j] : vertexes) {
			if (i == j) {
				result[i][i] = 0;
			}
			else if (edges.at(i).find(j) != edges.at(i).end()) {
				result[i][j] = edges.at(i).at(j)->length;
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

std::vector<int> Graph::Dijkstra(int start_idx, int end_idx) const
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

		for (const auto& [idx, edge] : edges.at(current_idx)) {
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
	way.push_back(start_idx);
	std::reverse(way.begin(), way.end());
	return way;
}
