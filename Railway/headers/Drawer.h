#pragma once
#include "SFML/Graphics.hpp"
#include "Graph.h"

class Drawer {
public:
	void drawGraph(sf::RenderWindow&, const Graph&);
	void visualUpdate(Graph &);
};