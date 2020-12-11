#pragma once
#include "SFML/Graphics.hpp"
#include "../Game/Graph.h"

struct CameraConfig {
	float camera_speed = 2.0f;
};

void FocusOnGraph(sf::View&, const Graph&);