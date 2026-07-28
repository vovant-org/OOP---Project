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

    bool hovered = false;
    bool pressed = false;
    bool enabled = true;

    // Luôn căn giữa chữ trên button
    void centerText();

public:

    Button();

    // Texture
    void setTexture(const sf::Texture& texture);

    // Transform
    void setPosition(float x, float y);
    void setScale(float scaleX, float scaleY);

    // Text
    void setText(const std::string& str);
    void setFont(const sf::Font& font);
    void setCharacterSize(unsigned int size);

    // Color
    void setNormalColor(const sf::Color& color);
    void setHoverColor(const sf::Color& color);
    void setPressedColor(const sf::Color& color);

    // State
    void setEnabled(bool enable);
    bool isEnabled() const;

    bool contains(sf::Vector2f mousePos) const;
    bool isPressed() const;

    // Update
    void processEvent(const sf::Event& event,
        const sf::RenderWindow& window);

    void update();

    // Render
    void draw(sf::RenderWindow& window) const;
};