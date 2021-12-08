#include <SFML/Graphics/Text.hpp>
#include "hud.hpp"

Hud::Hud(Game* game) : game(game) {
	window = game->getWindow();
	view = sf::View();

	gameOverText = sf::Text();
	gameOverText.setFont(*game->getMainFont());
	gameOverText.setCharacterSize(24);

	fpsText = sf::Text();
	fpsText.setFont(*game->getMainFont());
	fpsText.setCharacterSize(12);
	fpsText.setString("Fps: 0");
}

void Hud::draw() {
	window->setView(view);

	window->draw(fpsText);

	if (game->isGameOver()) {
		gameOverText.setString("Your score was: " + std::to_string(game->getScore()));
		window->draw(gameOverText);
	}
}

void Hud::setFps(int fps) {
	fpsText.setString("Fps: " + std::to_string(fps));
	fpsText.setPosition((float) window->getSize().x - fpsText.getLocalBounds().width, 0);
}

void Hud::adjustSize(unsigned int windowWidth, unsigned int windowHeight) {
	view.reset(sf::FloatRect(0, 0, (float) windowWidth, (float) windowHeight));
	fpsText.setPosition((float) windowWidth - fpsText.getLocalBounds().width, 0);
}
