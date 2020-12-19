#include "MouseTracker.h"

sf::Vector2f MouseTracker::getMousePos(const sf::RenderWindow &window)
{
	sf::Vector2f mouse_pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
	return mouse_pos;
}
