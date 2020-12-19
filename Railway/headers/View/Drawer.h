#pragma once
#include "SFML/Graphics.hpp"
#include "../Game/Graph.h"
#include "../Game/Posts.h"

class Drawer {
	std::map <PostType, std::unique_ptr<sf::Texture>> textures;
	std::vector <sf::Sprite> posts;
	std::vector <std::vector <sf::Vertex>> edges;
public:
	Drawer();
	void drawLabels(sf::RenderWindow&, const Graph&, const sf::Font&);
	void InitRenderObjects(const Graph&, const std::unordered_map<int, std::shared_ptr<Post>> &idx_to_post);
	void DrawObjects(sf::RenderWindow &window);
	void ScaleObjects(const float &scale_coeff);
};