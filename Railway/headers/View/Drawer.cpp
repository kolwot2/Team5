#include "Drawer.h"

Drawer::Drawer() {
	std::unique_ptr<sf::Texture> town_texture(new sf::Texture);
	town_texture->loadFromFile("textures/town.png");
	textures["town"] = std::move(town_texture);

	std::unique_ptr<sf::Texture> storage_texture(new sf::Texture);
	storage_texture->loadFromFile("textures/storage.png");
	textures["storage"] = std::move(storage_texture);

	std::unique_ptr<sf::Texture> market_texture(new sf::Texture);
	market_texture->loadFromFile("textures/market.png");
	textures["market"] = std::move(market_texture);

	std::unique_ptr<sf::Texture> train_texture(new sf::Texture);
	train_texture->loadFromFile("textures/train.png");
	textures["train"] = std::move(train_texture);

	std::unique_ptr<sf::Texture> default_texture(new sf::Texture);
	default_texture->loadFromFile("textures/default.png");
	textures["default"] = std::move(default_texture);

	label_font.loadFromFile("fonts\\jai.ttf");
}

const std::vector<std::pair<sf::Sprite, int>>& Drawer::GetPostSprites() {
	return posts_sprites;
}

void Drawer::InitRenderObjects(const Game &game) {
	const auto &graph = game.GetGraph();
	const auto &idx_to_post = game.GetPosts();
	const auto &vertexes = graph.GetVertexes();
	const auto &edges = graph.GetEdges();
	const auto &trains = game.GetPlayer().trains;
	for (const auto &vertex_pair : vertexes) {
		const auto &vertex = vertex_pair.second;
		if (idx_to_post.find(vertex.index) != idx_to_post.end()) {
			Post post = *idx_to_post.at(vertex.index);
			sf::Sprite sprite;
			if(post.type == PostType::TOWN) sprite.setTexture(*textures["town"]);
			if(post.type == PostType::MARKET) sprite.setTexture(*textures["market"]);
			if (post.type == PostType::STORAGE) sprite.setTexture(*textures["storage"]);
			sprite.setPosition(sf::Vector2f(vertex.pos.x - sprite.getTexture()->getSize().x / 2.0f, 
								vertex.pos.y - sprite.getTexture()->getSize().y / 2.0f));
			posts_sprites.push_back({sprite, vertex.index});
		}
		else {
			sf::Sprite sprite;
			sprite.setTexture(*textures["default"]);
			sprite.setPosition(sf::Vector2f(vertex.pos.x - sprite.getTexture()->getSize().x / 2.0f,
				vertex.pos.y - sprite.getTexture()->getSize().y / 2.0f));
			posts_sprites.push_back({sprite, vertex.index});
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

	for (const auto &[idx, train] : trains) {
		sf::Sprite train_sprite;
		train_sprite.setTexture(*textures["train"]);
		trains_sprites.push_back({train_sprite, train.idx});
	}

	post_label.setFont(label_font);
	post_label.setCharacterSize(label_size);

	rating_label.setFont(label_font);
	rating_label.setCharacterSize(label_size);
}

void Drawer::DrawObjects(sf::RenderWindow &window) {
	for (auto &edge : edges) {
		window.draw(&edge[0], edge.size(), sf::Lines);
	}

	for (const auto &post : posts_sprites) {
		window.draw(post.first);
	}

	for (const auto &[sprite, index] : trains_sprites) {
		window.draw(sprite);
	}
}

void Drawer::ScaleObjects(const float &scale_coeff) {
	for (auto &post : posts_sprites) {
		sf::Vector2f cur_position = post.first.getPosition();
		sf::FloatRect prev_post_size = post.first.getGlobalBounds();
		sf::Vector2f post_center = sf::Vector2f(cur_position.x + prev_post_size.width / 2.0f,
											cur_position.y + prev_post_size.height / 2.0f);
		post.first.scale(sf::Vector2f(scale_coeff, scale_coeff));
		sf::FloatRect post_size = post.first.getGlobalBounds();
		post.first.setPosition(sf::Vector2f(post_center.x - post_size.width / 2.0f,
								post_center.y - post_size.height / 2.0f));
	}

	for (auto &[train_sprite, train_index] : trains_sprites) {
		sf::Vector2f cur_position = train_sprite.getPosition();
		sf::FloatRect prev_train_size = train_sprite.getGlobalBounds();
		sf::Vector2f train_center = sf::Vector2f(cur_position.x + prev_train_size.width / 2.0f,
			cur_position.y + prev_train_size.height / 2.0f);
		train_sprite.scale(sf::Vector2f(scale_coeff, scale_coeff));
		sf::FloatRect post_size = train_sprite.getGlobalBounds();
		train_sprite.setPosition(sf::Vector2f(train_center.x - post_size.width / 2.0f,
			train_center.y - post_size.height / 2.0f));
	}
}

void Drawer::PrintPostInfo(sf::RenderWindow &window, const std::string &post_info) {
	post_label.setString(post_info);
	window.draw(post_label);
}

void Drawer::PrintRating(sf::RenderWindow &window, const int &rating, GameState state) {
	switch (state)
	{
	case GameState::INIT:
		rating_label.setString("Waiting for game's start");
		break;
	case GameState::RUN:
		rating_label.setString("Game is running. Rating: " + std::to_string(rating));
		break;
	case GameState::FINISHED:
		rating_label.setString("GAME OVER. Rating: " + std::to_string(rating));
		break;
	}
	rating_label.setPosition(sf::Vector2f((window.getSize().x - rating_label.getLocalBounds().width) / 2.0f, 0));
	window.draw(rating_label);
}



void Drawer::UpdateTrainSpriteState(const Game &game) {
	const auto &trains = game.GetPlayer().trains;
	const auto &idx_to_edge = game.GetGraph().GetIndices();
	const auto &vertecies = game.GetGraph().GetVertexes();
	for (auto &[train_sprite, train_index] : trains_sprites) {
		for (const auto &[idx, train] : trains) {
			if (train_index == train.idx) {
				const int &edge_ind = train.line_idx;
				const auto &edge = *idx_to_edge.at(edge_ind);
				const auto &start_point = vertecies.at(edge.from).pos;
				const auto &end_point = vertecies.at(edge.to).pos;
				float dir_cos = (end_point.x - start_point.x) / edge.length;
				float dir_sin = (end_point.y - start_point.y) / edge.length;
				Point center;
				center.x = end_point.x - dir_cos * train.position;
				center.y = end_point.y - dir_sin * train.position;
				
				train_sprite.setPosition(center.x - train_sprite.getGlobalBounds().width / 2.0f,
					center.y - train_sprite.getGlobalBounds().height / 2.0f);
			}
		}
	}
}

