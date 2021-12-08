#include <logger.hpp>
#include "player.hpp"
#include "../game/game.hpp"

std::shared_ptr<sf::Texture> Player::texture = std::shared_ptr<sf::Texture>();

Player::Player(crs::Location *location, const sf::Color &color, const int &id, const bool &remote) : location(location), color(color), id(id), remote(remote), lastMoveTime(0) {
	if (texture == nullptr) {
		texture = std::shared_ptr<sf::Texture>(new sf::Texture());
		if (!texture->loadFromFile("resources/image.png")) {
			std::cout << Logger::error << "Failed to load texture" << std::endl;
		}
		texture->setSmooth(false);
	}
}

const sf::Color &Player::getColor() const {
	return color;
}

crs::Location* &Player::getLocation() {
	return location;
}

void Player::setLocation(crs::Location *location) {
	delete Player::location;
	Player::location = location;
}

Player::~Player() {
	delete Player::location;
}

Player::Player(const Player &player): location(player.location), color(player.color), id(player.id), remote(player.remote), lastMoveTime(player.lastMoveTime) {};

void Player::update() {
	lastMoveTime++;
}

void Player::draw(sf::RenderWindow *window, const float &alpha) {
	Game* game = Game::get();

	//sf::Sprite shape(*texture);
	//shape.setScale(40, 40);
	sf::RectangleShape shape(sf::Vector2f(blockSize, blockSize));
	shape.setPosition((float) location->getX() * blockSize, (float) location->getY() * blockSize);

	if (game->playersMoving[id] != crs::STOP) {
		float gamma = alpha / 2 + (float) lastMoveTime / 2;

		if (game->playersMoving[id] == crs::UP) {
			shape.move(0, (-1 * blockSize * gamma) + blockSize);
		} else if (game->playersMoving[id] == crs::DOWN) {
			shape.move(0, (blockSize * gamma) - blockSize);
		} else if (game->playersMoving[id] == crs::LEFT) {
			shape.move((-1 * blockSize * gamma) + blockSize, 0);
		} else if (game->playersMoving[id] == crs::RIGHT) {
			shape.move((blockSize * gamma) - blockSize, 0);
		}
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

int Player::getLastMoveTime() const {
	return lastMoveTime;
}

void Player::setLastMoveTime(int lastMoveTime) {
	Player::lastMoveTime = lastMoveTime;
}

RemotePlayer::RemotePlayer(crs::Location *location, const sf::Color &color, const int &id, sf::TcpSocket* socket) : Player(location, color, id, true), socket(socket) {}

RemotePlayer::~RemotePlayer() {
	delete socket;
}

sf::TcpSocket *RemotePlayer::getSocket() const {
	return socket;
}
