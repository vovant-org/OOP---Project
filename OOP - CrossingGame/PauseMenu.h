#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "Menu.h"
#include "Button.h"

class AudioManager;

//==================================================
// Button index
//==================================================

enum class PauseMenuButton
{
    Resume = 0,
    Restart,
    Settings,
    MainMenu
};

//==================================================
// Result
//==================================================

enum class PauseMenuResult
{
    None = 0,
    Resume,
    Restart,
    Settings,
    MainMenu
};

class PauseMenu : public Menu
{
private:

    sf::RectangleShape overlay;
    sf::Text titleText;
    bool hasFont = false;

    // ===== ADDED: luu lai canvas de can giua text/button khi can =====
    float canvasW = 1280.f;
    float canvasH = 720.f;

    std::vector<Button> buttons;

    int selectedIndex = 0;

    PauseMenuResult result = PauseMenuResult::None;

    AudioManager* audio = nullptr;

    void updateFocus();
    void moveSelectionUp();
    void moveSelectionDown();

public:

    PauseMenu();

    void setAudioManager(AudioManager* manager);

    void setOverlaySize(float canvasW, float canvasH);
    void setFont(const sf::Font& font);

    Button& getButton(PauseMenuButton button);
    const Button& getButton(PauseMenuButton button) const;
    std::size_t getButtonCount() const;

    void processEvent(const sf::Event& event,
        const sf::RenderWindow& window) override;

    void update(float dt) override;

    void draw(sf::RenderWindow& window) const override;

    PauseMenuResult getResult() const;
    void clearResult();
};