#include "game.hpp"
#include "logger.hpp"
#include "../multiplayer/network.hpp"
#include "currentGame.hpp"

#include <SFML/Graphics.hpp>

void Game::addRandomObstacle() {
	std::uniform_int_distribution<int> widthDistribution(0, width);
	std::uniform_int_distribution<int> heightDistribution(0, height);

	int obstacleX = widthDistribution(random);
	int obstacleY = heightDistribution(random);

	if (map[obstacleX][obstacleY].getType() == crs::AIR) {
		map[obstacleX][obstacleY].setType(crs::OBSTACLE);
	} else {
		addRandomObstacle();
	}
}

bool Game::isObstacle(int locX, int locY) {
	return map[locX][locY].getType() == crs::OBSTACLE;
}

crs::Location Game::randomFruitLocation() {
	sf::Vector2<float> viewSize = view.getSize();
	sf::Vector2<float> viewOrigin = sf::Vector2<float>(view.getCenter().x - (viewSize.x / 2), view.getCenter().y - (viewSize.y / 2));

	std::uniform_int_distribution<int> widthDistribution(viewOrigin.x, viewOrigin.x + viewSize.x - 1);
	std::uniform_int_distribution<int> heightDistribution(viewOrigin.y, viewOrigin.y + viewSize.y - 1);

	int newFruitX = widthDistribution(random) / blockSize;
	int newFruitY = heightDistribution(random) / blockSize;

	if (map[newFruitX][newFruitY].getType() != crs::AIR) {
		return randomFruitLocation();
	}

	return {newFruitX, newFruitY};
}

bool Game::bfs() {
	/*
	 * SETUP
	 */

	bool visited[width][height];
	for (auto & i : visited) {
		for (bool & j : i)
			j = false;
	}

	moveCount = 0;
	int nodesLeftInLayer = 1;
	int nodesInNextLayer = 0;

	PathfinderTile* finishTile = nullptr;

	bool reachedFinish = false;

	std::list<PathfinderTile> queue;

	/*
	 * ALGORITHM
	 */

	crs::Location* playerLocation = clientPlayer->getLocation();

	visited[playerLocation->getX()][playerLocation->getY()] = true;
	map[playerLocation->getX()][playerLocation->getY()].setType(crs::VISITED);
	queue.emplace_back(playerLocation->getX(), playerLocation->getY());

	std::list<PathfinderTile>::iterator i;

	while (!queue.empty()) {
		PathfinderTile currentTile = queue.front();
		queue.pop_front();

		if (map[currentTile.getX()][currentTile.getY()].getType() == crs::FRUIT) {
			reachedFinish = true;
			finishTile = &currentTile;
			break;
		}

		std::list<PathfinderTile> adjacentBlocks = currentTile.getAdjacentTiles(this);
		for (i = adjacentBlocks.begin(); i != adjacentBlocks.end(); i++) {
			PathfinderTile adjacentTile = *i;
			if (visited[adjacentTile.getX()][adjacentTile.getY()]) continue;

			visited[adjacentTile.getX()][adjacentTile.getY()] = true;
			if (map[adjacentTile.getX()][adjacentTile.getY()].getType() != crs::FRUIT) {
				map[adjacentTile.getX()][adjacentTile.getY()].setType(crs::VISITED);
			}
			queue.push_back(adjacentTile);

			nodesInNextLayer++;
		}

		nodesLeftInLayer--;
		if (nodesLeftInLayer == 0) {
			nodesLeftInLayer = nodesInNextLayer;
			nodesInNextLayer = 0;
			moveCount++;
		}
	}

	for (auto & loopX : map) {
		for (auto & loopY : loopX) {
			if (loopY.getType() == crs::VISITED) {
				loopY.setType(crs::AIR);
			}
		}
	}

	PathfinderTile* current = finishTile;
	while (!current->isFirst()) {
		map[current->getX()][current->getY()].setType(crs::VISITED);
		current = current->getPrevious();
	}

	std::cout << current->getX() << " " << current->getY() << std::endl;

	return reachedFinish;
}

Game::Game() { // NOLINT(cert-msc51-cpp)
	gameOver = false;
	direction = crs::STOP;

	random.seed(time(nullptr) * 5);

	for (int loopX = 0; loopX < width; loopX++) {
		for (int loopY = 0; loopY < height; loopY++) {
			map[loopX][loopY].setX(loopX);
			map[loopX][loopY].setY(loopY);
		}
	}

	view.setCenter(mapSize / 2, mapSize / 2);

	clientPlayer = newPlayer(new crs::Location(width / 2, height / 2), sf::Color::Cyan);
	players[clientPlayer->getId()] = clientPlayer;
	playersMoving[clientPlayer->getId()] = crs::STOP;

	map[fruitLocation.getX()][fruitLocation.getY()].setType(crs::FRUIT);

	for (int i = 0; i < 60; i++) {
		addRandomObstacle();
	}
}

void Game::draw(sf::RenderWindow* &window, float alpha) {
	if (!gameOver && zoom != previousZoom) {
		//Interpolate zoom
		float framesPassed;
		if (waitBeforeZoom != 0) {
			framesPassed = (float) (maxWaitBeforeZoom - waitBeforeZoom) + alpha;
		} else {
			framesPassed = (float) maxWaitBeforeZoom;
		}
		float newZoom = zoom + ((zoom - previousZoom) * framesPassed / (float) maxWaitBeforeZoom) - (zoom - previousZoom);
		adjustSize(window->getSize().x, window->getSize().y);
		//view->zoom(newZoom);
	}

	window->setView(view);

	sf::RectangleShape shape = sf::RectangleShape(sf::Vector2f(mapSize, mapSize));
	shape.setFillColor(sf::Color::Black);
	window->draw(shape);

	for (auto & loopX : map) {
		for (auto & loopY : loopX) {
			loopY.draw(window);
		}
	}

	for (auto & pair : players) {
		pair.second->draw(window, alpha);
	}
}

void Game::onInput(const sf::Event &event) {
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up) {
			direction = crs::UP;
		} else if (event.key.code == sf::Keyboard::A || event.key.code == sf::Keyboard::Left) {
			direction = crs::LEFT;
		} else if (event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down) {
			direction = crs::DOWN;
		} else if (event.key.code == sf::Keyboard::D || event.key.code == sf::Keyboard::Right) {
			direction = crs::RIGHT;
		} else if (event.key.code == sf::Keyboard::M) {
			direction = crs::AUTO;
		} else if (event.key.code == sf::Keyboard::Q || event.key.code == sf::Keyboard::Escape) {
			if (!gameOver) {
				crs::Event *crsEvent = new crs::QuitEvent();
				auto *quitEvent = (crs::QuitEvent *) crsEvent;
				PluginManager::instance.broadcastEvent(crsEvent);

				gameOver = !quitEvent->isCancelled();

				if (gameOver && Network::client) {
					Network::disconnect();
				}
				if (gameOver && Network::serverUp) {
					Network::serverUp = false;
				}
			}
		} else if (event.key.code == sf::Keyboard::B) {
			std::cout << Logger::info << "Player: " << clientPlayer->getLocation()->getX() << " "
					  << clientPlayer->getLocation()->getY() << std::endl;
			std::cout << Logger::info << "Fruit: " << fruitLocation.getX() << " " << fruitLocation.getY()
					  << std::endl;
		} else if (event.key.code == sf::Keyboard::U) {
			setZoom(zoom + 0.1f);
		} else if (event.key.code == sf::Keyboard::K) {
			Network::startServer();
		} else if (event.key.code == sf::Keyboard::L) {
			Network::connect();
		}
	} else if (event.type == sf::Event::KeyReleased) {
		if (direction == crs::UP && (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up)) {
			direction = crs::STOP;
		} else if (direction == crs::LEFT && (event.key.code == sf::Keyboard::A || event.key.code == sf::Keyboard::Left)) {
			direction = crs::STOP;
		} else if (direction == crs::DOWN && (event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down)) {
			direction = crs::STOP;
		} else if (direction == crs::RIGHT && (event.key.code == sf::Keyboard::D || event.key.code == sf::Keyboard::Right)) {
			direction = crs::STOP;
		} else if (direction == crs::AUTO && event.key.code == sf::Keyboard::M) {
			direction = crs::STOP;
		}
	}
}

void Game::logic() {
	if (gameOver) return;

	if (waitBeforeZoom == 0) {
		previousZoom = zoom;
	} else {
		waitBeforeZoom--;
	}

	for (auto &pair : players) {
		pair.second->update();
		if (pair.second->getLastMoveTime() >= 2) {
			playersMoving[pair.first] = crs::STOP;
		}
	}

	if (Network::client) {
		Network::receive();
	} else {
		while (!moveRequests.empty()) {
			std::pair<Player*, crs::Direction>* pair = moveRequests.front();
			movePlayer(pair->first, pair->second);
			moveRequests.pop();
			delete pair;
		}
	}

	controlPlayer(direction);
}

[[nodiscard]] bool Game::isGameOver() const {
	return gameOver;
}

void Game::setZoom(float newZoom) {
	if (newZoom <= 1.1 && newZoom >= 0) {
		previousZoom = zoom;
		zoom = newZoom;
		waitBeforeZoom = maxWaitBeforeZoom;
	}
}

Game::~Game() {
	for (auto & pair : players) {
		delete pair.second;
	}
}

void Game::setFruitLocation(const crs::Location& location) {
	bool send = Network::serverUp;

	map[fruitLocation.getX()][fruitLocation.getY()].setType(crs::AIR);
	if (send) Network::sendTileUpdate(fruitLocation, crs::AIR);

	fruitLocation = location;
	if (map[fruitLocation.getX()][fruitLocation.getY()].getType() != crs::AIR) {
		fruitLocation = randomFruitLocation();
		std::cerr << Logger::warning << "Cross placed on not-air tile by plugin. Location changed." << std::endl;
	}
	map[fruitLocation.getX()][fruitLocation.getY()].setType(crs::FRUIT);
	if (send) Network::sendTileUpdate(fruitLocation, crs::FRUIT);
}

void Game::controlPlayer(crs::Direction moveDirection) {
	if (clientPlayer->getLastMoveTime() < 2) return;
	if (direction == crs::STOP) return;

	int fruitX = fruitLocation.getX();
	int fruitY = fruitLocation.getY();

	int x = clientPlayer->getLocation()->getX();
	int y = clientPlayer->getLocation()->getY();

	if (moveDirection == crs::AUTO) {
		bfs();

		if (x > fruitX) {
			if (fruitX - x < (0 - width)/2) {
				moveDirection = crs::RIGHT;
			} else {
				moveDirection = crs::LEFT;
			}
		} else if (x < fruitX) {
			if (fruitX - x > width/2) {
				moveDirection = crs::LEFT;
			} else {
				moveDirection = crs::RIGHT;
			}
		} else if (y > fruitY) {
			if (fruitY - y < (0 - height)/2) {
				moveDirection = crs::DOWN;
			} else {
				moveDirection = crs::UP;
			}
		} else if (y < fruitY) {
			if (fruitY - y > height/2) {
				moveDirection = crs::UP;
			} else {
				moveDirection = crs::DOWN;
			}
		}
	}

	switch (moveDirection) {
		case crs::UP:
			if (y > 0) {
				if (!isObstacle(x, y - 1)) {
					playersMoving[clientPlayer->getId()] = crs::UP;
				} else {
					playersMoving[clientPlayer->getId()] = crs::STOP;
				}
			} else {
				playersMoving[clientPlayer->getId()] = crs::UP;
			}
			break;
		case crs::LEFT:
			if (x > 0) {
				if (!isObstacle(x - 1, y)) {
					playersMoving[clientPlayer->getId()] = crs::LEFT;
				} else {
					playersMoving[clientPlayer->getId()] = crs::STOP;
				}
			} else {
				playersMoving[clientPlayer->getId()] = crs::LEFT;
			}
			break;
		case crs::DOWN:
			if (y < height - 1) {
				if (!isObstacle(x, y + 1)) {
					playersMoving[clientPlayer->getId()] = crs::DOWN;
				} else {
					playersMoving[clientPlayer->getId()] = crs::STOP;
				}
			} else {
				playersMoving[clientPlayer->getId()] = crs::DOWN;
			}
			break;
		case crs::RIGHT:
			if (x < width - 1) {
				if (!isObstacle(x + 1, y)) {
					playersMoving[clientPlayer->getId()] = crs::RIGHT;
				} else {
					playersMoving[clientPlayer->getId()] = crs::STOP;
				}
			} else {
				playersMoving[clientPlayer->getId()] = crs::RIGHT;
			}
			break;
		default:
			break;
	}

	if (playersMoving[clientPlayer->getId()] != crs::STOP) {
		movePlayer(clientPlayer, playersMoving[clientPlayer->getId()]);
		if (Network::client || Network::serverUp) {
			Network::sendMove(playersMoving[clientPlayer->getId()]);
		}
	}
}

void Game::requestMove(Player* player, crs::Direction moveDirection) {
	auto* pair = new std::pair(player, moveDirection);
	moveRequests.push(pair);
}

void Game::movePlayer(Player* player, crs::Direction moveDirection) {
	if (moveDirection == crs::STOP) return;

	int x = player->getLocation()->getX();
	int y = player->getLocation()->getY();

	crs::Location* newLocation = nullptr;

	switch (moveDirection) {
		case crs::UP:
			if (y > 0) {
				if (!isObstacle(x, y - 1)) {
					newLocation = new crs::Location(x, y - 1);
				}
			} else {
				newLocation = new crs::Location(x, height - 1);
			}
			break;
		case crs::LEFT:
			if (x > 0) {
				if (!isObstacle(x - 1, y)) {
					newLocation = new crs::Location(x - 1, y);
				}
			} else {
				newLocation = new crs::Location(width - 1, y);
			}
			break;
		case crs::DOWN:
			if (y < height - 1) {
				if (!isObstacle(x, y + 1)) {
					newLocation = new crs::Location(x, y + 1);
				}
			} else {
				newLocation = new crs::Location(x, 0);
			}
			break;
		case crs::RIGHT:
			if (x < width - 1) {
				if (!isObstacle(x + 1, y)) {
					newLocation = new crs::Location(x + 1, y);
				}
			} else {
				newLocation = new crs::Location(0, y);
			}
			break;
		default:
			break;
	}

	if (newLocation == nullptr) {
		newLocation = new crs::Location(x, y);
	}

	crs::Event* event = new crs::MoveEvent(moveDirection, newLocation);
	auto* moveEvent = (crs::MoveEvent*) event;
	PluginManager::instance.broadcastEvent(event);

	if (moveEvent->isCancelled()) {
		delete newLocation;
		delete event;
		return;
	}
	delete event;

	playersMoving[player->getId()] = moveDirection;
	player->setLocation(newLocation);
	player->setLastMoveTime(0);

	if (!Network::client && player->getLocation()->getX() == fruitLocation.getX() && player->getLocation()->getY() == fruitLocation.getY()) {
		player->setScore(player->getScore() + 1);
		setZoom(zoom + 0.1f);
		crs::Location newFruitLoc = randomFruitLocation();

		crs::Event *event = new crs::CrossFoundEvent(fruitLocation, &newFruitLoc);
		PluginManager::instance.broadcastEvent(event);

		auto* crossFoundEvent = (crs::CrossFoundEvent*) event;
		setFruitLocation(*crossFoundEvent->getNewCrossLocation());
		delete event;
	}
}

void Game::adjustSize(unsigned int windowWidth, unsigned int windowHeight) {
	float viewWidth;
	float viewHeight;
	if (windowWidth > windowHeight) {
		viewWidth = (float) windowWidth / (float) windowHeight * mapSize;
		viewHeight = mapSize;
	} else {
		viewWidth = mapSize;
		viewHeight = (float) windowHeight / (float) windowWidth * mapSize;
	}
	view.setSize(viewWidth, viewHeight);
	//view->zoom(zoom);
}

void Game::setTileType(const crs::Location& location, crs::TileType type) {
	map[location.getX()][location.getY()].setType(type);
}

crs::TileType Game::getTileType(const crs::Location& location) {
	return map[location.getX()][location.getY()].getType();
}

Player *Game::newPlayer(crs::Location *location, const sf::Color &color, sf::TcpSocket* socket) {
	lastId++;
	int id = lastId;

	Player* player;
	if (socket == nullptr) {
		player = new Player(location, color, id);
	} else {
		player = new RemotePlayer(location, color, id, socket);
	}

	players.insert_or_assign(id, player);
	return player;
}
