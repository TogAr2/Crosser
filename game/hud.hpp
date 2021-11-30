#ifndef CROSSER_HUD_HPP
#define CROSSER_HUD_HPP

#include <SFML/Graphics.hpp>
#include "game.hpp"

class Hud {
	Game* game;
	sf::View view;
	sf::Text gameOverText;
	sf::Text fpsText;

public:
	explicit Hud(Game* game);

	void draw(sf::RenderWindow* window);
	void setFps(int fps);
};

#endif //CROSSER_HUD_HPP
