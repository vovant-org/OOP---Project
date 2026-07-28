#pragma once

#include <SFML/Graphics.hpp>
#include <string>

class MenuBackground
{
private:

    sf::Sprite sprite;

public:

    MenuBackground();

    bool load(const std::string& filename);

    void setPosition(float x, float y);

    void draw(sf::RenderWindow& window);
};