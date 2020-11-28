#include <iostream>
#include <unistd.h>
#include <termios.h>

#include "utils.hpp";
#include "game.hpp"

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