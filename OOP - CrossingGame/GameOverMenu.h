#pragma once

#include "Menu.h"
#include "Button.h"

class GameOverMenu : public Menu
{
private:

    Button retryButton;
    Button mainMenuButton;

public:

    GameOverMenu();

    void processEvent(const sf::Event& event) override;

    void update(float deltaTime) override;

    void draw(sf::RenderWindow& window) override;
};
