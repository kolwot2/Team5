#include <iostream>
#include "SFML/Graphics.hpp"

using namespace std;

int main() {
	sf::RenderWindow window(sf::VideoMode(800, 800), "SFML works!");
	sf::Vertex line[] = {sf::Vertex(sf::Vector2f(10, 10)), sf::Vertex(sf::Vector2f(150, 150))};

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();
		window.draw(line, 2, sf::Lines);
		window.display();
	}

	return 0;
}