#ifndef CROSSER_HUD_HPP
#define CROSSER_HUD_HPP

#include <SFML/Graphics.hpp>
#include "game.hpp"

class Hud {
	Game* game;
	sf::RenderWindow* window;
	sf::View view;
	sf::Text gameOverText;
	sf::Text fpsText;

public:
	explicit Hud(Game* game);

	void draw();
	void setFps(int fps);

	void adjustSize(unsigned int windowWidth, unsigned int windowHeight);
};

#endif //CROSSER_HUD_HPP
