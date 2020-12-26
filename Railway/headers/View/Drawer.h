#pragma once
#include "SFML/Graphics.hpp"
#include "../Game/Game.h"

class Drawer {
	std::unordered_map <std::string, std::unique_ptr<sf::Texture>> textures;
	std::vector <std::pair <sf::Sprite, int>> posts_sprites;
	std::vector <std::pair <sf::Sprite, int>> trains_sprites;
	std::vector <std::vector <sf::Vertex>> edges;
	sf::Text rating_label;
	sf::Text post_label;
	sf::Font label_font;
	const unsigned int label_size = 18;

public:
	Drawer();
	const std::vector < std::pair <sf::Sprite, int>>& GetPostSprites();
	void InitRenderObjects(const Game &game);
	void DrawObjects(sf::RenderWindow &window);
	void ScaleObjects(const float &scale_coeff);
	void PrintPostInfo(sf::RenderWindow &window, const std::string &post_info);
	void PrintRating(sf::RenderWindow &window, const int &rating, GameState state);
	void UpdateTrainSpriteState(const Game &game);
};