#include "Drawer.h"

Drawer::Drawer() {
	std::unique_ptr<sf::Texture> town_texture(new sf::Texture);
	town_texture->loadFromFile("textures/town.png");
	textures[PostType::TOWN] = std::move(town_texture);

	std::unique_ptr<sf::Texture> storage_texture(new sf::Texture);
	storage_texture->loadFromFile("textures/storage.png");
	textures[PostType::STORAGE] = std::move(storage_texture);

	std::unique_ptr<sf::Texture> market_texture(new sf::Texture);
	market_texture->loadFromFile("textures/market.png");
	textures[PostType::MARKET] = std::move(market_texture);

	std::unique_ptr<sf::Texture> default_texture(new sf::Texture);
	default_texture->loadFromFile("textures/default.png");
	textures[PostType::DEFAULT] = std::move(default_texture);
}

void Drawer::drawLabels(sf::RenderWindow &window, const Graph &graph, const sf::Font &label_font) {
	const auto& vertexes = graph.GetVertexes();
	for (const auto& vertex_pair : vertexes) {
		const Vertex& vertex = vertex_pair.second;
		sf::Text label_text(std::to_string(vertex.index), label_font, 50u);
		label_text.setPosition(sf::Vector2f(vertex.pos.x, vertex.pos.y));
		label_text.setFillColor(sf::Color::White);
		window.draw(label_text);
	}
}

void Drawer::InitRenderObjects(const Graph &graph, const std::unordered_map<int, std::unique_ptr<Post>> &idx_to_post) {
	const auto& vertexes = graph.GetVertexes();
	const auto& edges = graph.GetEdges();
	for (const auto &vertex_pair : vertexes) {
		const auto &vertex = vertex_pair.second;
		if (idx_to_post.find(vertex.index) != idx_to_post.end()) {
			Post post = *idx_to_post.at(vertex.index);
			sf::Sprite sprite;
			sprite.setTexture(*textures[post.type]);
			sprite.setPosition(sf::Vector2f(vertex.pos.x - sprite.getTexture()->getSize().x / 2.0f, 
								vertex.pos.y - sprite.getTexture()->getSize().y / 2.0f));
			posts.push_back(sprite);
		}
		else {
			sf::Sprite sprite;
			sprite.setTexture(*textures[PostType::DEFAULT]);
			sprite.setPosition(sf::Vector2f(vertex.pos.x - sprite.getTexture()->getSize().x / 2.0f,
				vertex.pos.y - sprite.getTexture()->getSize().y / 2.0f));
			posts.push_back(sprite);
		}
	}
	
	for (const auto &first_vertex : edges) {
		for (const auto &second_vertex : first_vertex.second) {
			Point p1 = vertexes.at(first_vertex.first).pos;
			Point p2 = vertexes.at(second_vertex.first).pos;
			std::vector <sf::Vertex> line;
			line.push_back(sf::Vertex(sf::Vector2f(p1.x, p1.y)));
			line.push_back(sf::Vertex(sf::Vector2f(p2.x, p2.y)));
			this->edges.push_back(line);
		}
	}
}

void Drawer::DrawObjects(sf::RenderWindow &window) {
	for (auto &edge : edges) {
		window.draw(&edge[0], edge.size(), sf::Lines);
	}
	for (const auto &post : posts) {
		window.draw(post);
	}
}

void Drawer::ScaleObjects(const float &scale_coeff) {
	for (auto &post : posts) {
		sf::Vector2f cur_position = post.getPosition();
		sf::FloatRect prev_post_size = post.getGlobalBounds();
		sf::Vector2f post_center = sf::Vector2f(cur_position.x + prev_post_size.width / 2.0f,
											cur_position.y + prev_post_size.height / 2.0f);
		post.scale(sf::Vector2f(scale_coeff, scale_coeff));
		sf::FloatRect post_size = post.getGlobalBounds();
		post.setPosition(sf::Vector2f(post_center.x - post_size.width / 2.0f,
								post_center.y - post_size.height / 2.0f));
	}
}
