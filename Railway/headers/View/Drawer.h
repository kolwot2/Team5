#pragma once
#include "SFML/Graphics.hpp"
#include "../Game/Graph.h"
#include "../Game/Posts.h"

class Drawer {
	std::unordered_map <std::string, std::unique_ptr<sf::Texture>> textures;
	std::vector < std::pair <sf::Sprite, int>> posts;
	std::vector <std::vector <sf::Vertex>> edges;
public:
	Drawer();
	const std::vector < std::pair <sf::Sprite, int>>& GetPostSprites();
	void InitRenderObjects(const Graph&, const std::unordered_map<int, std::shared_ptr<Post>> &idx_to_post);
	void DrawObjects(sf::RenderWindow &window);
	void ScaleObjects(const float &scale_coeff);
	void PrintPostInfo(sf::RenderWindow &window, const std::string &post_info, const sf::Font &font);
};