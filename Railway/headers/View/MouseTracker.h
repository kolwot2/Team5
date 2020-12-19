#pragma once
#include "Drawer.h"


class MouseTracker {
private:
	sf::Vector2f mouse_pos;
public:
	MouseTracker() = default;
	MouseTracker(const MouseTracker &mstr) = delete;
	sf::Vector2f GetMousePos(const sf::RenderWindow &window);
	int CheckMouseOnPost(const std::vector<std::pair<sf::Sprite, int>> &posts);
};