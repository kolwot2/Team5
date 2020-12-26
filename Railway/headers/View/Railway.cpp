#include "Railway.h"
#include <algorithm>
#include "SFML/Graphics.hpp"
#include <fstream>
#include "Drawer.h"
#include "Camera.h"
#include "../ServerConnection/ServerConnection.h"
#include "../ServerConnection/Login.h"
#include "../ServerConnection/Messages.h"
#include "../Game/Game.h"
#include "../Controller/Controller.h"
#include "MouseTracker.h"
#include <future>

Railway::Railway(int winWidth, int winHeight) 
	: windowWidth{std::max(winWidth, 0)}, windowHeight{std::max(winHeight, 0)} {}
	
void Railway::start() {
	Controller controller;
	controller.Init();
	//PlaceGraph(graph, 500.f, 50.f, 0.1f, 500.f);

	sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Railway");
	window.setFramerateLimit(60);
	
	sf::View camera(sf::FloatRect(0.f, 0.f, static_cast<float>(windowWidth), static_cast<float>(windowHeight)));
	CameraConfig camera_config;
	
	Drawer drawer;
	{
		auto sync_game = controller.GetGame();
		drawer.InitRenderObjects(sync_game.game);
		FocusOnGraph(camera, sync_game.game.GetGraph(), drawer);
	}

	MouseTracker mouse_tracker;

	std::promise<void> stop;
	auto turn_processing = [&controller](std::future<void> stop) {
		auto period = std::chrono::microseconds(0);
		controller.WaitForGameStart();
		while (stop.wait_for(period) != std::future_status::ready &&
			controller.GetGameState() != GameState::FINISHED) {
			controller.MakeTurn();
		}
	};
	std::thread(turn_processing, stop.get_future()).detach();

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) {
				stop.set_value();
				controller.Disconnect();
				return;
			}
			if (event.type == sf::Event::MouseWheelMoved) {
				camera.zoom(1 - event.mouseWheel.delta / 30.0);
				drawer.ScaleObjects(1 - event.mouseWheel.delta / 30.0);
			}
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

		window.clear();
		window.setView(camera);

		{
			auto sync_game = controller.GetGame();
			drawer.UpdateTrainSpriteState(sync_game.game);
		}

		drawer.DrawObjects(window);
		mouse_tracker.GetMousePos(window);
		window.setView(window.getDefaultView());

		{
			auto sync_game = controller.GetGame();
			const auto& game = sync_game.game;
			drawer.PrintPostInfo(window, game.GetPostInfo(mouse_tracker.CheckMouseOnPost(drawer.GetPostSprites())));
			drawer.PrintRating(window, game.GetPlayer().rating, controller.GetGameState());
		}

		window.display();
	}
}