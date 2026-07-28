// MapSelection.h
// Màn hình chọn map — hiện ra sau khi người dùng chọn nhân vật xong
#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include <string>

#include "Menu.h"
#include "Button.h"
#include "MenuBackground.h"

// ── Kết quả ──────────────────────────────────────────────────
enum class MapSelectionResult
{
    None = 0,
    Selected,
    Back
};

// ── Dữ liệu từng map ─────────────────────────────────────────
struct MapInfo
{
    std::string name;
    std::string description;
    std::string thumbnailPath;   // ảnh thu nhỏ của map
    sf::Color   themeColor;      // màu chủ đạo cho border/highlight
};

class MapSelection : public Menu
{
private:

    MenuBackground background;

    static constexpr int MAP_COUNT = 4;

    std::array<MapInfo, MAP_COUNT> mapInfos =
    { {
        { "CITY",    "Urban roads with bikes, motors & trucks",
          "Map/City_map.png",    sf::Color(100, 200, 255) },
        { "ANCIENT", "Jungle trails with dinos, crocs & beasts",
          "Map/Ancient_map.png", sf::Color(100, 220, 100) },
        { "HELL",    "Inferno lanes with trains, brutes & piglins",
          "Map/Hell_map.png",    sf::Color(255, 100,  60) },
        { "SKY",     "Cloud paths with birds, angels & clouds",
          "Map/Sky_map.png",     sf::Color(200, 180, 255) }
    } };

    std::array<sf::Texture, MAP_COUNT> mapTextures;
    std::array<sf::Sprite, MAP_COUNT> mapSprites;

    int selectedIndex = 0;

    // Buttons
    Button prevButton;
    Button nextButton;
    Button playButton;
    Button backButton;

    // Font & texts
    sf::Font font;
    sf::Text titleText;
    sf::Text mapNameText;
    sf::Text mapDescText;
    sf::Text hintText;

    // Layout
    float W = 1280.f;
    float H = 720.f;

    sf::RectangleShape previewBox;
    sf::RectangleShape previewBorder;

    // Dot indicators
    std::array<sf::CircleShape, MAP_COUNT> dots;

    // Result
    MapSelectionResult result = MapSelectionResult::None;

    // Helpers
    void updatePreview();
    void updateDots();
    void updateTexts();
    void selectPrev();
    void selectNext();
    void centerText(sf::Text& t, float cx, float y);

public:

    MapSelection();

    void setWindowSize(float w, float h);
    void setBackgroundTexture(const sf::Texture& tex, float sx, float sy);
    bool loadFont(const std::string& path);
    bool loadMapThumbnail(int index, const std::string& path);
    void setupButtons(const sf::Texture& buttonTex,
        float btnW, float btnH,
        float scaleX, float scaleY);
    void setupLayout();

    MapSelectionResult getResult()       const;
    void               clearResult();
    int                getSelectedIndex() const;

    void processEvent(const sf::Event& event,
        const sf::RenderWindow& window) override;
    void update() override;
    void draw(sf::RenderWindow& window) const override;
};
