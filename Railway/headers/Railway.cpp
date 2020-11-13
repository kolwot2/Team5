#include "Railway.h"
#include <algorithm>
#include "SFML/Graphics.hpp"
#include <fstream>
#include "ParseGraph.h"
#include "Drawer.h"

Railway::Railway(int winWidth, int winHeight) 
	: windowWidth{std::max(winWidth, 0)}, windowHeight{std::max(winHeight, 0)} {}
	
void Railway::start() {
	std::ifstream fin("tests\\small_graph.json");
		auto graph = ParseGraph(fin);
	fin.close();
	for (auto &vertex : graph.GetVertexes()) {
		vertex.second.pos.x = static_cast<float>(rand() % 200 + 50);
		vertex.second.pos.y = static_cast<float>(rand() % 200 + 50);
	}
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