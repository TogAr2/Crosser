#include "game.hpp"

#include <SFML/Graphics.hpp>

void Game::addRandomObstacle() {
	std::uniform_int_distribution<int> widthDistribution(0, width / 2);
	std::uniform_int_distribution<int> heightDistribution(0, height / 2);

	int obstacleX = widthDistribution(random) * 2;
	int obstacleY = heightDistribution(random) * 2;

	if (map[obstacleX][obstacleY].getType() == AIR) {
		map[obstacleX][obstacleY].setType(OBSTACLE);
	} else {
		addRandomObstacle();
	}
}

bool Game::isObstacle(int locX, int locY) {
	return map[locX][locY].getType() == OBSTACLE;
}

void Game::setPlayerInMap(int locX, int locY) {
	map[x][y].setType(AIR);

	x = locX;
	y = locY;

	map[x][y].setType(PLAYER);
}

void Game::randomFruitLocation() {
	sf::Vector2<float> viewSize = window->getView().getSize();
	sf::Vector2<float> viewOrigin = sf::Vector2<float>(window->getView().getCenter().x - (viewSize.x / 2), window->getView().getCenter().y - (viewSize.y / 2));

	std::uniform_int_distribution<int> widthDistribution(viewOrigin.x, viewOrigin.x + viewSize.x - 1);
	std::uniform_int_distribution<int> heightDistribution(viewOrigin.y, viewOrigin.y + viewSize.y - 1);

	fruitX = widthDistribution(random) / blockSize;
	fruitY = heightDistribution(random) / blockSize;

	if (map[fruitX][fruitY].getType() == AIR) {
		map[fruitX][fruitY].setType(FRUIT);
	} else {
		randomFruitLocation();
	}
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

	visited[x][y] = true;
	map[x][y].setType(VISITED);
	queue.emplace_back(x, y);

	std::list<PathfinderTile>::iterator i;

	while (!queue.empty()) {
		PathfinderTile currentTile = queue.front();
		queue.pop_front();

		if (map[currentTile.getX()][currentTile.getY()].getType() == FRUIT) {
			reachedFinish = true;
			finishTile = &currentTile;
			break;
		}

		std::list<PathfinderTile> adjacentBlocks = currentTile.getAdjacentTiles(*this);
		for (i = adjacentBlocks.begin(); i != adjacentBlocks.end(); i++) {
			PathfinderTile adjacentTile = *i;
			if (visited[adjacentTile.getX()][adjacentTile.getY()]) continue;

			visited[adjacentTile.getX()][adjacentTile.getY()] = true;
			if (map[adjacentTile.getX()][adjacentTile.getY()].getType() != FRUIT) {
				map[adjacentTile.getX()][adjacentTile.getY()].setType(VISITED);
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

		draw(0);
	}

	for (auto & loopX : map) {
		for (auto & loopY : loopX) {
			if (loopY.getType() == VISITED) {
				loopY.setType(AIR);
			}
		}
	}

	PathfinderTile* current = finishTile;
	while (!current->isFirst()) {
		map[current->getX()][current->getY()].setType(VISITED);
		current = &current->getPrevious();
	}

	return reachedFinish;
}

Game::Game(sf::RenderWindow* window) { // NOLINT(cert-msc51-cpp)
	gameOver = false;
	direction = STOP;
	moving = STOP;
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

	setPlayerInMap(width / 2, height / 2);
	randomFruitLocation();

	for (int i = 0; i < 30; i++) {
		addRandomObstacle();
	}

	sf::Font font;
	if (!font.loadFromFile("/System/Library/Fonts/Helvetica.ttc")) {
		std::cout << "Error: Could not load font!" << std::endl;
	}

	mainFont = font;
}

void Game::draw(float alpha) {
	window->clear(sf::Color::Black);

	for (auto & loopX : map) {
		for (auto & loopY : loopX) {
			loopY.draw(window, alpha, moving);
		}

		//cout << " " << explanation[i] << endl;
	}

	//cout << "Score: " << score << endl;

	sf::View view = sf::View();

	view.setCenter((float) width / 2, (float) height / 2);
	view.reset(sf::FloatRect(0, 0, window->getSize().x, window->getSize().y));

	//Interpolate zoom
	float newZoom = zoom + ((zoom - previousZoom) * alpha / (float) maxWaitBeforeZoom) - (zoom - previousZoom);
	view.zoom(newZoom);

	window->setView(view);

	window->display();
}

void Game::input() {
	direction = STOP;

	sf::Event event{};
	while (window->pollEvent(event)) {
		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up) {
				direction = UP;
			} else if (event.key.code == sf::Keyboard::A || event.key.code == sf::Keyboard::Left) {
				direction = LEFT;
			} else if (event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down) {
				direction = DOWN;
			} else if (event.key.code == sf::Keyboard::D || event.key.code == sf::Keyboard::Right) {
				direction = RIGHT;
			} else if (event.key.code == sf::Keyboard::M) {
				direction = AUTO;
			} else if (event.key.code == sf::Keyboard::Q) {
				gameOver = true;
			} else if (event.key.code == sf::Keyboard::B) {
				std::cout << "Player: " << x << " " << y << std::endl;
				std::cout << "Fruit: " << fruitX << " " << fruitY << std::endl;
			} else if (event.key.code == sf::Keyboard::U) {
				setZoom(zoom + 0.1f);
			}
		}
	}
}

void Game::logic() {
	if (waitBeforeZoom == 0) {
		previousZoom = zoom;
	} else {
		waitBeforeZoom--;
	}

	moving = STOP;

	if (direction == AUTO) {
		bfs();

		if (x > fruitX) {
			if (fruitX - x < (0 - width)/2) {
				direction = RIGHT;
			} else {
				direction = LEFT;
			}
		} else if (x < fruitX) {
			if (fruitX - x > width/2) {
				direction = LEFT;
			} else {
				direction = RIGHT;
			}
		} else if (y > fruitY) {
			if (fruitY - y < (0 - height)/2) {
				direction = DOWN;
			} else {
				direction = UP;
			}
		} else if (y < fruitY) {
			if (fruitY - y > height/2) {
				direction = UP;
			} else {
				direction = DOWN;
			}
		}
	}

	switch (direction) {
	case UP:
		if (y > 0) {
			if (!isObstacle(x, y - 1)) {
				moving = UP;
				setPlayerInMap(x, y - 1);
			} else {
				moving = STOP;
			}
		} else {
			moving = UP;
			setPlayerInMap(x, height - 1);
		}
		break;
	case LEFT:
		if (x > 0) {
			if (!isObstacle(x - 1, y)) {
				moving = LEFT;
				setPlayerInMap(x - 1, y);
			} else {
				moving = STOP;
			}
		} else {
			moving = LEFT;
			setPlayerInMap(width - 1, y);
		}
		break;
	case DOWN:
		if (y < height - 1) {
			if (!isObstacle(x, y + 1)) {
				moving = DOWN;
				setPlayerInMap(x, y + 1);
			} else {
				moving = STOP;
			}
		} else {
			moving = DOWN;
			setPlayerInMap(x, 0);
		}
		break;
	case RIGHT:
		if (x < width - 1) {
			if (!isObstacle(x + 1, y)) {
				moving = RIGHT;
				setPlayerInMap(x + 1, y);
			} else {
				moving = STOP;
			}
		} else {
			moving = RIGHT;
			setPlayerInMap(0, y);
		}
		break;
	default:
		break;
	}

	if (y == fruitY && x == fruitX) {
		score++;
		randomFruitLocation();
	}
}

[[nodiscard]] bool Game::isGameOver() const {
	return gameOver;
}

[[nodiscard]] int Game::getScore() const {
	return score;
}

[[nodiscard]] sf::Font Game::getMainFont() const {
	return mainFont;
}

void Game::setZoom(float newZoom) {
	previousZoom = zoom;
	zoom = newZoom;
	waitBeforeZoom = maxWaitBeforeZoom;
}
