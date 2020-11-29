#include <iostream>

using namespace std;

enum Direction {
  STOP = 0,
  LEFT,
  RIGHT,
  UP,
  DOWN,
  AUTO
};

class Game {

    const std::string explanation[20] = {
        "",
        "Welcome to Crosser v0.2",
        "",
        "Use WASD to move.",
        "",
        "Search for the cross.",
        "",
        "Press M if you are a lazy person :/",
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
        "",
        ""
    };
    const int width = 20;
    const int height = 20;

    bool gameOver;
    int x, y, fruitX, fruitY, score;

    Direction direction;

    void randomFruitLocation() {
        fruitX = rand() % width;
        fruitY = rand() % height;
    }

public:
    Game() {
        gameOver = false;
        direction = STOP;
        x = width / 2;
        y = height / 2;
        score = 0;

        randomFruitLocation();
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

                if (i == y && j == x) {
                    cout << "█";
                } else if (i == fruitY && j == fruitX) {
                    cout << "X";
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
                if (fruitX - x > width/2 || fruitX - x < (0 - width)/2) {
                    direction = RIGHT;
                } else {
                    direction = LEFT;
                }
            } else if (x < fruitX) {
                if (fruitX - x > width/2 || fruitX - x < (0 - width)/2) {
                    direction = LEFT;
                } else {
                    direction = RIGHT;
                }
            } else if (y > fruitY) {
                if (fruitY - y > height/2 || fruitY - y < (0 - height)/2) {
                    direction = DOWN;
                } else {
                    direction = UP;
                }
            } else if (y < fruitY) {
                if (fruitY - y > height/2 || fruitY - y < (0 - height)/2) {
                    direction = UP;
                } else {
                    direction = DOWN;
                }
                direction = DOWN;
            }
        }

        switch (direction) {
        case UP:
            if (y > 0) {
                y--;
            } else {
                y = height - 1;
            }
            break;
        case LEFT:
            if (x > 0) {
                x--;
            } else {
                x = width - 1;
            }
            break;
        case DOWN:
            if (y < height - 1) {
                y++;
            } else {
                y = 0;
            }
            break;
        case RIGHT:
            if (x < width - 1) {
                x++;
            } else {
                x = 0;
            }
            break;
        default:
            break;
        }

        if (x == fruitX && y == fruitY) {
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
