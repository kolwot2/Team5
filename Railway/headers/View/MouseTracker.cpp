#include "MouseTracker.h"

sf::Vector2f MouseTracker::GetMousePos(const sf::RenderWindow &window)
{
	mouse_pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
	return mouse_pos;
}

int MouseTracker::CheckMouseOnPost(const std::vector<std::pair<sf::Sprite, int>>& posts) {
	for (const auto &[sprite, post_index] : posts) {
		sf::Vector2f sprite_pos = sprite.getPosition();
		sf::FloatRect sprite_size = sprite.getGlobalBounds();
		if (mouse_pos.x >= sprite_pos.x && mouse_pos.x <= sprite_pos.x + sprite_size.width &&
			mouse_pos.y >= sprite_pos.y && mouse_pos.y <= sprite_pos.y + sprite_size.height) {
			return post_index;
		}
	}
	return -1;
}
