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
    bool focused = false;
    bool enabled = true;

    // Luôn căn giữa chữ trên button
    void centerText();

    // ===== ADDED: vung click/hover CO DINH, tinh lai moi khi setTexture/
    // setPosition/setScale() - KHONG doi theo animation phong to luc hover
    // (truoc day contains() dung thang sprite.getGlobalBounds(), nen khi
    // hover phong to sprite ma khong co origin o giua, no chi phinh ve
    // phia duoi-phai va lan sang vung click cua nut ben duoi/ben canh) =====
    sf::FloatRect baseBounds;
    void updateBaseBounds();

    // Scale animation
    sf::Vector2f normalScale;

    float hoverScale = 1.05f;
    float pressedScale = 0.97f;

    // Press animation
    sf::Clock pressClock;
    bool pressAnimating = false;
    sf::Time pressDuration = sf::milliseconds(50);

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

    void setFocused(bool focus);
    bool isFocused() const;

    bool contains(sf::Vector2f mousePos) const;
    bool isPressed() const;

    void press();

    // Update
    void processEvent(const sf::Event& event,
        const sf::RenderWindow& window);

    void update();

    // Render
    void draw(sf::RenderWindow& window) const;
};