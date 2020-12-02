#include <iostream>

#include <SFML/Graphics.hpp>

#include "game.hpp"
#include "config.hpp"

#include <chrono>

using namespace std;
using namespace std::chrono_literals;

constexpr std::chrono::nanoseconds timestep(50ms);

int main() {
    sf::RenderWindow window(sf::VideoMode(760, 760), "Crosser v" + (string) CROSSER_VERSION, sf::Style::Titlebar | sf::Style::Close);

  	Game game(&window);
  	game.draw(0);

  	using clock = std::chrono::high_resolution_clock;

  	std::chrono::nanoseconds lag(0ns);
  	auto time_start = clock::now();

  	while (!game.isGameOver() && window.isOpen()) {
  		auto delta_time = clock::now() - time_start;
  		time_start = clock::now();
  		lag += std::chrono::duration_cast<std::chrono::nanoseconds>(delta_time);

  		//Update game logic as lag permits
  		while (lag >= timestep) {
  			lag -= timestep;

			game.input();
  			game.logic(); //Update at a fixed state each time
  		}

  		//Calculate how close or far we are from the next timestep
  		float alpha = (float) lag.count() / timestep.count();
  		game.draw(alpha);
  	}

  	if (window.isOpen()) {
		window.clear(sf::Color::Black);

		sf::Text text("Your score was: " + to_string(game.getScore()), game.getMainFont(), 16);
		window.draw(text);

		window.display();

		while (window.isOpen()) {
			sf::Event event{};
			while (window.pollEvent(event)) {
				if (event.type == sf::Event::Closed)
					window.close();
			}
		}
	}

  	return 0;
}