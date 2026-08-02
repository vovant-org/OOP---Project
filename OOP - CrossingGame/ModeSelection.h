// ModeSelection.h
// Màn chọn độ khó — hiện ra sau khi chọn map, trước khi vào gameplay
#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include <string>

#include "Menu.h"
#include "Button.h"
#include "MenuBackground.h"

class AudioManager;

//==================================================
// Kết quả
//==================================================

enum class ModeSelectionResult
{
    None = 0,
    Selected,
    Back
};

//==================================================
// Dữ liệu từng mode
//==================================================

struct ModeInfo
{
    std::string name;
    sf::Color   color;   // màu tint cho SilverBox + chữ
};

class ModeSelection : public Menu
{
private:

    MenuBackground background;

    static constexpr int MODE_COUNT = 3;

    std::array<ModeInfo, MODE_COUNT> modeInfos =
    { {
        { "EASY",      sf::Color(154, 205, 50)  },   // xanh la chuoi
        { "HARD",      sf::Color(220, 20, 60)   },   // do
        { "NIGHTMARE", sf::Color(148, 0, 211)   }    // tim tuoi dam
    } };

    //--------------------------------------------------
    // Box (dung chung 1 texture SilverBox, tint mau rieng)
    //--------------------------------------------------

    sf::Texture boxTexture;
    std::array<sf::Sprite, MODE_COUNT> boxSprites;
    std::array<sf::Text, MODE_COUNT>   nameTexts;
    std::array<sf::RectangleShape, MODE_COUNT> highlightBorders;

    int selectedIndex = 0;

    //--------------------------------------------------
    // Buttons
    //--------------------------------------------------

    Button playButton;
    Button backButton;

    //--------------------------------------------------
    // Text
    //--------------------------------------------------

    sf::Font font;
    sf::Text titleText;
    sf::Text hintText;

    //--------------------------------------------------
    // Window
    //--------------------------------------------------

    float W = 1280.f;
    float H = 720.f;

    ModeSelectionResult result = ModeSelectionResult::None;

    AudioManager* audio = nullptr;

    //--------------------------------------------------
    // Helpers
    //--------------------------------------------------

    void updateHighlight();
    void selectPrev();
    void selectNext();
    void centerText(sf::Text& t, float cx, float y);

public:

    ModeSelection();

    void setAudioManager(AudioManager* manager);

    void setWindowSize(float w, float h);
    void setBackgroundTexture(const sf::Texture& tex, float sx, float sy);
    bool loadFont(const std::string& path);

    // Tai texture SilverBox (co the tai chinh anh dang dung o SettingMenu)
    bool loadBoxTexture(const std::string& path);

    void setupButtons(const sf::Texture& buttonTex,
        float btnW, float btnH,
        float scaleX, float scaleY);

    void setupLayout();

    ModeSelectionResult getResult() const;
    void clearResult();

    // 0 = Easy, 1 = Hard, 2 = Nightmare
    int getSelectedMode() const;

    void processEvent(const sf::Event& event,
        const sf::RenderWindow& window) override;

    void update(float dt) override;

    void draw(sf::RenderWindow& window) const override;
};
