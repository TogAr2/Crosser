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

void GuiElement::onFocusedKeyPress(sf::Event::KeyEvent event) {
}

void GuiElement::onFocusedTextEnter(sf::Event::TextEvent event) {
}

void GuiElement::unfocus() {
}

TextGuiElement::TextGuiElement(const sf::Text &text) : GuiElement(text.getGlobalBounds()), text(text) {}

void TextGuiElement::draw(sf::RenderWindow* &window) const {
	window->draw(text);
}

sf::Text* TextGuiElement::getText() {
	return &text;
}

void TextGuiElement::setAlpha(int alpha) {
	sf::Color color = text.getFillColor();
	color.a = alpha;
	text.setFillColor(color);
}

ButtonGuiElement::ButtonGuiElement(const sf::RectangleShape &background, sf::Text text, std::optional<std::function<void(sf::Mouse::Button)>> callback) : GuiElement(background.getGlobalBounds(), std::move(callback)), background(background), text(std::move(text)) {}

bool ButtonGuiElement::isHovered(float mouseX, float mouseY) const {
	return enabled && GuiElement::isHovered(mouseX, mouseY);
}

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

void ButtonGuiElement::setAlpha(int alpha) {
	sf::Color backgroundColor = background.getFillColor();
	backgroundColor.a = alpha;
	background.setFillColor(backgroundColor);

	sf::Color outlineColor = background.getOutlineColor();
	outlineColor.a = alpha;
	background.setOutlineColor(outlineColor);

	sf::Color textColor = text.getFillColor();
	textColor.a = alpha;
	text.setFillColor(textColor);
}

bool ButtonGuiElement::isEnabled() const {
	return enabled;
}

void ButtonGuiElement::setEnabled(bool enabled) {
	ButtonGuiElement::enabled = enabled;

	setAlpha(enabled ? 255 : 150);
}

TextFieldGuiElement::TextFieldGuiElement(const sf::RectangleShape &background, sf::Text text, std::string placeholder) : GuiElement(background.getGlobalBounds()), background(background), text(std::move(text)), placeholder(std::move(placeholder)) {
	cursor.setSize(sf::Vector2f(2, text.getLocalBounds().height));

	TextFieldGuiElement::background.setOutlineThickness(1);
	TextFieldGuiElement::background.setOutlineColor(sf::Color(255, 255, 255, 180));

	updateInput();
}

void TextFieldGuiElement::updateInput() {
	sf::Color textColor = text.getFillColor();
	if (enteredText.empty()) {
		text.setString(placeholder);
		textColor.a = 200;
	} else {
		text.setString(enteredText);
		textColor.a = 255;
	}
	text.setFillColor(textColor);

	sf::Vector2f cursorPosition = text.findCharacterPos(cursorIndex);
	cursorPosition.y += 4;
	cursor.setPosition(cursorPosition);

	time = 0;
}

void TextFieldGuiElement::onClick(sf::Mouse::Button button) {
	GuiElement::onClick(button);

	sf::Color outlineColor = background.getOutlineColor();
	outlineColor.a = 255;
	background.setOutlineColor(outlineColor);
	background.setOutlineThickness(2);

	focused = true;
	time = 0;
}

void TextFieldGuiElement::onFocusedKeyPress(sf::Event::KeyEvent event) {
	if (event.code == sf::Keyboard::Backspace) {
		if (cursorIndex <= 0) return;
		enteredText.erase(cursorIndex - 1, 1);
		cursorIndex--;
	} else if (event.code == sf::Keyboard::Space) {
		enteredText.insert(cursorIndex, " ");
		cursorIndex++;
	} else if (event.code == sf::Keyboard::Left) {
		if (cursorIndex <= 0) return;
		cursorIndex--;
	} else if (event.code == sf::Keyboard::Right) {
		if (cursorIndex >= enteredText.size()) return;
		cursorIndex++;
	} else {
		return;
	}

	updateInput();
}

void TextFieldGuiElement::onFocusedTextEnter(sf::Event::TextEvent event) {
	if (event.unicode > 0x0020) { // Check if character is valid
		enteredText.insert(cursorIndex, (char*) &event.unicode);
		cursorIndex++;

		updateInput();
	}
}

void TextFieldGuiElement::unfocus() {
	focused = false;

	sf::Color outlineColor = background.getOutlineColor();
	outlineColor.a = 180;
	background.setOutlineColor(outlineColor);
	background.setOutlineThickness(1);
}

void TextFieldGuiElement::update(float mouseX, float mouseY) {
	GuiElement::update(mouseX, mouseY);
	time++;
}

void TextFieldGuiElement::draw(sf::RenderWindow* &window) const {
	window->draw(background);
	window->draw(text);

	if (focused && (time / 10) % 2 == 0) {
		window->draw(cursor);
	}
}

void TextFieldGuiElement::setAlpha(int alpha) {
	sf::Color backgroundColor = background.getFillColor();
	backgroundColor.a = alpha;
	background.setFillColor(backgroundColor);

	sf::Color outlineColor = background.getOutlineColor();
	outlineColor.a = alpha;
	background.setOutlineColor(outlineColor);

	sf::Color textColor = text.getFillColor();
	textColor.a = alpha;
	text.setFillColor(textColor);

	sf::Color cursorColor = cursor.getFillColor();
	cursorColor.a = alpha;
	cursor.setFillColor(cursorColor);
}

const std::string &TextFieldGuiElement::getEnteredText() const {
	return enteredText;
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
	if (focusedElement != nullptr && !focusedElement->isHovered(mouseX, mouseY)) {
		focusedElement->unfocus();
		focusedElement = nullptr;
	}

	for (GuiElement* element : elements) {
		if (element->isHovered(mouseX, mouseY)) {
			element->onClick(button);
			focusedElement = element;
			break;
		}
	}
}

void Gui::onKeyPress(sf::Event::KeyEvent event) {
	if (focusedElement != nullptr) {
		focusedElement->onFocusedKeyPress(event);
	}
}

void Gui::onTextEnter(sf::Event::TextEvent event) {
	if (focusedElement != nullptr) {
		focusedElement->onFocusedTextEnter(event);
	}
}

void Gui::draw(sf::RenderWindow* &window, float alpha) const {
	for (GuiElement* element : elements) {
		element->draw(window);
	}
}

void Gui::init(const sf::Font &font, unsigned int windowWidth, unsigned int windowHeight) {
	focusedElement = nullptr;
	initialized = true;

	for (GuiElement* element : elements) {
		delete element;
	}
	elements.clear();
}
