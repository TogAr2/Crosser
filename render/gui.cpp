#include "gui.hpp"

GuiElement::GuiElement(sf::RectangleShape* shape, std::function<void()> &callback): hovered(false), shape(shape), callback(callback) {}

GuiElement::~GuiElement() {
	delete shape;
}

void GuiElement::update(float mouseX, float mouseY) {
	bool nowHovered = isHovered(mouseX, mouseY);
	if (hovered != nowHovered) {
		hovered = nowHovered;
		if (hovered) {
			shape->setOutlineColor(sf::Color::White);
		} else {
			shape->setOutlineColor(sf::Color::Transparent);
		}
	}
}

bool GuiElement::isHovered(float mouseX, float mouseY) const {
	sf::Vector2f position = shape->getPosition();
	sf::Vector2f size = shape->getSize();
	return mouseX >= position.x && mouseX < position.x + size.x && mouseY >= position.y && mouseY < position.y + size.y;
}

void GuiElement::onClick() {
	callback();
}

void GuiElement::draw(sf::RenderWindow* &window) const {
	window->draw(*shape);
}

void Gui::update(float mouseX, float mouseY) {
	for (GuiElement* element : elements) {
		element->update(mouseX, mouseY);
	}
}

void Gui::onClick(float mouseX, float mouseY) {
	for (GuiElement* element : elements) {
		if (element->isHovered(mouseX, mouseY)) {
			element->onClick();
			break;
		}
	}
}

void Gui::draw(sf::RenderWindow* &window) const {
	for (GuiElement* element : elements) {
		element->draw(window);
	}
}
