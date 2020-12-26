#ifndef CROSSER_PLAYER_HPP
#define CROSSER_PLAYER_HPP

#include <SFML/Graphics/Color.hpp>
#include <location.hpp>
#include <api-utils.hpp>
#include <SFML/Graphics.hpp>
#include "../plugins/utils.hpp"

class Player {
	const sf::Color color{};
	crs::Location* location{};

public:
	Player(crs::Location *location, const sf::Color &color);
	Player(Player const &player);
	~Player();

	void draw(sf::RenderWindow* window, const float& alpha, const crs::Direction& moving);

	crs::Location* &getLocation();
	void setLocation(crs::Location *location);

	[[nodiscard]] const sf::Color &getColor() const;
};

#endif //CROSSER_PLAYER_HPP
