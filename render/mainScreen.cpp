#include "mainScreen.hpp"
#include "renderUtils.hpp"
#include "render.hpp"

#include <iostream>

void MainScreen::init(const sf::Font &font, unsigned int windowWidth, unsigned int windowHeight) {
	bool firstInit = !initialized;
	Gui::init(font, windowWidth, windowHeight);

	sf::Color whiteTransparent = sf::Color(255, 255, 255, 0);

	sf::Text crosserText = sf::Text("Crosser", font, 96);
	crosserText.setFillColor(whiteTransparent);
	sf::FloatRect textBounds = crosserText.getLocalBounds();
	crosserText.setPosition((float) windowWidth / 2 - textBounds.width / 2, (float) windowHeight / (firstInit ? 2.f : 4.f) - textBounds.height / 2);
	crosserTextElement = new TextGuiElement(crosserText);
	elements.emplace_back(crosserTextElement);

	sf::RectangleShape background = sf::RectangleShape();
	background.setPosition((float) windowWidth / 2 - 100, (float) windowHeight / 2 - 30);
	background.setSize(sf::Vector2f(200, 60));
	background.setFillColor(sf::Color(0, 255, 0, 0));
	sf::Text playText = sf::Text("Play Game", font, 24);
	playText.setFillColor(whiteTransparent);
	textBounds = playText.getLocalBounds();
	playText.setPosition((float) windowWidth / 2 - textBounds.width / 2, (float) windowHeight / 2 - textBounds.height / 2 - 8);
	playButton = new ButtonGuiElement(background, playText, [](sf::Mouse::Button button){
		if (button == sf::Mouse::Button::Left) {
			Render::get()->setGui(new Gui());
			Render::get()->setGame(new Game());
		}
	});
	elements.emplace_back(playButton);

	sf::RectangleShape background2 = sf::RectangleShape();
	background2.setPosition((float) windowWidth / 2 - 100, (float) windowHeight / 2 + 45);
	background2.setSize(sf::Vector2f(200, 60));
	background2.setFillColor(sf::Color(0, 255, 0, 0));
	sf::Text multiplayerText = sf::Text("Multiplayer", font, 24);
	multiplayerText.setFillColor(whiteTransparent);
	textBounds = multiplayerText.getLocalBounds();
	multiplayerText.setPosition((float) windowWidth / 2 - textBounds.width / 2, (float) windowHeight / 2 - textBounds.height / 2 - 8 + 75);
	multiplayerButton = new ButtonGuiElement(background2, multiplayerText);
	elements.emplace_back(multiplayerButton);
}

void MainScreen::update(float mouseX, float mouseY) {
	Gui::update(mouseX, mouseY);
	animationProgress++;
}

void MainScreen::draw(sf::RenderWindow* &window, float alpha) const {
	sf::Text* crosserText = crosserTextElement->getText();
	sf::RectangleShape* playButtonBackground = playButton->getBackground();
	sf::Text* playButtonText = playButton->getText();
	sf::RectangleShape* multiplayerBackground = multiplayerButton->getBackground();
	sf::Text* multiplayerText = multiplayerButton->getText();

	if (animationProgress < 20 && animationProgress >= 5) {
		crosserText->setFillColor(sf::Color(255, 255, 255, interpolate(0, 255, alpha / 15 + (float) (animationProgress - 5) / 15)));
	}
	if (animationProgress >= 20) {
		crosserText->setFillColor(sf::Color::White);
	}

	if (animationProgress < 30 && animationProgress >= 20) {
		int opacity = interpolate(0, 255, alpha / 10 + (float) (animationProgress - 20) / 10);
		sf::Color backgroundColor = playButtonBackground->getFillColor();
		sf::Color textColor = playButtonText->getFillColor();
		backgroundColor.a = opacity;
		textColor.a = opacity;
		playButtonBackground->setFillColor(backgroundColor);
		multiplayerBackground->setFillColor(backgroundColor);
		playButtonText->setFillColor(textColor);
		multiplayerText->setFillColor(textColor);
	}
	if (animationProgress >= 30) {
		sf::Color backgroundColor = playButtonBackground->getFillColor();
		sf::Color textColor = playButtonText->getFillColor();
		backgroundColor.a = 255;
		textColor.a = 255;
		playButtonBackground->setFillColor(backgroundColor);
		multiplayerBackground->setFillColor(backgroundColor);
		playButtonText->setFillColor(textColor);
		multiplayerText->setFillColor(textColor);
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
