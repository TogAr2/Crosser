#ifndef CROSSER_GAME_HPP
#define CROSSER_GAME_HPP

#include <iostream>
#include <random>
#include <list>

#include "config.hpp"
#include "tile.hpp"

const std::string explanation[height] = {
		"",
		"Welcome to Crosser v" + (std::string) CROSSER_VERSION,
		"",
		"Use WASD to move.",
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

class Game {
	std::mt19937 random;

public:
	Tile map[width][height];
	Direction direction;
	Direction moving;

private:
	bool gameOver;
	int x{}, y{}, fruitX{}, fruitY{}, score;

	sf::RenderWindow* window;
	sf::Font mainFont;

	void addRandomObstacle();
	bool isObstacle(int locX, int locY);
	void setPlayerInMap(int locX, int locY);
	void randomFruitLocation();

	int moveCount = 0;
	bool bfs();

public:
	explicit Game(sf::RenderWindow* window);

	void draw(float alpha);
	void input();
	void logic();

	[[nodiscard]] bool isGameOver() const;
	[[nodiscard]] int getScore() const;
	[[nodiscard]] sf::Font getMainFont() const;
};

#endif //CROSSER_GAME_HPP
