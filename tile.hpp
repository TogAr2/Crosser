#ifndef CROSSER_TILE_HPP
#define CROSSER_TILE_HPP

#include <SFML/Graphics.hpp>
#include <list>

#include "game.hpp"
#include "utils.hpp"

enum BlockType {
	AIR,
	PLAYER,
	FRUIT,
	OBSTACLE,
	VISITED
};

class Tile {

	int x;
	int y;

	BlockType type;

public:
	Tile();
	Tile(int x, int y, BlockType type);

	void draw(sf::RenderWindow* window, const float& alpha, const Direction& moving);

	[[nodiscard]] int getX() const;
	void setX(int x);
	[[nodiscard]] int getY() const;
	void setY(int y);

	[[nodiscard]] BlockType getType() const;
	void setType(BlockType blockType);
};

class Game;

class PathfinderTile : public Tile {

	PathfinderTile* previous = nullptr;

public:
	PathfinderTile(int x, int y, PathfinderTile *previous);
	PathfinderTile(int x, int y);

	[[nodiscard]] bool isFirst() const;
	[[nodiscard]] PathfinderTile &getPrevious() const;
	void setPrevious(PathfinderTile *previous);

	std::list<PathfinderTile> getAdjacentTiles(const Game& game);
};

#endif //CROSSER_TILE_HPP
