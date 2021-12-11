#include "logger.hpp"
#include "render.hpp"

Render* Render::instance = nullptr;

Render::Render(sf::RenderWindow* window) : window(window) {
	instance = this;

	if (!mainFont.loadFromFile("/System/Library/Fonts/Courier.dfont")) {
		std::cout << Logger::error << " Could not load font!" << std::endl;
	}

	hud = new Hud(mainFont);

	adjustSize(window->getSize().x, window->getSize().y);
}

Render::~Render() {
	delete hud;
}

sf::RenderWindow* &Render::getWindow() {
	return window;
}

const sf::Font &Render::getMainFont() const {
	return mainFont;
}

Hud* Render::getHud() {
	return hud;
}

Game* Render::getGame() {
	return &game;
}

void Render::setGame(Game &game) {
	Render::game = game;
}

Gui* Render::getGui() {
	return &gui;
}

void Render::setGui(Gui &gui) {
	Render::gui = gui;
}

void Render::draw(float alpha) {
	window->clear(sf::Color(184, 115, 51));

	game.draw(window, alpha);

	window->setView(view);
	gui.draw(window);
	hud->draw(window);

	window->display();
}

void Render::adjustSize(unsigned int windowWidth, unsigned int windowHeight) {
	view.reset(sf::FloatRect(0, 0, (float) windowWidth, (float) windowHeight));

	game.adjustSize(windowWidth, windowHeight);
	hud->adjustSize(windowWidth, windowHeight);
}

void Render::logic() {
	sf::Vector2<int> mouse = sf::Mouse::getPosition(*window);
	sf::Vector2<float> coords = window->mapPixelToCoords(mouse, view);
	gui.update(coords.x, coords.y);

	game.logic();
}

void Render::input() {
	sf::Event event{};
	while (window->pollEvent(event)) {
		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::Escape) {
				window->close();
			}
		} else if (event.type == sf::Event::MouseButtonPressed) {
			sf::Vector2<int> mouse = sf::Vector2i(event.mouseMove.x, event.mouseMove.y);
			sf::Vector2<float> coords = window->mapPixelToCoords(mouse, view);
			gui.onClick(coords.x, coords.y);
		} else if (event.type == sf::Event::Closed) {
			window->close();
		} else if (event.type == sf::Event::Resized) {
			adjustSize(event.size.width, event.size.height);
		}

		game.onInput(event);
	}
}

Render* Render::get() {
	return instance;
}
