#include <iostream>

using namespace std;

Game::Game() {
    gameOver = false;
    direction = STOP;
    x = width / 2;
    y = height / 2;
    score = 0;

    randomFruitLocation();
}

void Game::randomFruitLocation() {
    fruitX = rand() % width;
    fruitY = rand() % height;
}

void Game::draw() {
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

void Game::input() {
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

void Game::logic() {
    if (direction == AUTO) {
    if (x > fruitX) {
      direction = LEFT;
    } else if (x < fruitX) {
      direction = RIGHT;
    } else if (y > fruitY) {
      direction = UP;
    } else if (y < fruitY) {
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

bool Game::isGameOver() {
    return gameOver;
}

int Game::getScore() {
    return score;
}