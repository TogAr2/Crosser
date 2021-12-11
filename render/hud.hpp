#ifndef CROSSER_HUD_HPP
#define CROSSER_HUD_HPP

#include <SFML/Graphics.hpp>
#include "../game/game.hpp"
#include "gui.hpp"

class Hud {
	sf::Text gameOverText;
	sf::Text fpsText;

public:
	explicit Hud(const sf::Font &font);

	void draw(sf::RenderWindow* &window);
	void setFps(int fps);

	void adjustSize(unsigned int windowWidth, unsigned int windowHeight);
};

#endif //CROSSER_HUD_HPP
