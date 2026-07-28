#pragma once

#include "Menu.h"
#include "Button.h"

class AboutMenu : public Menu
{
private:

    Button backButton;

public:

    AboutMenu();

    void processEvent(const sf::Event& event) override;

    void update(float deltaTime) override;

    void draw(sf::RenderWindow& window) override;
};
