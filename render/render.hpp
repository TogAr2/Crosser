#ifndef CROSSER_RENDER_HPP
#define CROSSER_RENDER_HPP

#include "hud.hpp"

class Render {
	static Render* instance;

	sf::RenderWindow* window;
	sf::Font mainFont;
	sf::View view;
	Game* game = nullptr;
	Hud* hud = nullptr;
	Gui* gui = nullptr;

	bool resizeQueued = false;

public:
	explicit Render(sf::RenderWindow* window);
	~Render();

	sf::RenderWindow* &getWindow();

	sf::Font getMainFont() const;

	Hud* getHud();

	Gui* getGui();
	void setGui(Gui* gui);

	Game* getGame();
	void setGame(Game* game);

	void draw(float alpha);
	void adjustSize(unsigned int windowWidth, unsigned int windowHeight);

	void logic();
	void input();

	static Render* get();
};

#endif //CROSSER_RENDER_HPP
