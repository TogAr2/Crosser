#include <SFML/Graphics/Text.hpp>
#include "hud.hpp"

Hud::Hud(Game* game) : game(game) {
	view = sf::View();

	gameOverText = sf::Text();
	gameOverText.setFont(*game->getMainFont());
	gameOverText.setCharacterSize(24);

	fpsText = sf::Text();
	fpsText.setFont(*game->getMainFont());
	fpsText.setCharacterSize(12);
	fpsText.setString("Fps: 0");
}

void Hud::draw(sf::RenderWindow* window) {
	view.setCenter((float) width / 2, (float) height / 2);
	view.reset(sf::FloatRect(0, 0, (float) window->getSize().x, (float) window->getSize().y));
	window->setView(view);

	fpsText.setPosition(window->getSize().x - fpsText.getLocalBounds().width, 0);
	window->draw(fpsText);

	if (game->isGameOver()) {
		gameOverText.setString("Your score was: " + std::to_string(game->getScore()));
		window->draw(gameOverText);
	}
}

void Hud::setFps(int fps) {
	fpsText.setString("Fps: " + std::to_string(fps));
}