#pragma once

#include <SFML/Graphics.hpp>

#include "Menu.h"
#include "Button.h"
#include "MenuBackground.h"

class MainMenu : public Menu
{
private:

    MenuBackground background;

    sf::Sprite logo;

    Button playButton;
    Button continueButton;
    Button settingButton;
    Button leaderboardButton;
    Button aboutButton;
    Button exitButton;

public:

    MainMenu();

    void processEvent(const sf::Event& event) override;

    void update(float deltaTime) override;

    void draw(sf::RenderWindow& window) override;
};
