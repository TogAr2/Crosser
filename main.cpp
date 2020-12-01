#include <iostream>

#include <SFML/Graphics.hpp>

#include "utils.hpp"
#include "game.cpp"
#include "config.hpp"

using namespace std;

int main() {
    sf::RenderWindow window(sf::VideoMode(840, 840), "Crosser v" + (string) CROSSER_VERSION);

  	Game game(&window);
  	game.draw();

  	while (!game.isGameOver() && window.isOpen()) {
  		sf::Event event{};
  		window.pollEvent(event);

    	game.input();
    	game.logic();
    	game.draw();

    	sf::sleep(sf::milliseconds(80));
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