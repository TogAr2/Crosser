#ifndef CROSSER_TILE_HPP
#define CROSSER_TILE_HPP

#include <SFML/Graphics.hpp>
#include <list>

#include "game.hpp"
#include "../plugins/utils.hpp"
#include "api-utils.hpp"

class Tile {

	int x;
	int y;

	crs::TileType type;

public:
	Tile();
	Tile(int x, int y, crs::TileType type);

	void draw(sf::RenderWindow* window);

	[[nodiscard]] int getX() const;
	void setX(int x);
	[[nodiscard]] int getY() const;
	void setY(int y);

	[[nodiscard]] crs::TileType getType() const;
	void setType(crs::TileType blockType);
};

class Game;

class PathfinderTile : public Tile {

	PathfinderTile* previous = nullptr;

public:
	PathfinderTile(int x, int y, PathfinderTile *previous);
	PathfinderTile(int x, int y);

	[[nodiscard]] bool isFirst() const;
	[[nodiscard]] PathfinderTile* getPrevious() const;
	void setPrevious(PathfinderTile *previous);

	std::list<PathfinderTile> getAdjacentTiles(Game* game);
};

#endif //CROSSER_TILE_HPP
