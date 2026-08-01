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

enum class GameOverMenuButton
{
    Retry = 0,
    MainMenu
};

//==================================================
// Result
//==================================================

enum class GameOverMenuResult
{
    None = 0,
    Retry,
    MainMenu
};

class GameOverMenu : public Menu
{
private:

    //----------------------------------
    // Overlay (nen mo den phia sau, de nhin thay man hinh
    // Playing/CGAME dong lai phia sau) + tieu de "GAME OVER"
    //----------------------------------

    sf::RectangleShape overlay;
    sf::Text titleText;
    sf::Text scoreText;

    bool hasFont = false;

    // ===== ADDED: luu lai canvas de can giua text khi setStats() goi lai =====
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

    GameOverMenuResult result = GameOverMenuResult::None;

    // Audio
    AudioManager* audio = nullptr;

    //----------------------------------
    // Helper
    //----------------------------------

    void updateFocus();
    void moveSelectionUp();
    void moveSelectionDown();

public:

    GameOverMenu();

    // Audio
    void setAudioManager(AudioManager* manager);

    //----------------------------------
    // Overlay / text
    //----------------------------------

    // canvasW/canvasH: kich thuoc canvas logic hien tai (1280x720)
    void setOverlaySize(float canvasW, float canvasH);

    void setFont(const sf::Font& font);

    // Goi moi lan hien GameOver de cap nhat noi dung score/level
    void setStats(int score, int level);

    //----------------------------------
    // Button
    //----------------------------------

    Button& getButton(GameOverMenuButton button);
    const Button& getButton(GameOverMenuButton button) const;
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

    GameOverMenuResult getResult() const;
    void clearResult();
};