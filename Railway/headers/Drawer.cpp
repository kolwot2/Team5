#include "Drawer.h"

void Drawer::drawGraph(sf::RenderWindow &window, const Graph &graph) {
	const auto& edges = graph.GetEdges();
	const auto& vertexes = graph.GetVertexes();
	for (const auto &vertex : vertexes) {
		window.draw(vertex.second.shape);
	}
	for (const auto &first_vertex : edges) {
		for (const auto &second_vertex : first_vertex.second) {
			Point p1 = vertexes.at(first_vertex.first).pos;
			Point p2 = vertexes.at(second_vertex.first).pos;
			sf::Vertex line[2] = { sf::Vertex(sf::Vector2f(p1.x, p1.y)), 
									sf::Vertex(sf::Vector2f(p2.x, p2.y)) };
			window.draw(line, 2, sf::Lines);
		}
	}
}

void Drawer::drawLabels(sf::RenderWindow &window, const Graph &graph, const sf::Font &label_font) {
	const auto& vertexes = graph.GetVertexes();
	for (const auto& vertex_pair : vertexes) {
		const Vertex& vertex = vertex_pair.second;
		sf::Text label_text(std::to_string(vertex.index), label_font, 50u);
		label_text.setPosition(sf::Vector2f(vertex.pos.x, vertex.pos.y - vertex.shape.getRadius() * 5));
		label_text.setFillColor(sf::Color::White);
		window.draw(label_text);
	}
}

void Drawer::visualUpdate(Graph &graph) {
	auto& vertexes = graph.GetVertexes();
	for (auto& vertex_pair : vertexes) {
		Vertex& vertex = vertex_pair.second;
		if (vertex.pos.x != vertex.shape.getPosition().x ||
			vertex.pos.y != vertex.shape.getPosition().y) {
			vertex.shape.setPosition(vertex.pos.x - vertex.shape.getRadius(), vertex.pos.y - vertex.shape.getRadius());
		}
	}
}
