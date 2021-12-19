#include "multiplayerScreen.hpp"
#include "../../multiplayer/network.hpp"
#include "../render.hpp"

void MultiplayerScreen::init(const sf::Font &font, unsigned int windowWidth, unsigned int windowHeight) {
	Gui::init(font, windowWidth, windowHeight);

	sf::Text title = sf::Text("Connect to server", font, 36);
	sf::FloatRect textBounds = title.getLocalBounds();
	title.setPosition((float) windowWidth / 2 - textBounds.width / 2, 30);
	elements.emplace_back(new TextGuiElement(title));

	sf::RectangleShape background = sf::RectangleShape();
	background.setPosition((float) windowWidth / 2 - 250, (float) windowHeight / 3 - 30);
	background.setSize(sf::Vector2f(500, 60));
	background.setFillColor(sf::Color::Black);
	sf::Text text = sf::Text("|", font, 24); // '|' so we can retrieve the maximum height of the text
	textBounds = text.getLocalBounds();
	text.setPosition(background.getPosition().x + 10, (float) windowHeight / 3 - textBounds.height / 2 - 5);
	textField = new TextFieldGuiElement(background, text, "Enter ip address...");
	elements.emplace_back(textField);

	background = sf::RectangleShape();
	background.setPosition((float) windowWidth / 2 - 100, (float) windowHeight / 2 - 30);
	background.setSize(sf::Vector2f(200, 60));
	background.setFillColor(sf::Color::Green);
	text = sf::Text("Connect", font, 24);
	textBounds = text.getLocalBounds();
	text.setPosition((float) windowWidth / 2 - textBounds.width / 2, (float) windowHeight / 2 - textBounds.height / 2 - 8);
	connectButton = new ButtonGuiElement(background, text, [this](sf::Mouse::Button button){
		connectButton->setEnabled(false);
		std::string ip = textField->getEnteredText();

		Game* game = Network::createClient(ip, Network::defaultPort);
		if (game != nullptr) {
			Render::get()->setGui(new Gui());
			Render::get()->setGame(game);
		} else {
			connectButton->setEnabled(true);
		}
	});
	elements.emplace_back(connectButton);
}
