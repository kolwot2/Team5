#include "Railway.h"
#include <algorithm>
#include "SFML/Graphics.hpp"
#include <fstream>
#include <iostream>
#include "Drawer.h"
#include "Camera.h"
#include "../ServerConnection/ServerConnection.h"
#include "../ServerConnection/Login.h"
#include "../ServerConnection/Messages.h"
#include "../Game/Game.h"
#include "../Controller/Controller.h"

Railway::Railway(int winWidth, int winHeight) 
	: windowWidth{std::max(winWidth, 0)}, windowHeight{std::max(winHeight, 0)} {}
	
void Railway::start() {
	Controller controller;
	const auto& game = controller.GetGame();
	const Graph& graph = game.GetGraph();

	//PlaceGraph(graph, 500.f, 50.f, 0.1f, 500.f);

	sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Railway");
	window.setFramerateLimit(60);
	
	sf::View camera(sf::FloatRect(0.f, 0.f, static_cast<float>(windowWidth), static_cast<float>(windowHeight)));
	CameraConfig camera_config;
	
	Drawer drawer;
	drawer.InitRenderObjects(graph, game.GetPosts());
	
	sf::Font label_font;
	label_font.loadFromFile("fonts\\jai.ttf");

	FocusOnGraph(camera, graph);
	while (window.isOpen())
	{
		controller.MakeTurn();
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::MouseWheelMoved) {
				camera.zoom(1 - event.mouseWheel.delta / 30.0);
				drawer.ScaleObjects(1 - event.mouseWheel.delta / 30.0);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
				camera.move(-camera_config.camera_speed, 0.f);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
				camera.move(camera_config.camera_speed, 0.f);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
				camera.move(0.f, -camera_config.camera_speed);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
				camera.move(0.f, camera_config.camera_speed);
			}
		}

		window.clear();
		
		window.setView(camera);
		//drawer.drawLabels(window, graph, label_font);
		drawer.DrawObjects(window);
		
		window.setView(window.getDefaultView());
		window.display();
	}
}