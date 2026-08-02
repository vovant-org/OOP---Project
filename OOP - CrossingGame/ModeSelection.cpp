// ModeSelection.cpp
#include "ModeSelection.h"
#include "AudioManager.h"
#include <iostream>

//==================================================
// Constructor
//==================================================

ModeSelection::ModeSelection()
{
}

//==================================================
// Setup
//==================================================

void ModeSelection::setAudioManager(AudioManager* manager)
{
    audio = manager;
}

void ModeSelection::setWindowSize(float w, float h)
{
    W = w;
    H = h;
}

void ModeSelection::setBackgroundTexture(const sf::Texture& tex,
    float sx, float sy)
{
    background.setTexture(tex);
    background.setScale(sx, sy);
}

bool ModeSelection::loadFont(const std::string& path)
{
    if (!font.loadFromFile(path))
    {
        std::cout << "[ModeSelection] Cannot load font: " << path << "\n";
        return false;
    }
    titleText.setFont(font);
    hintText.setFont(font);

    for (auto& t : nameTexts)
        t.setFont(font);

    return true;
}

bool ModeSelection::loadBoxTexture(const std::string& path)
{
    if (!boxTexture.loadFromFile(path))
    {
        std::cout << "[ModeSelection] Cannot load box texture: "
            << path << "\n";
        return false;
    }

    boxTexture.setSmooth(true);

    for (auto& sp : boxSprites)
        sp.setTexture(boxTexture);

    return true;
}

void ModeSelection::setupButtons(const sf::Texture& buttonTex,
    float btnW, float btnH,
    float scaleX, float scaleY)
{
    (void)btnW;
    (void)btnH;

    playButton.setTexture(buttonTex);
    playButton.setScale(scaleX, scaleY);

    backButton.setTexture(buttonTex);
    backButton.setScale(scaleX, scaleY);
}

//==================================================
// Layout
//==================================================

void ModeSelection::setupLayout()
{
    float cx = W / 2.f;

    //--------------------------------------------------
    // Title
    //--------------------------------------------------
    titleText.setString("SELECT MODE");
    titleText.setCharacterSize(44);
    titleText.setFillColor(sf::Color(255, 220, 80));
    titleText.setStyle(sf::Text::Bold);
    centerText(titleText, cx, H * 0.06f);

    //--------------------------------------------------
    // 3 box xep ngang, chia deu
    //--------------------------------------------------
    const float BOX_SIZE = 240.f;
    const float GAP = 50.f;
    const float totalWidth = MODE_COUNT * BOX_SIZE + (MODE_COUNT - 1) * GAP;
    const float startX = cx - totalWidth / 2.f;
    const float boxY = H * 0.28f;

    for (int i = 0; i < MODE_COUNT; i++)
    {
        float bx = startX + i * (BOX_SIZE + GAP);

        // Box (tint theo mau rieng cua mode)
        sf::Vector2u sz = boxTexture.getSize();
        if (sz.x > 0 && sz.y > 0)
        {
            float scaleX = BOX_SIZE / (float)sz.x;
            float scaleY = BOX_SIZE / (float)sz.y;
            boxSprites[i].setScale(scaleX, scaleY);
        }
        boxSprites[i].setPosition(bx, boxY);
        boxSprites[i].setColor(modeInfos[i].color);

        // Vien highlight quanh box (chi hien khi dang chon)
        highlightBorders[i].setSize({ BOX_SIZE, BOX_SIZE });
        highlightBorders[i].setPosition(bx, boxY);
        highlightBorders[i].setFillColor(sf::Color::Transparent);
        highlightBorders[i].setOutlineThickness(5.f);
        highlightBorders[i].setOutlineColor(sf::Color::White);

        // Ten mode, can giua trong box
        nameTexts[i].setString(modeInfos[i].name);
        nameTexts[i].setCharacterSize(22);
        nameTexts[i].setStyle(sf::Text::Bold);
        nameTexts[i].setFillColor(sf::Color::White);
        nameTexts[i].setOutlineColor(sf::Color::Black);
        nameTexts[i].setOutlineThickness(2.f);
        centerText(nameTexts[i], bx + BOX_SIZE / 2.f, boxY + BOX_SIZE / 2.f - 14.f);
    }

    //--------------------------------------------------
    // Nut PLAY / BACK
    //--------------------------------------------------
    float buttonY = H * 0.62f;

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
    hintText.setString("LEFT / RIGHT chon do kho  |  ENTER = Play  |  ESC = Back");
    hintText.setCharacterSize(18);
    hintText.setFillColor(sf::Color(120, 120, 140, 200));
    centerText(hintText, cx, H * 0.955f);

    updateHighlight();
}

//==================================================
// Result
//==================================================

ModeSelectionResult ModeSelection::getResult() const { return result; }
void                ModeSelection::clearResult() { result = ModeSelectionResult::None; }
int                 ModeSelection::getSelectedMode() const { return selectedIndex; }

//==================================================
// Navigation
//==================================================

void ModeSelection::selectPrev()
{
    selectedIndex = (selectedIndex - 1 + MODE_COUNT) % MODE_COUNT;
    updateHighlight();
}

void ModeSelection::selectNext()
{
    selectedIndex = (selectedIndex + 1) % MODE_COUNT;
    updateHighlight();
}

void ModeSelection::updateHighlight()
{
    for (int i = 0; i < MODE_COUNT; i++)
        highlightBorders[i].setOutlineColor(
            i == selectedIndex ? sf::Color::White : sf::Color::Transparent);
}

//==================================================
// processEvent
//==================================================

void ModeSelection::processEvent(const sf::Event& event,
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
            result = ModeSelectionResult::Selected;
            break;

        case sf::Keyboard::Escape:
            backButton.press();
            if (audio) audio->playSound("select");
            result = ModeSelectionResult::Back;
            break;

        default: break;
        }
    }

    playButton.processEvent(event, window);
    backButton.processEvent(event, window);

    if (event.type == sf::Event::MouseButtonReleased &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mp = window.mapPixelToCoords(
            sf::Mouse::getPosition(window));

        for (int i = 0; i < MODE_COUNT; i++)
        {
            if (boxSprites[i].getGlobalBounds().contains(mp))
            {
                selectedIndex = i;
                updateHighlight();
                if (audio) audio->playSound("select");
            }
        }

        if (playButton.contains(mp))
        {
            playButton.press();
            if (audio) audio->playSound("select");
            result = ModeSelectionResult::Selected;
        }

        if (backButton.contains(mp))
        {
            backButton.press();
            if (audio) audio->playSound("select");
            result = ModeSelectionResult::Back;
        }
    }
}

//==================================================
// update / draw
//==================================================

void ModeSelection::update(float dt)
{
    (void)dt;

    playButton.update();
    backButton.update();
}

void ModeSelection::draw(sf::RenderWindow& window) const
{
    background.draw(window);

    sf::RectangleShape ov;
    ov.setSize({ W, H });
    ov.setFillColor(sf::Color(0, 0, 0, 120));
    window.draw(ov);

    window.draw(titleText);

    for (int i = 0; i < MODE_COUNT; i++)
    {
        window.draw(boxSprites[i]);
        window.draw(highlightBorders[i]);
        window.draw(nameTexts[i]);
    }

    playButton.draw(window);
    backButton.draw(window);

    window.draw(hintText);
}

//==================================================
// Helpers
//==================================================

void ModeSelection::centerText(sf::Text& t, float cx, float y)
{
    sf::FloatRect b = t.getLocalBounds();
    t.setOrigin(b.left + b.width / 2.f, b.top);
    t.setPosition(cx, y);
}