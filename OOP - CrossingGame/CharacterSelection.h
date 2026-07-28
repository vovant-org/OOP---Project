// CharacterSelection.h
#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <array>

#include "Menu.h"
#include "Button.h"
#include "MenuBackground.h"

enum class CharacterSelectionResult
{
    None = 0,
    Selected,
    Back
};

struct CharacterInfo
{
    std::string name;
    int speed;
    int hp;
    std::string skill;
};

class CharacterSelection : public Menu
{
private:

    MenuBackground background;

    static constexpr int CHARACTER_COUNT = 4;

    std::array<CharacterInfo, CHARACTER_COUNT> charInfos =
    { {
        { "Chicken", 4, 3, "Jump"  },
        { "Knight",  3, 5, "Block" },
        { "Dog",     5, 2, "Dash"  },
        { "Luffy",   4, 4, "Gear"  }
    } };

    //--------------------------------------------------
    // Character Preview
    //--------------------------------------------------

    std::array<sf::Texture, CHARACTER_COUNT> charTextures;
    std::array<sf::Sprite, CHARACTER_COUNT> charSprites;

    //--------------------------------------------------
    // UI Textures
    //--------------------------------------------------

    sf::Texture leftArrowTexture;
    sf::Texture heartTexture;
    sf::Texture lightningTexture;

    //--------------------------------------------------
    // UI Sprites
    //--------------------------------------------------

    sf::Sprite heartIcon;
    sf::Sprite lightningIcon;

    int selectedIndex = 0;
    int previewFrame = 0;
    float frameTimer = 0.f;
    float frameDuration = 0.12f;

    int frameW = 0;
    int frameH = 0;

    //--------------------------------------------------
    // Buttons
    //--------------------------------------------------

    Button prevButton;
    Button nextButton;
    Button selectButton;
    Button backButton;

    //--------------------------------------------------
    // Text
    //--------------------------------------------------

    sf::Font font;

    sf::Text titleText;
    sf::Text charNameText;
    sf::Text statSpeedText;
    sf::Text statHPText;
    sf::Text statSkillText;
    sf::Text hintText;

    //--------------------------------------------------
    // Bottom Indicators
    //--------------------------------------------------

    std::array<sf::CircleShape, CHARACTER_COUNT> dots;

    //--------------------------------------------------
    // Window
    //--------------------------------------------------

    float W = 1280.f;
    float H = 720.f;

    //--------------------------------------------------
    // Boxes
    //--------------------------------------------------

    sf::RectangleShape previewBox;
    sf::RectangleShape infoBox;
    sf::RectangleShape previewBorder;

    CharacterSelectionResult result = CharacterSelectionResult::None;

    //--------------------------------------------------
    // Helpers
    //--------------------------------------------------

    void updatePreview();
    void updateDots();
    void updateStatsText();

    void selectPrev();
    void selectNext();

    void centerText(sf::Text& t, float cx, float y);

    // Draw Heart / Lightning icons
    void drawIcons(sf::RenderWindow& window) const;

public:

    CharacterSelection();

    void setWindowSize(float w, float h);

    void setBackgroundTexture(const sf::Texture& tex,
        float sx,
        float sy);

    bool loadFont(const std::string& path);

    bool loadCharacterTexture(int index,
        const std::string& path);

    // Load UI Icons
    bool loadUITextures(const std::string& arrow,
        const std::string& heart,
        const std::string& lightning);

    void setupButtons(const sf::Texture& buttonTex,
        float btnW,
        float btnH,
        float scaleX,
        float scaleY);

    void setupLayout();

    CharacterSelectionResult getResult() const;
    void clearResult();

    int getSelectedIndex() const;

    void processEvent(const sf::Event& event,
        const sf::RenderWindow& window) override;

    void update() override;
    void update(float deltaTime);

    void draw(sf::RenderWindow& window) const override;
};