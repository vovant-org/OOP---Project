#include "MainMenu.h"

//==================================================
// Constructor
//==================================================

MainMenu::MainMenu()
{
    buttons.resize(6);
}

//==================================================
// Texture
//==================================================

void MainMenu::setBackgroundTexture(const sf::Texture& texture)
{
    background.setTexture(texture);
}

void MainMenu::setBackgroundScale(float scaleX, float scaleY)
{
    background.setScale(scaleX, scaleY);
}

void MainMenu::setLogoTexture(const sf::Texture& texture)
{
    logo.setTexture(texture);
}

//==================================================
// Logo
//==================================================

void MainMenu::setLogoPosition(float x, float y)
{
    logo.setPosition(x, y);
}

void MainMenu::setLogoScale(float scaleX, float scaleY)
{
    logo.setScale(scaleX, scaleY);
}

//==================================================
// Button
//==================================================
Button& MainMenu::getButton(MainMenuButton button)
{
    return buttons.at(static_cast<std::size_t>(button));
}

const Button& MainMenu::getButton(MainMenuButton button) const
{
    return buttons.at(static_cast<std::size_t>(button));
}

std::size_t MainMenu::getButtonCount() const
{
    return buttons.size();
}

//==================================================
// Menu
//==================================================

void MainMenu::processEvent(const sf::Event& event,
    const sf::RenderWindow& window)
{
    for (auto& button : buttons)
    {
        button.processEvent(event, window);
    }
}

void MainMenu::update()
{
    for (auto& button : buttons)
    {
        button.update();
    }
}

void MainMenu::draw(sf::RenderWindow& window) const
{
    background.draw(window);

    window.draw(logo);

    for (const auto& button : buttons)
    {
        button.draw(window);
    }
}