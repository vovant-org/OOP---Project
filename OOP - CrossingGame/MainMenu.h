#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "Menu.h"
#include "MenuBackground.h"
#include "Button.h"

class AudioManager;

//==================================================
// Button
//==================================================

enum class MainMenuButton
{
    Play = 0,
    Continue,
    Settings,
    Leaderboard,
    About,
    Exit
};

//==================================================
// Result
//==================================================

enum class MainMenuResult
{
    None = 0,

    Play,
    Continue,
    Settings,
    Leaderboard,
    About,
    Exit
};

class MainMenu : public Menu
{
private:

    //----------------------------------
    // Background
    //----------------------------------

    MenuBackground background;

    //----------------------------------
    // Logo
    //----------------------------------

    sf::Sprite logo;

    //----------------------------------
    // Buttons
    //----------------------------------

    std::vector<Button> buttons;

    int selectedIndex = 0;

    //----------------------------------
    // Result
    //----------------------------------

    MainMenuResult result = MainMenuResult::None;

    // Audio
    AudioManager* audio = nullptr;

    //----------------------------------
    // Helper
    //----------------------------------

    void updateFocus();

    void moveSelectionUp();

    void moveSelectionDown();

public:

    MainMenu();

    // Audio
    void setAudioManager(AudioManager* manager);

    //----------------------------------
    // Background
    //----------------------------------

    void setBackgroundTexture(const sf::Texture& texture);

    void setBackgroundScale(float scaleX, float scaleY);

    //----------------------------------
    // Logo
    //----------------------------------

    void setLogoTexture(const sf::Texture& texture);

    void setLogoPosition(float x, float y);

    void setLogoScale(float scaleX, float scaleY);

    //----------------------------------
    // Button
    //----------------------------------

    Button& getButton(MainMenuButton button);

    const Button& getButton(MainMenuButton button) const;

    std::size_t getButtonCount() const;

    //----------------------------------
    // Menu
    //----------------------------------

    void processEvent(const sf::Event& event,
        const sf::RenderWindow& window) override;

    void update(float dt) override;

    void draw(sf::RenderWindow& window) const override;

    //----------------------------------
    // Result
    //----------------------------------

    MainMenuResult getResult() const;

    void clearResult();
};