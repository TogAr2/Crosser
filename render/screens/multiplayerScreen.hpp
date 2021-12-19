#ifndef CROSSER_MULTIPLAYERSCREEN_HPP
#define CROSSER_MULTIPLAYERSCREEN_HPP

#include "../gui.hpp"

class MultiplayerScreen : public Gui {
	TextFieldGuiElement* textField;
	ButtonGuiElement* connectButton;

public:
	void init(const sf::Font &font, unsigned int windowWidth, unsigned int windowHeight) override;
};

#endif //CROSSER_MULTIPLAYERSCREEN_HPP
