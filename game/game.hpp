#ifndef CROSSER_GAME_HPP
#define CROSSER_GAME_HPP

#include <iostream>
#include <random>
#include <unordered_map>
#include <queue>

#include "config.hpp"
#include "tile.hpp"
#include "api-utils.hpp"
#include "IGame.hpp"
#include "../multiplayer/player.hpp"
#include "hud.hpp"

#include "../plugins/pluginManager.hpp"

class Hud;

class Game : public crs::IGame {
	static Game* instance;

	int lastId = -1;

public:
	std::mt19937 random;

	crs::Direction direction;
	std::unordered_map<int, crs::Direction> playersMoving;
	std::queue<std::pair<Player*, crs::Direction>*> moveRequests;

	std::unordered_map<int, Player*> players;

	Player* clientPlayer;

	Tile map[width][height];

private:
	bool gameOver;
	crs::Location* fruitLocation = nullptr;
	int score;
	float zoom = 0.1;
	float previousZoom = 0.1;
	int waitBeforeZoom = 0;
	int maxWaitBeforeZoom = 3;

	sf::RenderWindow* window;
	sf::Font* mainFont;
	Hud* hud = nullptr;

	void addRandomObstacle();
	bool isObstacle(int locX, int locY);
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
	[[nodiscard]] sf::Font* getMainFont();

	void setZoom(float zoom);
	void setFruitLocation(crs::Location *location);
	bool controlPlayer(crs::Direction moveDirection);
	void requestMove(Player* player, crs::Direction moveDirection);
	void movePlayer(Player* player, crs::Direction moveDirection);

	void setFps(int fps);

	void setTileType(const crs::Location& location, crs::TileType type);
	crs::TileType getTileType(const crs::Location& location);

	static Game* get();

	crs::Location *getFruitLocation() const;

	Player* newPlayer(crs::Location *location, const sf::Color &color, sf::TcpSocket* socket = nullptr);
};

#endif //CROSSER_GAME_HPP
