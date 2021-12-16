#include "gui.hpp"

#include <utility>
#include <iostream>

GuiElement::GuiElement(sf::FloatRect bounds, std::optional<std::function<void(sf::Mouse::Button)>> callback): bounds(bounds), callback(std::move(callback)) {}

void GuiElement::update(float mouseX, float mouseY) {
	hovered = isHovered(mouseX, mouseY);
}

bool GuiElement::isHovered(float mouseX, float mouseY) const {
	return mouseX >= bounds.left && mouseX < bounds.left + bounds.width && mouseY >= bounds.top && mouseY < bounds.top + bounds.height;
}

void GuiElement::onClick(sf::Mouse::Button button) {
	if (callback.has_value())
		callback->operator()(button);
}

TextGuiElement::TextGuiElement(const sf::Text &text) : GuiElement(text.getGlobalBounds()), text(text) {}

void TextGuiElement::draw(sf::RenderWindow* &window) const {
	window->draw(text);
}

sf::Text* TextGuiElement::getText() {
	return &text;
}

ButtonGuiElement::ButtonGuiElement(const sf::RectangleShape &background, sf::Text text, std::optional<std::function<void(sf::Mouse::Button)>> callback) : GuiElement(background.getGlobalBounds(), std::move(callback)), background(background), text(std::move(text)) {}

void ButtonGuiElement::draw(sf::RenderWindow *&window) const {
	window->draw(background);
	window->draw(text);
}

void ButtonGuiElement::update(float mouseX, float mouseY) {
	bool wasHovered = hovered;
	GuiElement::update(mouseX, mouseY);
	if (wasHovered != hovered) {
		if (hovered) {
			background.setOutlineColor(sf::Color::White);
			background.setOutlineThickness(2);
			text.setRotation(3);
			text.setPosition(text.getPosition().x, text.getPosition().y - 3);
		} else {
			background.setOutlineColor(sf::Color::Transparent);
			text.setRotation(0);
			text.setPosition(text.getPosition().x, text.getPosition().y + 3);
		}
	}
}

sf::RectangleShape* ButtonGuiElement::getBackground() {
	return &background;
}

sf::Text* ButtonGuiElement::getText() {
	return &text;
}

Gui::~Gui() {
	for (GuiElement* element : elements) {
		delete element;
	}
}

void Gui::update(float mouseX, float mouseY) {
	for (GuiElement* element : elements) {
		element->update(mouseX, mouseY);
	}
}

void Gui::onClick(float mouseX, float mouseY, sf::Mouse::Button button) {
	for (GuiElement* element : elements) {
		if (element->isHovered(mouseX, mouseY)) {
			element->onClick(button);
			break;
		}
	}
}

void Gui::draw(sf::RenderWindow* &window, float alpha) const {
	for (GuiElement* element : elements) {
		element->draw(window);
	}
}

void Gui::init(const sf::Font &font, unsigned int windowWidth, unsigned int windowHeight) {
	initialized = true;

	for (GuiElement* element : elements) {
		delete element;
	}
	elements.clear();
}
