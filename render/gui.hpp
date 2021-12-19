#ifndef CROSSER_GUI_HPP
#define CROSSER_GUI_HPP

#include <SFML/Graphics.hpp>

class GuiElement {
	sf::FloatRect bounds;
	std::optional<std::function<void(sf::Mouse::Button)>> callback;

protected:
	bool hovered = false;

public:
	explicit GuiElement(sf::FloatRect bounds, std::optional<std::function<void(sf::Mouse::Button)>> callback = std::optional<std::function<void(sf::Mouse::Button)>>());
	virtual ~GuiElement() = default;

	virtual void update(float mouseX, float mouseY);
	virtual bool isHovered(float mouseX, float mouseY) const;
	virtual void onClick(sf::Mouse::Button button);
	virtual void draw(sf::RenderWindow* &window) const = 0;

	virtual void onFocusedKeyPress(sf::Event::KeyEvent event);
	virtual void onFocusedTextEnter(sf::Event::TextEvent event);
	virtual void unfocus();

	virtual void setAlpha(int alpha) = 0;
};

class TextGuiElement : public GuiElement {
	sf::Text text;

public:
	explicit TextGuiElement(const sf::Text &text);

	void draw(sf::RenderWindow* &window) const override;

	sf::Text* getText();

	void setAlpha(int alpha) override;
};

class ButtonGuiElement : public GuiElement {
	sf::RectangleShape background;
	sf::Text text;
	bool enabled = true;

public:
	ButtonGuiElement(const sf::RectangleShape &background, sf::Text text, std::optional<std::function<void(sf::Mouse::Button)>> callback = std::optional<std::function<void(sf::Mouse::Button)>>());

	void draw(sf::RenderWindow* &window) const override;

	void update(float mouseX, float mouseY) override;

	sf::RectangleShape* getBackground();
	sf::Text* getText();

	void setAlpha(int alpha) override;

	bool isEnabled() const;

	void setEnabled(bool enabled);

	bool isHovered(float mouseX, float mouseY) const override;
};

class TextFieldGuiElement : public GuiElement {
	sf::RectangleShape background;
	sf::Text text;
	sf::RectangleShape cursor;
	std::string placeholder;
	std::string enteredText;
	int cursorIndex = 0;
	bool focused = false;
	int time = 0;

public:
	TextFieldGuiElement(const sf::RectangleShape &background, sf::Text text, std::string placeholder);

	void onClick(sf::Mouse::Button button) override;
	void unfocus() override;

	void draw(sf::RenderWindow* &window) const override;

	void setAlpha(int alpha) override;

	void update(float mouseX, float mouseY) override;
	void onFocusedKeyPress(sf::Event::KeyEvent event) override;
	void onFocusedTextEnter(sf::Event::TextEvent event) override;

	void updateInput();

	const std::string &getEnteredText() const;
};

class Gui {
protected:
	std::vector<GuiElement*> elements;
	GuiElement* focusedElement = nullptr;
	bool initialized = false;

public:
	~Gui();

	virtual void update(float mouseX, float mouseY);
	void onClick(float mouseX, float mouseY, sf::Mouse::Button button);
	virtual void onKeyPress(sf::Event::KeyEvent event);
	virtual void onTextEnter(sf::Event::TextEvent event);
	virtual void draw(sf::RenderWindow* &window, float alpha) const;

	virtual void init(const sf::Font &font, unsigned int windowWidth, unsigned int windowHeight);
};

#endif //CROSSER_GUI_HPP
