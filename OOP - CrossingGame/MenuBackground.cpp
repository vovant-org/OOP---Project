#include "MenuBackground.h"

//==================================================
// Constructor
//==================================================

MenuBackground::MenuBackground()
{
}

//==================================================
// Texture
//==================================================

void MenuBackground::setTexture(const sf::Texture& texture)
{
    sprite.setTexture(texture);
}

//==================================================
// Transform
//==================================================

void MenuBackground::setPosition(float x, float y)
{
    sprite.setPosition(x, y);
}

void MenuBackground::setScale(float scaleX, float scaleY)
{
    sprite.setScale(scaleX, scaleY);
}

sf::FloatRect MenuBackground::getGlobalBounds() const
{
    return sprite.getGlobalBounds();
}

//==================================================
// Render
//==================================================

void MenuBackground::draw(sf::RenderWindow& window) const
{
    window.draw(sprite);
}