#pragma once
#include "SFML/Graphics.hpp"
#include "Graph.h"

class Drawer {
public:
	void drawGraph(sf::RenderWindow&, const Graph&);
	void drawLabels(sf::RenderWindow&, const Graph&, const sf::Font&);
	void visualUpdate(Graph &);
};