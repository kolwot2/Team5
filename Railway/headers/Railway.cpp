#include "Railway.h"
#include <algorithm>
#include "SFML/Graphics.hpp"
#include <fstream>
#include "ParseGraph.h"
#include "Drawer.h"

Railway::Railway(int winWidth, int winHeight) 
	: windowWidth{std::max(winWidth, 0)}, windowHeight{std::max(winHeight, 0)} {}
	
void Railway::start() {
	std::ifstream fin("tests\\big_graph.json");
		auto graph = ParseGraph(fin);
	fin.close();
	PlaceGraph(graph, 500.f, 50.f, 10.f, 500.f);
	sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Railway");
	Drawer drawer;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();
		drawer.visualUpdate(graph);
		drawer.drawGraph(window, graph);
		window.display();
	}
}