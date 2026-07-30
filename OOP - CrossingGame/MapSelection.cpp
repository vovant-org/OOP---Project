// MapSelection.cpp
#include "MapSelection.h"
#include "AudioManager.h"
#include <iostream>
using namespace std;

void MapSelection::setAudioManager(AudioManager* manager)
{
    audio = manager;
}

//==================================================
// Helper: scale sprite theo kích thước mong muốn (px) thay vì
// dùng hệ số scale cố định — tránh icon bị to/lệch khi đổi ảnh
//==================================================
static void fitSpriteToSize(sf::Sprite& sp, float targetW, float targetH, bool flipX = false)
{
    sf::Vector2u sz = sp.getTexture()->getSize();
    if (sz.x == 0 || sz.y == 0) return;

    float sx = targetW / (float)sz.x;
    float sy = targetH / (float)sz.y;
    sp.setScale(flipX ? -sx : sx, sy);
}
//==================================================
// Constructor
//==================================================

MapSelection::MapSelection()
{
}

//==================================================
// Setup
//==================================================

void MapSelection::setWindowSize(float w, float h)
{
    W = w; H = h;
}

void MapSelection::setBackgroundTexture(const sf::Texture& tex,
    float sx, float sy)
{
    background.setTexture(tex);
    background.setScale(sx, sy);
}

bool MapSelection::loadFont(const std::string& path)
{
    if (!font.loadFromFile(path))
    {
        std::cout << "[MapSelection] Cannot load font: " << path << "\n";
        return false;
    }
    titleText.setFont(font);
    mapNameText.setFont(font);
    mapDescText.setFont(font);
    hintText.setFont(font);
    return true;
}

bool MapSelection::loadMapThumbnail(int index, const std::string& path)
{
    if (index < 0 || index >= MAP_COUNT) return false;
    if (!mapTextures[index].loadFromFile(path))
    {
        std::cout << "[MapSelection] Cannot load map thumbnail " << index
            << ": " << path << "\n";
        return false;
    }
    mapTextures[index].setSmooth(true);
    mapSprites[index].setTexture(mapTextures[index]);
    return true;
}

bool MapSelection::loadArrowTexture(const std::string& path)
{
    if (!leftArrowTexture.loadFromFile(path))
    {
        std::cout << "[MapSelection] Cannot load arrow: "
            << path << "\n";
        return false;
    }

    leftArrowTexture.setSmooth(true);
    return true;
}

void MapSelection::setupButtons(const sf::Texture& buttonTex,
    float btnW, float btnH,
    float scaleX, float scaleY)
{
    // Scale mũi tên theo kích thước px mong muốn (56x56), không phụ
    // thuộc độ phân giải ảnh gốc — tránh bị to/lệch tỉ lệ
    const float ARROW_SIZE = 56.f;
    sf::Vector2u arrSz = leftArrowTexture.getSize();
    if (arrSz.x > 0 && arrSz.y > 0)
    {
        float asx = ARROW_SIZE / (float)arrSz.x;
        float asy = ARROW_SIZE / (float)arrSz.y;

        prevButton.setTexture(leftArrowTexture);
        prevButton.setScale(asx, asy);

        nextButton.setTexture(leftArrowTexture);
        nextButton.setScale(-asx, asy);
    }

    playButton.setTexture(buttonTex);
    playButton.setScale(scaleX, scaleY);

    backButton.setTexture(buttonTex);
    backButton.setScale(scaleX, scaleY);
}

void MapSelection::setupLayout()
{
    float cx = W / 2.f;

    //--------------------------------------------------
    // Preview box (thumbnail map — chiếm phần lớn màn hình)
    //--------------------------------------------------
    float pvW = W * 0.60f;
    float pvH = H * 0.42f;      // giảm bớt để chừa chỗ cho tên map, mô tả, dots, nút
    float pvX = cx - pvW / 2.f;
    float pvY = H * 0.13f;

    previewBox.setSize({ pvW, pvH });
    previewBox.setPosition(pvX, pvY);
    previewBox.setFillColor(sf::Color(10, 15, 30, 180));

    previewBorder.setSize({ pvW + 8.f, pvH + 8.f });
    previewBorder.setPosition(pvX - 4.f, pvY - 4.f);
    previewBorder.setFillColor(sf::Color::Transparent);
    previewBorder.setOutlineThickness(4.f);
    // màu border sẽ update theo map được chọn

    //--------------------------------------------------
    // Title
    //--------------------------------------------------
    titleText.setString("SELECT MAP");
    titleText.setCharacterSize(44);
    titleText.setFillColor(sf::Color(255, 220, 80));
    titleText.setStyle(sf::Text::Bold);
    centerText(titleText, cx, H * 0.04f);

    //--------------------------------------------------
    // Map name
    //--------------------------------------------------
    mapNameText.setCharacterSize(30);
    mapNameText.setStyle(sf::Text::Bold);

    //--------------------------------------------------
    // Map description
    //--------------------------------------------------
    mapDescText.setCharacterSize(20);
    mapDescText.setFillColor(sf::Color(180, 190, 210));

    //--------------------------------------------------
    // Nút < >
    //--------------------------------------------------
    const float arrowHalf = 28.f; // ARROW_SIZE / 2
    prevButton.setText("");
    prevButton.setPosition(pvX - 50.f, pvY + pvH / 2.f - arrowHalf);

    nextButton.setText("");
    nextButton.setPosition(pvX + pvW + 50.f, pvY + pvH / 2.f - arrowHalf);

    //--------------------------------------------------
    // Dot indicators — ngay dưới mô tả map (được đặt lại trong updateTexts())
    //--------------------------------------------------
    float dotY = pvY + pvH + 116.f;  // chừa chỗ cho mapNameText + mapDescText
    float dotSpc = 32.f;
    float dotStartX = cx - (MAP_COUNT - 1) * dotSpc / 2.f;

    for (int i = 0; i < MAP_COUNT; i++)
    {
        dots[i].setRadius(7.f);
        dots[i].setOrigin(7.f, 7.f);
        dots[i].setPosition(dotStartX + i * dotSpc, dotY);
    }

    //--------------------------------------------------
    // Nút PLAY / BACK — đặt dưới dots, đủ chỗ trước hint
    //--------------------------------------------------
    float buttonY = H * 0.80f;

    playButton.setText("PLAY");
    playButton.setFont(font);
    playButton.setCharacterSize(26);
    playButton.setPosition(cx - 250.f, buttonY);

    backButton.setText("BACK");
    backButton.setFont(font);
    backButton.setCharacterSize(26);
    backButton.setPosition(cx + 10.f, buttonY);

    //--------------------------------------------------
    // Hint
    //--------------------------------------------------
    hintText.setString("LEFT / RIGHT  |  ENTER = Play  |  ESC = Back");
    hintText.setCharacterSize(18);
    hintText.setFillColor(sf::Color(120, 120, 140, 200));
    centerText(hintText, cx, H * 0.955f);

    //--------------------------------------------------
    // First update
    //--------------------------------------------------
    updatePreview();
    updateDots();
    updateTexts();
}

//==================================================
// Result
//==================================================

MapSelectionResult MapSelection::getResult() const { return result; }
void               MapSelection::clearResult() { result = MapSelectionResult::None; }
int                MapSelection::getSelectedIndex() const { return selectedIndex; }

//==================================================
// Navigation
//==================================================

void MapSelection::selectPrev()
{
    selectedIndex = (selectedIndex - 1 + MAP_COUNT) % MAP_COUNT;
    updatePreview();
    updateDots();
    updateTexts();
}

void MapSelection::selectNext()
{
    selectedIndex = (selectedIndex + 1) % MAP_COUNT;
    updatePreview();
    updateDots();
    updateTexts();
}

//==================================================
// updatePreview — scale thumbnail vừa preview box
//==================================================

void MapSelection::updatePreview()
{
    auto& tex = mapTextures[selectedIndex];
    auto& sp = mapSprites[selectedIndex];

    if (tex.getSize().x == 0) return;

    sp.setOrigin(0.f, 0.f);

    float pvW = previewBox.getSize().x;
    float pvH = previewBox.getSize().y;
    float scaleX = pvW / tex.getSize().x;
    float scaleY = pvH / tex.getSize().y;
    sp.setScale(scaleX, scaleY);
    sp.setPosition(previewBox.getPosition());

    // Cập nhật màu border theo map
    previewBorder.setOutlineColor(mapInfos[selectedIndex].themeColor);
}

void MapSelection::updateDots()
{
    for (int i = 0; i < MAP_COUNT; i++)
    {
        if (i == selectedIndex)
        {
            dots[i].setRadius(9.f);
            dots[i].setOrigin(9.f, 9.f);
            dots[i].setFillColor(mapInfos[selectedIndex].themeColor);
        }
        else
        {
            dots[i].setRadius(6.f);
            dots[i].setOrigin(6.f, 6.f);
            dots[i].setFillColor(sf::Color(70, 70, 90));
        }
    }
}

void MapSelection::updateTexts()
{
    const auto& m = mapInfos[selectedIndex];
    float cx = W / 2.f;
    float pvBot = previewBox.getPosition().y + previewBox.getSize().y;

    mapNameText.setString(m.name);
    mapNameText.setFillColor(m.themeColor);
    centerText(mapNameText, cx, pvBot + 34.f);

    mapDescText.setString(m.description);
    centerText(mapDescText, cx, pvBot + 72.f);
}

//==================================================
// processEvent
//==================================================

void MapSelection::processEvent(const sf::Event& event,
    const sf::RenderWindow& window)
{
    if (event.type == sf::Event::KeyPressed)
    {
        switch (event.key.code)
        {
        case sf::Keyboard::Left:
        case sf::Keyboard::A:
            selectPrev(); break;

        case sf::Keyboard::Right:
        case sf::Keyboard::D:
            selectNext(); break;

        case sf::Keyboard::Return:
            playButton.press();
            if (audio) audio->playSound("select");
            result = MapSelectionResult::Selected;
            break;

        case sf::Keyboard::Escape:
            backButton.press();
            if (audio) audio->playSound("select");
            result = MapSelectionResult::Back;
            break;

        default: break;
        }
    }

    prevButton.processEvent(event, window);
    nextButton.processEvent(event, window);
    playButton.processEvent(event, window);
    backButton.processEvent(event, window);

    if (event.type == sf::Event::MouseButtonReleased &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mp = window.mapPixelToCoords(
            sf::Mouse::getPosition(window));

        if (prevButton.contains(mp)) selectPrev();
        if (nextButton.contains(mp)) selectNext();
        if (playButton.contains(mp)) 
        {
            if (audio) audio->playSound("select");   // ===== ADDED =====
            result = MapSelectionResult::Selected;
        }
        if (backButton.contains(mp)) 
        {
            if (audio) audio->playSound("select");   // ===== ADDED =====
            result = MapSelectionResult::Back;
        }
    }
}

//==================================================
// update / draw
//==================================================

void MapSelection::update()
{
    prevButton.update();
    nextButton.update();
    playButton.update();
    backButton.update();
}

void MapSelection::draw(sf::RenderWindow& window) const
{
    background.draw(window);

    // Overlay
    sf::RectangleShape ov;
    ov.setSize({ W, H });
    ov.setFillColor(sf::Color(0, 0, 0, 120));
    window.draw(ov);

    window.draw(titleText);

    // Map thumbnail
    window.draw(previewBorder);
    if (mapTextures[selectedIndex].getSize().x > 0)
        window.draw(mapSprites[selectedIndex]);
    else
        window.draw(previewBox);   // fallback màu tối nếu chưa load ảnh

    // Texts
    window.draw(mapNameText);
    window.draw(mapDescText);

    // Dots
    for (const auto& dot : dots)
        window.draw(dot);

    // Buttons
    prevButton.draw(window);
    nextButton.draw(window);
    playButton.draw(window);
    backButton.draw(window);

    window.draw(hintText);
}

//==================================================
// Helpers
//==================================================

void MapSelection::centerText(sf::Text& t, float cx, float y)
{
    sf::FloatRect b = t.getLocalBounds();
    t.setOrigin(b.left + b.width / 2.f, b.top);
    t.setPosition(cx, y);
}