#include <iostream>

#include "utils.hpp"

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

    const string explanation[20] = {
        "",
        "Welcome to Crosser v0.3",
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
    BlockType map[20][40];

    bool gameOver;
    int x, y, fruitX, fruitY, score;

    Direction direction;

    void addRandomObstacle() {
        int obstacleX = (rand() % (width / 2)) * 2;
        int obstacleY = (rand() % (height / 2)) * 2;

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
        fruitX = rand() % width;
        fruitY = rand() % height;

        if (map[fruitY][fruitX] == AIR) {
            map[fruitY][fruitX] = FRUIT;
        } else {
            randomFruitLocation();
        }
    }

public:
    Game() {
        gameOver = false;
        direction = STOP;
        score = 0;

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                map[i][j] = AIR;
            }
        }

        setPlayerInMap(width / 2, height / 2);
        randomFruitLocation();

        for (int i = 0; i < 30; i++) {
            addRandomObstacle();
        }
    }

    void draw() {
        system("clear"); //Clear
        for (int i = 0; i < width+2; i++)
            cout << "#";
        cout << endl;

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (j == 0) {
                    cout << "#";
                }

                if (map[i][j] == PLAYER) {
                    cout << "O";
                } else if (map[i][j] == FRUIT) {
                    cout << "X";
                } else if (map[i][j] == OBSTACLE) {
                    cout << "█";
                } else {
                    cout << " ";
                }

                if (j == width - 1) {
                    cout << "#";
                }
            }

            cout << " " << explanation[i] << endl;
        }

        for (int i = 0; i < width+2; i++)
            cout << "#";
        cout << endl;

        cout << "Score: " << score << endl;
    }

    void input() {
        char input = getch();

        switch (input) {
            case 'w':
            direction = UP;
            break;
        case 'a':
            direction = LEFT;
            break;
        case 's':
            direction = DOWN;
            break;
        case 'd':
            direction = RIGHT;
            break;
        case 'm':
            direction = AUTO;
            break;
        case 'q':
            gameOver = true;
            //No break here
        default:
            direction = STOP;
            break;
        }
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

    bool isGameOver() {
        return gameOver;
    }

    int getScore() {
        return score;
    }
};
