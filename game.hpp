#include <iostream>

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

    void randomFruitLocation();

public:
    Game();

    void draw();
    void input();
    void logic();

    bool isGameOver();
    int getScore();
};