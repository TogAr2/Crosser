#ifndef CROSSER_GAME_HPP
#define CROSSER_GAME_HPP

#include <iostream>
#include <random>
#include <list>

#include "config.hpp"
#include "tile.hpp"
#include "api-utils.hpp"
#include "IGame.hpp"

#include "pluginManager.hpp"

const std::string explanation[height] = {
		"",
		"Welcome to Crosser v" + (std::string) CROSSER_VERSION,
		"",
		"Use WASD to movePlayer.",
		"",
		"Search for the cross.",
		"",
		"Press M if you are a lazy",
		"person :/",
		"",
		"Good luck!",
		"",
		"",
		"",
		"Press Q to quit.",
		"",
		"",
		"",
		"",
		""
};

class Game : public crs::IGame {
	std::mt19937 random;

public:
	crs::Direction direction;
	crs::Direction moving;

private:
	Tile map[width][height];

	bool gameOver;
	crs::Location* fruitLocation = nullptr;
	crs::Location* playerLocation = nullptr;
	int score;
	float zoom = 0.1;
	float previousZoom = 0.1;
	int waitBeforeZoom = 0;
	int maxWaitBeforeZoom = 3;

	sf::RenderWindow* window;
	sf::Font mainFont;

	void addRandomObstacle();
	bool isObstacle(int locX, int locY);
	void setPlayerInMap(crs::Location* location);
	crs::Location *randomFruitLocation();

	int moveCount = 0;
	bool bfs();

public:
	explicit Game(sf::RenderWindow* window);
	~Game();

	void draw(float alpha);
	void input();
	void logic();

	[[nodiscard]] bool isGameOver() const;
	[[nodiscard]] int getScore() const;
	[[nodiscard]] sf::Font getMainFont() const;

	void setZoom(float zoom);
	void setFruitLocation(crs::Location *location);
	bool movePlayer(crs::Direction moveDirection);

	void setTileType(const crs::Location& location, crs::TileType type);
	crs::TileType getTileType(const crs::Location& location);
};

#endif //CROSSER_GAME_HPP
