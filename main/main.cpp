#include <SFML/Graphics.hpp>

#include "../render/render.hpp"
#include "config.hpp"
#include "logger.hpp"
#include "../multiplayer/network.hpp"

#include <chrono>

using namespace std::chrono_literals;

constexpr std::chrono::nanoseconds timestep(50ms);

int main(int argc, char* argv[]) {
	if (argc > 2) {
		//Parse arguments
		Network::ip = argv[1];
		Network::port = std::stoi(argv[2]);
	}

    sf::RenderWindow window(sf::VideoMode(width * blockSize * 2, height * blockSize * 2), "Crosser v" + (std::string) CROSSER_VERSION, sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize);
    window.setFramerateLimit(120);
	window.setVerticalSyncEnabled(true);

  	Render render(&window);
	PluginManager::instance.start();

  	using clock = std::chrono::high_resolution_clock;

  	std::chrono::nanoseconds lag(0ns);
  	auto time_start = clock::now();

	int countedFrames = 0;
	sf::Clock sfClock;
  	while (window.isOpen()) {
		countedFrames++;
		if (sfClock.getElapsedTime().asMilliseconds() >= 1000) {
			render.getHud()->setFps(countedFrames);
			sfClock.restart();
			countedFrames = 0;
		}

  		auto delta_time = clock::now() - time_start;
  		time_start = clock::now();
  		lag += std::chrono::duration_cast<std::chrono::nanoseconds>(delta_time);

  		//Update game logic as lag permits
  		while (lag >= timestep) {
  			lag -= timestep;

			render.input();
  			render.logic(); //Update at a fixed state each time
  		}

  		//Calculate how close or far we are from the next timestep
  		float alpha = (float) lag.count() / (float) timestep.count();
  		render.draw(alpha);
  	}

  	PluginManager::instance.end();

  	return 0;
}