#pragma once

#include "Menu.h"
#include "Button.h"

class LeaderboardMenu : public Menu
{
private:

    Button backButton;

public:

    LeaderboardMenu();

    void processEvent(const sf::Event& event) override;

    void update(float deltaTime) override;

    void draw(sf::RenderWindow& window) override;
};
