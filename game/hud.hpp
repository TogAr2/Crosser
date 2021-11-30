#ifndef CROSSER_HUD_HPP
#define CROSSER_HUD_HPP

#include <SFML/Graphics.hpp>
#include "game.hpp"

class Hud {
	Game* game;
	sf::View view;
	sf::Text gameOverText;

public:
	explicit Hud(Game* game);

	void draw(sf::RenderWindow* window);
};

#endif //CROSSER_HUD_HPP
