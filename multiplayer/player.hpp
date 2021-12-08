#ifndef CROSSER_PLAYER_HPP
#define CROSSER_PLAYER_HPP

#include <SFML/Network.hpp>
#include <SFML/Graphics/Color.hpp>
#include <location.hpp>
#include <api-utils.hpp>
#include <SFML/Graphics.hpp>
#include "../plugins/utils.hpp"
#include "unordered_map"

class RemotePlayer;

class Player {
	static std::shared_ptr<sf::Texture> texture;

	const bool remote;
	int id;
	sf::Color color{};
	crs::Location* location{};

	int lastMoveTime;
	int score;

public:
	Player(crs::Location *location, const sf::Color &color, const int &id, const bool &remote = false);
	Player(Player const &player);
	~Player();

	void update();
	void draw(sf::RenderWindow* window, const float& alpha);

	crs::Location* &getLocation();
	void setLocation(crs::Location *location);

	[[nodiscard]] const sf::Color &getColor() const;
	void setColor(const sf::Color &color);

	[[nodiscard]] int getId() const;
	void setId(int id);

	[[nodiscard]] bool isRemote() const;
	[[nodiscard]] RemotePlayer *getRemote() const;

	int getLastMoveTime() const;
	void setLastMoveTime(int lastMoveTime);

	int getScore() const;
	virtual void setScore(int score);
};

class RemotePlayer : public Player {
	sf::TcpSocket* socket;

public:
	RemotePlayer(crs::Location *location, const sf::Color &color, const int &id, sf::TcpSocket* socket);
	~RemotePlayer();

	[[nodiscard]] sf::TcpSocket *getSocket() const;

	void setScore(int score) override;
};

#endif //CROSSER_PLAYER_HPP
