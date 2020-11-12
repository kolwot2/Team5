#include "Railway.h"
#include <algorithm>
#include "SFML/Graphics.hpp"

Railway::Railway(int winWidth, int winHeight) 
	: windowWidth{std::max(winWidth, 0)}, windowHeight{std::max(winHeight, 0)} {
	sf::RenderWindow window(sf::VideoMode(800, 800), "Railway");
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();
		window.display();
	}
}