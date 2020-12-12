#include "Camera.h"

void FocusOnGraph(sf::View& camera, const Graph& graph) {
	const auto& vertexes = graph.GetVertexes();
	float min_x, min_y, max_x, max_y;
 	const Vertex& first_vertex = (*vertexes.begin()).second;
	min_x = max_x = first_vertex.pos.x;
	min_y = max_y = first_vertex.pos.y;
	for (const auto& vertex_pair : vertexes) {
		const Vertex& vertex = vertex_pair.second;
		min_x = std::min(min_x, vertex.pos.x);
		min_y = std::min(min_y, vertex.pos.y);
		max_x = std::max(max_x, vertex.pos.x);
		max_y = std::max(max_y, vertex.pos.y);
	}
	double center_x = (min_x + max_x) / 2.0;
	double center_y = (min_y + max_y) / 2.0;
	camera.setCenter(center_x, center_y);
	camera.zoom(std::max(std::abs(max_y - min_y + 10.0f) / camera.getSize().y, 
		std::abs(max_x - min_x + 10.0f) / camera.getSize().x));
}
