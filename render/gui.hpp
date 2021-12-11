#ifndef CROSSER_GUI_HPP
#define CROSSER_GUI_HPP

#include <SFML/Graphics.hpp>

class GuiElement {
	bool hovered;
	sf::RectangleShape* shape;
	std::function<void()> callback;

public:
	GuiElement(sf::RectangleShape* shape, std::function<void()> &callback);
	~GuiElement();

	void update(float mouseX, float mouseY);
	bool isHovered(float mouseX, float mouseY) const;
	void onClick();
	void draw(sf::RenderWindow* &window) const;
};

class Gui {
	std::vector<GuiElement*> elements;

public:
	void update(float mouseX, float mouseY);
	void onClick(float mouseX, float mouseY);
	void draw(sf::RenderWindow* &window) const;
};

#endif //CROSSER_GUI_HPP
