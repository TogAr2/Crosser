#include <SFML/Graphics/Text.hpp>
#include "hud.hpp"
#include "render.hpp"

Hud::Hud(const sf::Font &font) {
	gameOverText = sf::Text();
	gameOverText.setFont(font);
	gameOverText.setCharacterSize(24);

	fpsText = sf::Text();
	fpsText.setFont(font);
	fpsText.setCharacterSize(12);
	fpsText.setString("Fps: 0");
}

void Hud::draw(sf::RenderWindow* &window) {
	window->draw(fpsText);

	Game* game = Render::get()->getGame();
	if (game != nullptr) {
		if (game->isGameOver()) {
			gameOverText.setString("Your score was: " + std::to_string(game->clientPlayer->getScore()));
			window->draw(gameOverText);
		}
	}
}

void Hud::setFps(int fps) {
	fpsText.setString("Fps: " + std::to_string(fps));
	fpsText.setPosition((float) Render::get()->getWindow()->getSize().x - fpsText.getLocalBounds().width, 0);
}

void Hud::adjustSize(unsigned int windowWidth, unsigned int windowHeight) {
	fpsText.setPosition((float) windowWidth - fpsText.getLocalBounds().width, 0);
}
