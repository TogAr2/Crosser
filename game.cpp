#include <iostream>
#include <random>

#include "config.hpp"
#include "utils.hpp"

#include <SFML/Graphics.hpp>

using namespace std;

enum Direction {
    STOP = 0,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    AUTO
};

enum BlockType {
    AIR,
    PLAYER,
    FRUIT,
    OBSTACLE
};

class Game {

    const string explanation[40] = {
        "",
        "Welcome to Crosser v" + (string) CROSSER_VERSION,
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

    const static int width = 40;
    const static int height = 20;

    const static int blockSize = 20;

    BlockType map[20][40]{};

    std::mt19937 random;

    bool gameOver;
    int x{}, y{}, fruitX{}, fruitY{}, score;

    Direction direction;

    sf::RenderWindow* window;
    sf::Font mainFont;

    void addRandomObstacle() {
        std::uniform_int_distribution<int> widthDistribution(0, width / 2);
        std::uniform_int_distribution<int> heightDistribution(0, height / 2);

        int obstacleX = widthDistribution(random) * 2;
        int obstacleY = heightDistribution(random) * 2;

        if (map[obstacleY][obstacleX] == AIR) {
            map[obstacleY][obstacleX] = OBSTACLE;
        } else {
            addRandomObstacle();
        }
    }

    bool isObstacle(int locX, int locY) {
        return map[locY][locX] == OBSTACLE;
    }

    void setPlayerInMap(int locX, int locY) {
        map[y][x] = AIR;

        x = locX;
        y = locY;

        map[y][x] = PLAYER;
    }

    void randomFruitLocation() {
        std::uniform_int_distribution<int> widthDistribution(0, width);
        std::uniform_int_distribution<int> heightDistribution(0, height );

        fruitX = widthDistribution(random);
        fruitY = heightDistribution(random);

        if (map[fruitY][fruitX] == AIR) {
            map[fruitY][fruitX] = FRUIT;
        } else {
            randomFruitLocation();
        }
    }

public:
    explicit Game(sf::RenderWindow* window) { // NOLINT(cert-msc51-cpp)
        gameOver = false;
        direction = STOP;
        score = 0;
        this->window = window;

        for (auto & i : map) {
            for (auto & j : i) {
                j = AIR;
            }
        }

        setPlayerInMap(width / 2, height / 2);
        randomFruitLocation();

        for (int i = 0; i < 30; i++) {
            addRandomObstacle();
        }

        std::random_device randomDevice;
        random.seed(randomDevice());

        sf::Font font;
        if (!font.loadFromFile("/System/Library/Fonts/Helvetica.ttc")) {
        	cout << "Error: Could not load font!" << endl;
        }

        mainFont = font;
    }

    void draw() {
        window->clear(sf::Color::Black);

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
            	if (map[i][j] == AIR) continue;

            	sf::RectangleShape shape(sf::Vector2f(blockSize, blockSize));
            	shape.setPosition((float) j * blockSize, (float) i * blockSize);

                if (map[i][j] == PLAYER) {
                	shape.setFillColor(sf::Color::Cyan);
                    //cout << "O";
                } else if (map[i][j] == FRUIT) {
                	shape.setFillColor(sf::Color::Green);
                    //cout << "X";
                } else if (map[i][j] == OBSTACLE) {
                	shape.setFillColor(sf::Color::Red);
                    //cout << "█";
                }

                window->draw(shape);
            }

            //cout << " " << explanation[i] << endl;
        }

        //cout << "Score: " << score << endl;

        window->display();
    }

    void input() {
    	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
    		direction = UP;
    		return;
    	}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			direction = LEFT;
			return;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			direction = DOWN;
			return;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			direction = RIGHT;
			return;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::M)) {
			direction = AUTO;
			return;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
			gameOver = true;
			direction = STOP;
			return;
		}

		direction = STOP;
    }

    void logic() {
        if (direction == AUTO) {
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
                    setPlayerInMap(x, y - 1);
                }
            } else {
                setPlayerInMap(x, height - 1);
            }
            break;
        case LEFT:
            if (x > 0) {
                if (!isObstacle(x - 1, y)) {
                    setPlayerInMap(x - 1, y);
                }
            } else {
                setPlayerInMap(width - 1, y);
            }
            break;
        case DOWN:
            if (y < height - 1) {
                if (!isObstacle(x, y + 1)) {
                    setPlayerInMap(x, y + 1);
                }
            } else {
                setPlayerInMap(x, 0);
            }
            break;
        case RIGHT:
            if (x < width - 1) {
                if (!isObstacle(x + 1, y)) {
                    setPlayerInMap(x + 1, y);
                }
            } else {
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

    [[nodiscard]] bool isGameOver() const {
        return gameOver;
    }

    [[nodiscard]] int getScore() const {
        return score;
    }

    [[nodiscard]] sf::Font getMainFont() const {
    	return mainFont;
    }
};
