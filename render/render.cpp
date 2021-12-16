#include "logger.hpp"
#include "render.hpp"
#include "mainScreen.hpp"

Render* Render::instance = nullptr;

Render::Render(sf::RenderWindow* window) : window(window) {
	instance = this;

	if (!mainFont.loadFromFile("/System/Library/Fonts/Courier.dfont")) {
		std::cout << Logger::error << " Could not load font!" << std::endl;
	}

	hud = new Hud(mainFont);
	gui = new MainScreen();

	adjustSize(window->getSize().x, window->getSize().y);
}

Render::~Render() {
	delete hud;
	delete gui;
}

sf::RenderWindow* &Render::getWindow() {
	return window;
}

sf::Font Render::getMainFont() const {
	return mainFont;
}

Hud* Render::getHud() {
	return hud;
}

std::optional<Game>* Render::getGame() {
	return &game;
}

void Render::setGame(Game* game) {
	Render::game = *game;
	resizeQueued = true;
}

Gui* Render::getGui() {
	return gui;
}

void Render::setGui(Gui* gui) {
	delete Render::gui;
	Render::gui = gui;
}

void Render::draw(float alpha) {
	if (resizeQueued) {
		adjustSize(window->getSize().x, window->getSize().y);
		resizeQueued = false;
	}

	window->clear(sf::Color::Black);

	if (game.has_value())
		game->draw(window, alpha);

	window->setView(view);
	gui->draw(window, alpha);
	hud->draw(window);

	window->display();
}

void Render::adjustSize(unsigned int windowWidth, unsigned int windowHeight) {
	view.reset(sf::FloatRect(0, 0, (float) windowWidth, (float) windowHeight));

	if (game.has_value())
		game->adjustSize(windowWidth, windowHeight);
	gui->init(mainFont, windowWidth, windowHeight);
	hud->adjustSize(windowWidth, windowHeight);
}

void Render::logic() {
	sf::Vector2<int> mouse = sf::Mouse::getPosition(*window);
	sf::Vector2<float> coords = window->mapPixelToCoords(mouse, view);
	gui->update(coords.x, coords.y);

	if (game.has_value())
		game->logic();
}

void Render::input() {
	sf::Event event{};
	while (window->pollEvent(event)) {
		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::Escape) {
				window->close();
			}
		} else if (event.type == sf::Event::MouseButtonPressed) {
			sf::Vector2<int> mouse = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
			sf::Vector2<float> coords = window->mapPixelToCoords(mouse, view);
			gui->onClick(coords.x, coords.y, event.mouseButton.button);
		} else if (event.type == sf::Event::Closed) {
			window->close();
		} else if (event.type == sf::Event::Resized) {
			adjustSize(event.size.width, event.size.height);
		}

		if (game.has_value())
			game->onInput(event);
	}
}

Render* Render::get() {
	return instance;
}
