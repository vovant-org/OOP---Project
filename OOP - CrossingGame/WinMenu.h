// WinMenu.h
#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

#include "Menu.h"
#include "Button.h"

class AudioManager;

//==================================================
// Button index
//==================================================

enum class WinMenuButton
{
    PlayAgain = 0,
    MainMenu
};

//==================================================
// Result
//==================================================

enum class WinMenuResult
{
    None = 0,
    PlayAgain,
    MainMenu
};

class WinMenu : public Menu
{
private:

    //----------------------------------
    // Overlay (nen mo, de nhin thay man hinh Playing dong lai
    // phia sau) + tieu de "YOU WIN"
    //----------------------------------

    sf::RectangleShape overlay;
    sf::Text titleText;
    sf::Text scoreText;

    bool hasFont = false;

    float canvasW = 1280.f;
    float canvasH = 720.f;

    //----------------------------------
    // Buttons
    //----------------------------------

    std::vector<Button> buttons;

    int selectedIndex = 0;

    //----------------------------------
    // Result
    //----------------------------------

    WinMenuResult result = WinMenuResult::None;

    // Audio
    AudioManager* audio = nullptr;

    //----------------------------------
    // Helper
    //----------------------------------

    void updateFocus();
    void moveSelectionUp();
    void moveSelectionDown();

public:

    WinMenu();

    // Audio
    void setAudioManager(AudioManager* manager);

    //----------------------------------
    // Overlay / text
    //----------------------------------

    // canvasW/canvasH: kich thuoc canvas logic hien tai (1280x720)
    void setOverlaySize(float canvasW, float canvasH);

    void setFont(const sf::Font& font);

    // Goi moi lan hien WinMenu de cap nhat noi dung score/level
    void setStats(int score, int level);

    //----------------------------------
    // Button
    //----------------------------------

    Button& getButton(WinMenuButton button);
    const Button& getButton(WinMenuButton button) const;
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

    WinMenuResult getResult() const;
    void clearResult();
};