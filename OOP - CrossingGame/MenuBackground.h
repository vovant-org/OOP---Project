#pragma once

#include <SFML/Graphics.hpp>

class MenuBackground
{
private:

    sf::Sprite sprite;

public:

    MenuBackground();

    void setTexture(const sf::Texture& texture);

    void setPosition(float x, float y);

    void setScale(float scaleX, float scaleY);

    sf::FloatRect getGlobalBounds() const;

    void draw(sf::RenderWindow& window) const;
};