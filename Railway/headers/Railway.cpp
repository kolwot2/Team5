#include "Railway.h"
#include <algorithm>
#include "SFML/Graphics.hpp"
#include <fstream>
#include "ParseGraph.h"
#include "Drawer.h"
#include "Camera.h"

Railway::Railway(int winWidth, int winHeight) 
	: windowWidth{std::max(winWidth, 0)}, windowHeight{std::max(winHeight, 0)} {}
	
void Railway::start() {
	std::ifstream fin("tests\\cylinder.json");
	auto graph = ParseGraph(fin);
	fin.close();
	
	PlaceGraph(graph, 500.f, 50.f, 1.f, 500.f);

	sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Railway");
	
	sf::View camera(sf::FloatRect(0.f, 0.f, static_cast<float>(windowWidth), static_cast<float>(windowHeight)));
	
	Drawer drawer;
	
	sf::Font label_font;
	label_font.loadFromFile("fonts\\jai.ttf");

	FocusOnGraph(camera, graph);
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();
		
		window.setView(camera);
		drawer.visualUpdate(graph);
		drawer.drawGraph(window, graph);
		drawer.drawLabels(window, graph, label_font);
		
		window.setView(window.getDefaultView());
		window.display();
	}
}