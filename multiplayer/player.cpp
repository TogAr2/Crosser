#include <logger.hpp>
#include "player.hpp"
#include "../game/game.hpp"

Player::Player(crs::Location *location, const sf::Color &color, const int &id, const bool &remote) : location(location), color(color), id(id), remote(remote) {}

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

Player::Player(const Player &player): location(player.location), color(player.color), id(player.id), remote(player.remote) {}

void Player::draw(sf::RenderWindow *window, const float &alpha) {
	Game* game = Game::get();

	sf::RectangleShape shape(sf::Vector2f(blockSize, blockSize));
	shape.setPosition((float) location->getX() * blockSize, (float) location->getY() * blockSize);

	if (game->playersMoving[id] == crs::UP) {
		shape.move(0, (-1 * blockSize * alpha) + blockSize);
	} else if (game->playersMoving[id] == crs::DOWN) {
		shape.move(0, (blockSize * alpha) - blockSize);
	} else if (game->playersMoving[id] == crs::LEFT) {
		shape.move((-1 * blockSize * alpha) + blockSize, 0);
	} else if (game->playersMoving[id] == crs::RIGHT) {
		shape.move((blockSize * alpha) - blockSize, 0);
	}

	shape.setFillColor(color);

	window->draw(shape);
}

int Player::getId() const {
	return id;
}

bool Player::isRemote() const {
	return remote;
}

RemotePlayer *Player::getRemote() const {
	if (remote) {
		return (RemotePlayer*) this;
	}

	return nullptr;
}

void Player::setId(int id) {
	Player::id = id;
}

void Player::setColor(const sf::Color &color) {
	Player::color = color;
}

RemotePlayer::RemotePlayer(crs::Location *location, const sf::Color &color, const int &id, sf::TcpSocket* socket) : Player(location, color, id, true), socket(socket) {}

sf::TcpSocket *RemotePlayer::getSocket() const {
	return socket;
}
