#pragma once

#include <SFML/Graphics.hpp>
#include <string>

class Button
{
private:

    sf::Sprite sprite;
    sf::Text text;

    sf::Color normalColor;
    sf::Color hoverColor;
    sf::Color pressedColor;

    bool hovered;
    bool pressed;
    bool enabled;

public:

    Button();

    void setPosition(float x, float y);
    void setScale(float scaleX, float scaleY);
  
    void setText(const std::string& str);
    void setFont(const sf::Font& font);
    void setCharacterSize(unsigned int size);

    void setNormalColor(const sf::Color& color);
    void setHoverColor(const sf::Color& color);
    void setPressedColor(const sf::Color& color);

    bool contains(sf::Vector2f mousePos) const;

    void processEvent(const sf::Event& event,
        const sf::RenderWindow& window);

    void update();

    void draw(sf::RenderWindow& window);

    bool isPressed() const;
};
