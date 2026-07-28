#pragma once

#include "Menu.h"
#include "Button.h"

class WinMenu : public Menu
{
private:

    Button nextLevelButton;
    Button mainMenuButton;

public:

    WinMenu();

    void processEvent(const sf::Event& event) override;

    void update(float deltaTime) override;

    void draw(sf::RenderWindow& window) override;
};
