#include <logger.hpp>
#include <iostream>
#include "player.hpp"

Player::Player(crs::Location *location, const sf::Color &color) : location(location), color(color) {}

const sf::Color &Player::getColor() const {
	return color;
}

crs::Location* &Player::getLocation() {
	return location;
}

void Player::setLocation(crs::Location *location) {
	Player::location = location;
}

Player::~Player() {
	delete Player::location;
}

Player::Player(const Player &player): location(player.location), color(player.color) {}

void Player::draw(sf::RenderWindow *window, const float &alpha, const crs::Direction &moving) {
	sf::RectangleShape shape(sf::Vector2f(blockSize, blockSize));
	shape.setPosition((float) location->getX() * blockSize, (float) location->getY() * blockSize);

	if (moving == crs::UP) {
		shape.move(0, (-1 * blockSize * alpha) + blockSize);
	} else if (moving == crs::DOWN) {
		shape.move(0, (blockSize * alpha) - blockSize);
	} else if (moving == crs::LEFT) {
		shape.move((-1 * blockSize * alpha) + blockSize, 0);
	} else if (moving == crs::RIGHT) {
		shape.move((blockSize * alpha) - blockSize, 0);
	}

	shape.setFillColor(color);

	window->draw(shape);
}
