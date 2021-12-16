#ifndef CROSSER_MAINSCREEN_HPP
#define CROSSER_MAINSCREEN_HPP

#include "gui.hpp"

class MainScreen : public Gui {
	TextGuiElement* crosserTextElement;
	ButtonGuiElement* playButton;
	ButtonGuiElement* multiplayerButton;
	int animationProgress = 0;

public:
	void init(const sf::Font &font, unsigned int windowWidth, unsigned int windowHeight) override;

	void update(float mouseX, float mouseY) override;
	void draw(sf::RenderWindow* &window, float alpha) const override;
};

#endif //CROSSER_MAINSCREEN_HPP
