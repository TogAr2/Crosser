#include "mainScreen.hpp"
#include "../renderUtils.hpp"
#include "../render.hpp"
#include "multiplayerScreen.hpp"

#include <iostream>

void MainScreen::init(const sf::Font &font, unsigned int windowWidth, unsigned int windowHeight) {
	bool firstInit = !initialized;
	Gui::init(font, windowWidth, windowHeight);

	sf::Color whiteTransparent = sf::Color(255, 255, 255, 0);

	sf::Text crosserText = sf::Text("Crosser", font, 96);
	sf::FloatRect textBounds = crosserText.getLocalBounds();
	crosserText.setPosition((float) windowWidth / 2 - textBounds.width / 2, (float) windowHeight / (firstInit ? 2.f : 4.f) - textBounds.height / 2);
	crosserTextElement = new TextGuiElement(crosserText);
	crosserTextElement->setAlpha(0);
	elements.emplace_back(crosserTextElement);

	sf::RectangleShape background = sf::RectangleShape();
	background.setPosition((float) windowWidth / 2 - 100, (float) windowHeight / 2 - 30);
	background.setSize(sf::Vector2f(200, 60));
	background.setFillColor(sf::Color(0, 255, 0, 0));
	sf::Text playText = sf::Text("Play Game", font, 24);
	textBounds = playText.getLocalBounds();
	playText.setPosition((float) windowWidth / 2 - textBounds.width / 2, (float) windowHeight / 2 - textBounds.height / 2 - 8);
	playButton = new ButtonGuiElement(background, playText, [](sf::Mouse::Button button){
		if (button == sf::Mouse::Button::Left) {
			Render::get()->setGui(new Gui());
			Render::get()->setGame(new Game(false));
		}
	});
	playButton->setAlpha(0);
	elements.emplace_back(playButton);

	background = sf::RectangleShape();
	background.setPosition((float) windowWidth / 2 - 100, (float) windowHeight / 2 + 45);
	background.setSize(sf::Vector2f(200, 60));
	background.setFillColor(sf::Color(0, 255, 0, 0));
	sf::Text multiplayerText = sf::Text("Multiplayer", font, 24);
	textBounds = multiplayerText.getLocalBounds();
	multiplayerText.setPosition((float) windowWidth / 2 - textBounds.width / 2, (float) windowHeight / 2 - textBounds.height / 2 - 8 + 75);
	multiplayerButton = new ButtonGuiElement(background, multiplayerText, [](sf::Mouse::Button button){
		if (button == sf::Mouse::Button::Left) {
			Render::get()->setGui(new MultiplayerScreen());
		}
	});
	multiplayerButton->setAlpha(0);
	elements.emplace_back(multiplayerButton);
}

void MainScreen::update(float mouseX, float mouseY) {
	Gui::update(mouseX, mouseY);
	animationProgress++;
}

void MainScreen::onKeyPress(sf::Event::KeyEvent event) {
	Gui::onKeyPress(event);

	if (event.alt && event.code == sf::Keyboard::R) {
		animationProgress = 0;

		crosserTextElement->setAlpha(0);
		sf::Vector2f position = crosserTextElement->getText()->getPosition();
		sf::FloatRect textBounds = crosserTextElement->getText()->getLocalBounds();
		float halfHeight = textBounds.height / 2;
		crosserTextElement->getText()->setPosition(position.x, (position.y + halfHeight) * 2 - halfHeight);
		playButton->setAlpha(0);
		multiplayerButton->setAlpha(0);
	}
}

void MainScreen::draw(sf::RenderWindow* &window, float alpha) const {
	sf::Text* crosserText = crosserTextElement->getText();

	if (animationProgress < 20 && animationProgress >= 5) {
		crosserTextElement->setAlpha(interpolate(0, 255, alpha / 15 + (float) (animationProgress - 5) / 15));
	}
	if (animationProgress >= 20) {
		crosserTextElement->setAlpha(255);
	}

	if (animationProgress < 20) {
		playButton->setAlpha(0);
		multiplayerButton->setAlpha(0);
	}
	if (animationProgress < 30 && animationProgress >= 20) {
		int opacity = interpolate(0, 255, alpha / 10 + (float) (animationProgress - 20) / 10);
		playButton->setAlpha(opacity);
		multiplayerButton->setAlpha(opacity);
	}
	if (animationProgress >= 30) {
		playButton->setAlpha(255);
		multiplayerButton->setAlpha(255);
	}

	float windowHeight = (float) window->getSize().y;
	float textHeight = crosserText->getLocalBounds().height;
	float titleTargetY = windowHeight / 4 - textHeight / 2;
	if (animationProgress < 30 && animationProgress >= 15) {
		float titleStartY = windowHeight / 2 - textHeight / 2;
		float y = interpolate(titleStartY, titleTargetY, alpha / 15 + (float) (animationProgress - 15) / 15);
		crosserText->setPosition(crosserText->getPosition().x, y);
	}
	if (animationProgress >= 30) {
		crosserText->setPosition(crosserText->getPosition().x, titleTargetY);
	}

	Gui::draw(window, alpha);
}
