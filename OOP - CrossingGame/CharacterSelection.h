#pragma once

#include "Menu.h"
#include "Button.h"

class CharacterSelection : public Menu
{
private:

    Button previousButton;
    Button nextButton;
    Button selectButton;
    Button backButton;

public:

    CharacterSelection();

    void processEvent(const sf::Event& event) override;

    void update(float deltaTime) override;

    void draw(sf::RenderWindow& window) override;
};
