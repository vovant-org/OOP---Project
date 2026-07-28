#pragma once

#include "Menu.h"
#include "Button.h"

class SettingMenu : public Menu
{
private:

    Button musicButton;
    Button soundButton;
    Button fullscreenButton;
    Button backButton;

public:

    SettingMenu();

    void processEvent(const sf::Event& event) override;

    void update(float deltaTime) override;

    void draw(sf::RenderWindow& window) override;
};
