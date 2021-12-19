#ifndef CROSSER_GAME_HPP
#define CROSSER_GAME_HPP

#include <iostream>
#include <random>
#include <unordered_map>
#include <queue>

#include "tile.hpp"
#include "api-utils.hpp"
#include "IGame.hpp"
#include "../multiplayer/player.hpp"
#include "../plugins/pluginManager.hpp"
#include "../multiplayer/network.hpp"

class Network;

class Game : public crs::IGame {
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
	Network* network = nullptr;
	bool client;

	bool gameOver;
	crs::Location fruitLocation = crs::Location(width / 2, height / 2);
	float zoom = 0.1;
	float previousZoom = 0.1;
	int waitBeforeZoom = 0;
	int maxWaitBeforeZoom = 3;

	sf::View view;

	void addRandomObstacle();
	bool isObstacle(int locX, int locY);
	crs::Location randomFruitLocation();

	int moveCount = 0;
	bool bfs();

public:
	explicit Game(bool client);
	~Game();

	void draw(sf::RenderWindow* &window, float alpha);
	void onInput(const sf::Event &event);
	void logic();

	[[nodiscard]] bool isGameOver() const;

	void setZoom(float zoom);
	void setFruitLocation(const crs::Location &location);
	void controlPlayer(crs::Direction moveDirection);
	void requestMove(Player* player, crs::Direction moveDirection);
	void movePlayer(Player* player, crs::Direction moveDirection);

	void adjustSize(unsigned int windowWidth, unsigned int windowHeight);

	void setTileType(const crs::Location& location, crs::TileType type);
	crs::TileType getTileType(const crs::Location& location);

	Player* newPlayer(crs::Location *location, const sf::Color &color, sf::TcpSocket* socket = nullptr);

	Network *getNetwork() const;
	void setNetwork(Network *network);

	bool isClient();
	bool isServer();
	bool isInitialized();
};

#endif //CROSSER_GAME_HPP
