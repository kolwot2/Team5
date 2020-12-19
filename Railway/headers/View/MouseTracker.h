#pragma once
#include <SFML/Graphics.hpp>

class MouseTracker {
private:
	sf::Vector2f mouse_pos;
public:
	MouseTracker() = default;
	MouseTracker(const MouseTracker &mstr) = delete;
	sf::Vector2f getMousePos(const sf::RenderWindow &window);
};