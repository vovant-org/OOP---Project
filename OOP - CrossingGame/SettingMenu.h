#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include<functional>

#include "Menu.h"
#include "Button.h"

class AudioManager;

//==================================================
// Result
//==================================================

enum class SettingMenuResult
{
    None = 0,
    Back
};

class SettingMenu : public Menu
{
private:

    //----------------------------------
    // Result
    //----------------------------------

    SettingMenuResult result = SettingMenuResult::None;

    //----------------------------------
    // Background
    //----------------------------------

    sf::Sprite backgroundSprite;

    sf::Sprite panelSprite;

    //----------------------------------
    // Value Boxes
    //----------------------------------

    sf::Sprite musicBox;
    sf::Sprite soundBox;
    sf::Sprite fpsBox;
    sf::Sprite resolutionBox;

    //----------------------------------
    // Label Boxes (SilverBox trang trí, không tương tác)
    // 3 mảnh: trái (bo góc) - giữa (co giãn) - phải (bo góc),
    // giúp không bị méo khi kéo rộng ra.
    //----------------------------------

    struct LabelBoxSprite
    {
        sf::Sprite left;
        sf::Sprite mid;
        sf::Sprite right;
    };

    LabelBoxSprite musicLabelBox;
    LabelBoxSprite soundLabelBox;
    LabelBoxSprite fullscreenLabelBox;
    LabelBoxSprite fpsLabelBox;
    LabelBoxSprite resolutionLabelBox;

    //----------------------------------
    // Icons
    //----------------------------------

    sf::Sprite musicMinusSprite;
    sf::Sprite musicPlusSprite;

    sf::Sprite soundMinusSprite;
    sf::Sprite soundPlusSprite;

    sf::Sprite fpsMinusSprite;
    sf::Sprite fpsPlusSprite;

    sf::Sprite resolutionMinusSprite;
    sf::Sprite resolutionPlusSprite;

    const sf::Texture* switchOnTexture = nullptr;
    const sf::Texture* switchOffTexture = nullptr;

    sf::Sprite fullscreenSprite;

    sf::Sprite resolutionTimesSprite;

    //----------------------------------
    // Back Button
    //----------------------------------

    Button backButton;

    //----------------------------------
    // Font
    //----------------------------------

    sf::Font font;

    //----------------------------------
    // Title
    //----------------------------------

    sf::Sprite title;

    //----------------------------------
    // Labels
    //----------------------------------

    sf::Text musicLabel;
    sf::Text soundLabel;
    sf::Text fullscreenLabel;
    sf::Text fpsLabel;
    sf::Text resolutionLabel;

    //----------------------------------
    // Values
    //----------------------------------

    sf::Text musicValueText;
    sf::Text soundValueText;

    sf::Text fpsValueText;

    sf::Text resolutionWidthText;
    sf::Text resolutionHeightText;

    //----------------------------------
    // Setting Values
    //----------------------------------

    int musicVolume = 100;
    std::function<void(int)> onMusicVolumeChanged;

    int soundVolume = 100;

    bool fullscreen = false;

    int fpsLimit = 60;

    static constexpr int RESOLUTION_COUNT = 4;

    sf::Vector2u resolutions[RESOLUTION_COUNT] =
    {
        {1280, 720},
        {1600, 900},
        {1920,1080},
        {2560,1440}
    };

    int resolutionIndex = 2;

    AudioManager* audio = nullptr;

    //----------------------------------
    // Helper Functions
    //----------------------------------

    void initializeObjects();

    void updateTexts();

    // ===== ADDED: helper cho label box 3-slice (không méo khi kéo rộng) =====
    static void setLabelBoxTexture(LabelBoxSprite& box,
        const sf::Texture& texture);

    static void setLabelBoxPosition(LabelBoxSprite& box,
        float x, float y);

    static void drawLabelBox(sf::RenderWindow& window,
        const LabelBoxSprite& box);

    bool isMouseOver(const sf::Sprite& sprite,
        const sf::RenderWindow& window) const;

public:

    //----------------------------------
    // Constructor
    //----------------------------------

    SettingMenu();

    void setAudioManager(AudioManager* manager);

    //----------------------------------
    // Texture Setters
    //----------------------------------

    void setPanelTexture(const sf::Texture& texture);

    // ===== ADDED: background giống màn Select Character =====
    void setBackgroundTexture(const sf::Texture& texture,
        float scaleX, float scaleY);

    void setValueBoxTexture(const sf::Texture& texture);

    void setPlusTexture(const sf::Texture& texture);

    void setMinusTexture(const sf::Texture& texture);

    void setSwitchTextures(const sf::Texture& onTexture,
        const sf::Texture& offTexture);

    void setTimesTexture(const sf::Texture& texture);

    // ===== ADDED =====
    void setBackButtonTexture(const sf::Texture& texture);

    // ===== ADDED: ảnh chữ "SETTING" thay cho sf::Text =====
    void setTitleTexture(const sf::Texture& texture);

    //----------------------------------
    // Font
    //----------------------------------

    void setFont(const sf::Font& newFont);

    //----------------------------------
    // Menu
    //----------------------------------

    void processEvent(const sf::Event& event,
        const sf::RenderWindow& window) override;

    void update() override;

    void draw(sf::RenderWindow& window) const override;

    //----------------------------------
    // Result
    //----------------------------------

    SettingMenuResult getResult() const;

    void clearResult();

    //----------------------------------
    // Getters
    //----------------------------------

    int getMusicVolume() const;

    int getSoundVolume() const;

    bool isFullscreen() const;

    int getFPSLimit() const;

    sf::Vector2u getResolution() const;

    //----------------------------------
    // Setters
    //----------------------------------

    void setMusicVolume(int volume);

    void setOnMusicVolumeChanged(std::function<void(int)> callback);

    void setSoundVolume(int volume);

    void setFullscreen(bool enable);

    void setFPSLimit(int fps);

    void setResolution(int index);
};