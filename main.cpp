#include <iostream>

#include "utils.hpp"
#include "game.cpp"

using namespace std;

int main() {
  Game game;
  game.draw();

  while (!game.isGameOver()) {
    game.input();
    game.logic();
    game.draw();
  }

  system("clear");

  cout << "Your score was: " << game.getScore() << endl;

  return 0;
}