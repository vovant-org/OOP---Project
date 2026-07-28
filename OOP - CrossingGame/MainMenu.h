#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "Menu.h"
#include "MenuBackground.h"
#include "Button.h"

// Xác định các button trong Main Menu
enum class MainMenuButton
{
    Play = 0,
    Continue,
    Settings,
    Leaderboard,
    About,
    Exit
};

class MainMenu : public Menu
{
private:

    // Background
    MenuBackground background;

    // Logo
    sf::Sprite logo;

    // Buttons
    std::vector<Button> buttons;

public:

    MainMenu();

    //==============================
    // Background
    //==============================
    void setBackgroundTexture(const sf::Texture& texture);
    void setBackgroundScale(float scaleX, float scaleY);

    //==============================
    // Logo
    //==============================
    void setLogoTexture(const sf::Texture& texture);
    void setLogoPosition(float x, float y);
    void setLogoScale(float scaleX, float scaleY);

    //==============================
    // Buttons
    //==============================
    Button& getButton(MainMenuButton button);
    const Button& getButton(MainMenuButton button) const;

    std::size_t getButtonCount() const;

    //==============================
    // Menu
    //==============================
    void processEvent(const sf::Event& event,
        const sf::RenderWindow& window) override;

    void update() override;

    void draw(sf::RenderWindow& window) const override;
};