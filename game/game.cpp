#include "game.hpp"
#include "logger.hpp"
#include "../multiplayer/network.hpp"
#include "currentGame.hpp"

#include <SFML/Graphics.hpp>

Game* Game::instance = nullptr;

void Game::addRandomObstacle() {
	std::uniform_int_distribution<int> widthDistribution(0, width / 2);
	std::uniform_int_distribution<int> heightDistribution(0, height / 2);

	int obstacleX = widthDistribution(random) * 2;
	int obstacleY = heightDistribution(random) * 2;

	if (map[obstacleX][obstacleY].getType() == crs::AIR) {
		map[obstacleX][obstacleY].setType(crs::OBSTACLE);
	} else {
		addRandomObstacle();
	}
}

bool Game::isObstacle(int locX, int locY) {
	return map[locX][locY].getType() == crs::OBSTACLE;
}

crs::Location *Game::randomFruitLocation() {
	sf::Vector2<float> viewSize = window->getView().getSize();
	sf::Vector2<float> viewOrigin = sf::Vector2<float>(window->getView().getCenter().x - (viewSize.x / 2), window->getView().getCenter().y - (viewSize.y / 2));

	std::uniform_int_distribution<int> widthDistribution(viewOrigin.x, viewOrigin.x + viewSize.x - 1);
	std::uniform_int_distribution<int> heightDistribution(viewOrigin.y, viewOrigin.y + viewSize.y - 1);

	int newFruitX = widthDistribution(random) / blockSize;
	int newFruitY = heightDistribution(random) / blockSize;

	if (map[newFruitX][newFruitY].getType() != crs::AIR) {
		return randomFruitLocation();
	}

	return new crs::Location(newFruitX, newFruitY);
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

	draw(0);

	return reachedFinish;
}

Game::Game(sf::RenderWindow* window) { // NOLINT(cert-msc51-cpp)
	instance = this;
	crs::currentGame = this;

	gameOver = false;
	direction = crs::STOP;
	score = 0;
	this->window = window;

	random.seed(time(nullptr) * 5);

	for (int loopX = 0; loopX < width; loopX++) {
		for (int loopY = 0; loopY < height; loopY++) {
			map[loopX][loopY].setX(loopX);
			map[loopX][loopY].setY(loopY);
		}
	}

	sf::View view = sf::View();
	view.setCenter((float) width / 2, (float) height / 2);
	view.reset(sf::FloatRect(0, 0, window->getSize().x, window->getSize().y));
	view.zoom(zoom);
	window->setView(view);

	clientPlayer = newPlayer(new crs::Location(width / 2, height / 2), sf::Color::Cyan);
	players[clientPlayer->getId()] = clientPlayer;
	playersMoving[clientPlayer->getId()] = crs::STOP;

	fruitLocation = randomFruitLocation();
	map[fruitLocation->getX()][fruitLocation->getY()].setType(crs::FRUIT);

	for (int i = 0; i < 30; i++) {
		addRandomObstacle();
	}

	mainFont = new sf::Font();
	if (!mainFont->loadFromFile("/System/Library/Fonts/Helvetica.ttc")) {
		std::cout << Logger::error << " Could not load font!" << std::endl;
	}

	hud = new Hud(this);
}

void Game::draw(float alpha) {
	window->clear(sf::Color::Black);

	sf::View view = sf::View();

	view.setCenter((float) width / 2, (float) height / 2);
	view.reset(sf::FloatRect(0, 0, (float) window->getSize().x, (float) window->getSize().y));

	if (!gameOver) {
		//Interpolate zoom
		float framesPassed;
		if (waitBeforeZoom != 0) {
			framesPassed = (float) (maxWaitBeforeZoom - waitBeforeZoom) + alpha;
		} else {
			framesPassed = (float) maxWaitBeforeZoom;
		}
		float newZoom = zoom + ((zoom - previousZoom) * framesPassed / (float) maxWaitBeforeZoom) - (zoom - previousZoom);
		view.zoom(newZoom);
	} else {
		view.zoom(zoom);
	}

	window->setView(view);

	for (auto & loopX : map) {
		for (auto & loopY : loopX) {
			loopY.draw(window);
		}
	}

	for (auto & pair : players) {
		pair.second->draw(window, alpha);
	}

	hud->draw(window);

	window->display();
}

void Game::input() {
	direction = crs::STOP;

	sf::Event event{};
	while (window->pollEvent(event)) {
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

				if (event.key.code == sf::Keyboard::Escape) {
					window->close();
				}
			} else if (event.key.code == sf::Keyboard::B) {
				std::cout << Logger::info << "Player: " << clientPlayer->getLocation()->getX() << " "
						  << clientPlayer->getLocation()->getY() << std::endl;
				std::cout << Logger::info << "Fruit: " << fruitLocation->getX() << " " << fruitLocation->getY()
						  << std::endl;
			} else if (event.key.code == sf::Keyboard::U) {
				setZoom(zoom + 0.1f);
			} else if (event.key.code == sf::Keyboard::K) {
				Network::startServer();
			} else if (event.key.code == sf::Keyboard::L) {
				Network::connect();
			}
		} else if (event.type == sf::Event::Closed) {
			window->close();
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

	for (auto &pair : playersMoving) {
		pair.second = crs::STOP;
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

	if (Network::client) return;

	for (auto &pair : players) {
		Player* player = pair.second;
		if (player->getLocation()->getX() == fruitLocation->getX() && player->getLocation()->getY() == fruitLocation->getY()) {
			score++;
			setZoom(zoom + 0.1f);
			crs::Location* newFruitLoc = randomFruitLocation();

			crs::Event *event = new crs::CrossFoundEvent(crs::Location(fruitLocation->getX(), fruitLocation->getY()), newFruitLoc);
			PluginManager::instance.broadcastEvent(event);

			setFruitLocation(((crs::CrossFoundEvent *) event)->getNewCrossLocation());
		}
	}
}

[[nodiscard]] bool Game::isGameOver() const {
	return gameOver;
}

[[nodiscard]] int Game::getScore() const {
	return score;
}

[[nodiscard]] sf::Font* Game::getMainFont() {
	return mainFont;
}

void Game::setZoom(float newZoom) {
	if (newZoom <= 1.1 && newZoom >= 0) {
		previousZoom = zoom;
		zoom = newZoom;
		waitBeforeZoom = maxWaitBeforeZoom;
	}
}

Game::~Game() {
	delete fruitLocation;
	delete mainFont;
	delete hud;

	for (auto & pair : players) {
		delete pair.second;
	}

	Game::instance = nullptr;
}

void Game::setFruitLocation(crs::Location *location) {
	bool send = Network::serverUp;

	map[fruitLocation->getX()][fruitLocation->getY()].setType(crs::AIR);
	if (send) Network::sendTileUpdate(*fruitLocation, crs::AIR);
	delete fruitLocation;

	fruitLocation = location;
	if (map[fruitLocation->getX()][fruitLocation->getY()].getType() != crs::AIR) {
		fruitLocation = randomFruitLocation();
		std::cerr << Logger::warning << "Cross placed on not-air tile by plugin. Location changed." << std::endl;
	}
	map[fruitLocation->getX()][fruitLocation->getY()].setType(crs::FRUIT);
	if (send) Network::sendTileUpdate(*fruitLocation, crs::FRUIT);
}

crs::Location *Game::getFruitLocation() const {
	return fruitLocation;
}

bool Game::controlPlayer(crs::Direction moveDirection) {
	if (direction == crs::STOP) return false;

	int fruitX = fruitLocation->getX();
	int fruitY = fruitLocation->getY();

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

	auto* newLocation = new crs::Location(clientPlayer->getLocation()->getX(), clientPlayer->getLocation()->getY());

	switch (moveDirection) {
		case crs::UP:
			if (y > 0) {
				if (!isObstacle(x, y - 1)) {
					playersMoving[clientPlayer->getId()] = crs::UP;
					newLocation = new crs::Location(x, y - 1);
				} else {
					playersMoving[clientPlayer->getId()] = crs::STOP;
				}
			} else {
				playersMoving[clientPlayer->getId()] = crs::UP;
				newLocation = new crs::Location(x, height - 1);
			}
			break;
		case crs::LEFT:
			if (x > 0) {
				if (!isObstacle(x - 1, y)) {
					playersMoving[clientPlayer->getId()] = crs::LEFT;
					newLocation = new crs::Location(x - 1, y);
				} else {
					playersMoving[clientPlayer->getId()] = crs::STOP;
				}
			} else {
				playersMoving[clientPlayer->getId()] = crs::LEFT;
				newLocation = new crs::Location(width - 1, y);
			}
			break;
		case crs::DOWN:
			if (y < height - 1) {
				if (!isObstacle(x, y + 1)) {
					playersMoving[clientPlayer->getId()] = crs::DOWN;
					newLocation = new crs::Location(x, y + 1);
				} else {
					playersMoving[clientPlayer->getId()] = crs::STOP;
				}
			} else {
				playersMoving[clientPlayer->getId()] = crs::DOWN;
				newLocation = new crs::Location(x, 0);
			}
			break;
		case crs::RIGHT:
			if (x < width - 1) {
				if (!isObstacle(x + 1, y)) {
					playersMoving[clientPlayer->getId()] = crs::RIGHT;
					newLocation = new crs::Location(x + 1, y);
				} else {
					playersMoving[clientPlayer->getId()] = crs::STOP;
				}
			} else {
				playersMoving[clientPlayer->getId()] = crs::RIGHT;
				newLocation = new crs::Location(0, y);
			}
			break;
		default:
			break;
	}

	crs::Event* event = new crs::MoveEvent(moveDirection, newLocation);
	auto* moveEvent = (crs::MoveEvent*) event;
	PluginManager::instance.broadcastEvent(event);

	if (moveEvent->isCancelled()) return false;

	clientPlayer->setLocation(moveEvent->getNewLocation());
	if (Network::client || Network::serverUp) {
		Network::sendMove(playersMoving[clientPlayer->getId()]);
	}

	return true;
}

void Game::requestMove(Player* player, crs::Direction moveDirection) {
	auto* pair = new std::pair(player, moveDirection);
	moveRequests.push(pair);
}

void Game::movePlayer(Player* player, crs::Direction moveDirection) {
	if (moveDirection == crs::STOP) return;

	int x = player->getLocation()->getX();
	int y = player->getLocation()->getY();

	playersMoving[player->getId()] = moveDirection;
	switch (moveDirection) {
		case crs::UP:
			player->setLocation(new crs::Location(x, y - 1));
			break;
		case crs::LEFT:
			player->setLocation(new crs::Location(x - 1, y));
			break;
		case crs::DOWN:
			player->setLocation(new crs::Location(x, y + 1));
			break;
		case crs::RIGHT:
			player->setLocation(new crs::Location(x + 1, y));
			break;
		default:
			break;
	}
}

void Game::setTileType(const crs::Location& location, crs::TileType type) {
	map[location.getX()][location.getY()].setType(type);
}

crs::TileType Game::getTileType(const crs::Location& location) {
	return map[location.getX()][location.getY()].getType();
}

Game *Game::get() {
	return instance;
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
