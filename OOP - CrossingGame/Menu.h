#pragma once

#include <SFML/Graphics.hpp>

class Menu
{
public:

    virtual void processEvent(const sf::Event& event,
        const sf::RenderWindow& window) = 0;

    virtual void update() = 0;

    virtual void draw(sf::RenderWindow& window) const = 0;

    virtual ~Menu() = default;
};