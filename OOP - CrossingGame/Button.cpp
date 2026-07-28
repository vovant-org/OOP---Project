#include "Button.h"

Button::Button()
{
    normalColor = sf::Color::White;
    hoverColor = sf::Color(255, 230, 120);
    pressedColor = sf::Color(255, 180, 40);

    text.setFillColor(normalColor);
}

//==================================================
// Private
//==================================================

void Button::centerText()
{
    sf::FloatRect spriteBounds = sprite.getGlobalBounds();
    sf::FloatRect textBounds = text.getLocalBounds();

    text.setPosition(
        spriteBounds.left + (spriteBounds.width - textBounds.width) / 2.f - textBounds.left,
        spriteBounds.top + (spriteBounds.height - textBounds.height) / 2.f - textBounds.top
    );
}

//==================================================
// Texture
//==================================================

void Button::setTexture(const sf::Texture& texture)
{
    sprite.setTexture(texture);
    centerText();
}

//==================================================
// Transform
//==================================================

void Button::setPosition(float x, float y)
{
    sprite.setPosition(x, y);
    centerText();
}

void Button::setScale(float scaleX, float scaleY)
{
    sprite.setScale(scaleX, scaleY);
    centerText();
}

//==================================================
// Text
//==================================================

void Button::setText(const std::string& str)
{
    text.setString(str);
    centerText();
}

void Button::setFont(const sf::Font& font)
{
    text.setFont(font);
    centerText();
}

void Button::setCharacterSize(unsigned int size)
{
    text.setCharacterSize(size);
    centerText();
}

//==================================================
// Color
//==================================================

void Button::setNormalColor(const sf::Color& color)
{
    normalColor = color;
}

void Button::setHoverColor(const sf::Color& color)
{
    hoverColor = color;
}

void Button::setPressedColor(const sf::Color& color)
{
    pressedColor = color;
}

//==================================================
// State
//==================================================

void Button::setEnabled(bool enable)
{
    enabled = enable;
}

bool Button::isEnabled() const
{
    return enabled;
}

bool Button::contains(sf::Vector2f mousePos) const
{
    return sprite.getGlobalBounds().contains(mousePos);
}

bool Button::isPressed() const
{
    return pressed;
}

//==================================================
// Update
//==================================================

void Button::processEvent(const sf::Event& event,
    const sf::RenderWindow& window)
{
    if (!enabled)
        return;

    sf::Vector2f mousePos =
        window.mapPixelToCoords(sf::Mouse::getPosition(window));

    hovered = contains(mousePos);

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        pressed = hovered;
    }

    if (event.type == sf::Event::MouseButtonReleased &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        pressed = false;
    }
}

void Button::update()
{
    if (!enabled)
    {
        text.setFillColor(sf::Color(150, 150, 150));
        return;
    }

    if (pressed)
        text.setFillColor(pressedColor);
    else if (hovered)
        text.setFillColor(hoverColor);
    else
        text.setFillColor(normalColor);
}

//==================================================
// Render
//==================================================

void Button::draw(sf::RenderWindow& window) const
{
    window.draw(sprite);
    window.draw(text);
}