#pragma once

#include <SFML/Graphics.hpp>

class Menu
{
public:
    virtual ~Menu() = default;

    virtual void processEvent(const sf::Event& event) = 0;
    virtual void update(float deltaTime) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
};
