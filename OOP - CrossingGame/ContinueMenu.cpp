// ContinueMenu.cpp
#include "ContinueMenu.h"
#include "AudioManager.h"
#include "CGAME.h"

#include <sstream>
#include <algorithm>
#include <ctime>

namespace
{
    const char* MAP_NAMES[4] = { "CITY", "ANCIENT", "HELL", "SKY" };
    const char* MODE_NAMES[3] = { "EASY", "HARD", "NIGHTMARE" };

    // Layout
    constexpr float SLOT_W_FRAC = 0.82f;
    constexpr float SLOT_H = 120.f;
    constexpr float SLOT_GAP = 20.f;
    constexpr float START_Y = 120.f;
    constexpr float CHAR_IMG_SIZE = 96.f;

    // 3-slice content for silver/golden box (match AboutMenu / Leaderboard)
    const sf::IntRect SILVERBOX_CONTENT(86, 323, 1374, 366);
    constexpr float SILVER_CAP_FRAC = 0.27f;

    void drawThreeSliceLocal(sf::RenderWindow& window, const sf::Texture& tex,
        const sf::IntRect& content, float x, float y, float w, float h)
    {
        int capPx = static_cast<int>(content.width * SILVER_CAP_FRAC);
        if (capPx * 2 > content.width)
            capPx = content.width / 2;

        float scale = h / static_cast<float>(content.height);
        float capScreenW = capPx * scale;

        sf::Sprite left;
        left.setTexture(tex);
        left.setTextureRect(sf::IntRect(content.left, content.top, capPx, content.height));
        left.setScale(scale, scale);
        left.setPosition(x, y);
        window.draw(left);

        sf::Sprite right;
        right.setTexture(tex);
        right.setTextureRect(sf::IntRect(content.left + content.width - capPx, content.top,
            capPx, content.height));
        right.setScale(scale, scale);
        right.setPosition(x + w - capScreenW, y);
        window.draw(right);

        int midSrcW = content.width - capPx * 2;
        float midScreenW = w - capScreenW * 2.f;

        if (midSrcW > 0 && midScreenW > 0.f)
        {
            sf::Sprite mid;
            mid.setTexture(tex);
            mid.setTextureRect(sf::IntRect(content.left + capPx, content.top, midSrcW, content.height));
            mid.setScale(midScreenW / static_cast<float>(midSrcW), scale);
            mid.setPosition(x + capScreenW, y);
            window.draw(mid);
        }
    }
}

//==================================================
// Helper
//==================================================

void ContinueMenu::centerText(sf::Text& t, float cx, float y)
{
    sf::FloatRect b = t.getLocalBounds();
    t.setOrigin(b.left + b.width / 2.f, b.top);
    t.setPosition(cx, y);
}

void ContinueMenu::centerTextFull(sf::Text& t, float cx, float cy)
{
    sf::FloatRect b = t.getLocalBounds();
    t.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
    t.setPosition(cx, cy);
}

//==================================================
// Constructor
//==================================================

ContinueMenu::ContinueMenu()
{
}

//==================================================
// Setup
//==================================================

void ContinueMenu::setAudioManager(AudioManager* manager)
{
    audio = manager;
}

void ContinueMenu::setWindowSize(float w, float h)
{
    W = w;
    H = h;
}

void ContinueMenu::setBackgroundTexture(const sf::Texture& tex, float sx, float sy)
{
    background.setTexture(tex);
    background.setScale(sx, sy);
}

void ContinueMenu::setFont(const sf::Font& f)
{
    font = &f;

    titleText.setFont(f);
    titleText.setString("CONTINUE");
    titleText.setCharacterSize(44);
    titleText.setStyle(sf::Text::Bold);
    titleText.setFillColor(sf::Color(255, 220, 80));
    centerText(titleText, W / 2.f, H * 0.06f);

    hintText.setFont(f);
    hintText.setString("Chua co tien trinh nao duoc luu");
    hintText.setCharacterSize(22);
    hintText.setFillColor(sf::Color(200, 200, 200));
    centerText(hintText, W / 2.f, H * 0.5f);

    backButton.setFont(f);
    backButton.setText("BACK");
    backButton.setCharacterSize(26);
    // No per-mode labels in slot-based continue menu
}

void ContinueMenu::setButtonTexture(const sf::Texture& tex, float scaleX, float scaleY)
{
    buttonTexture = &tex;
    btnScaleX = scaleX;
    btnScaleY = scaleY;
    btnRenderW = tex.getSize().x * scaleX;
    btnRenderH = tex.getSize().y * scaleY;

    backButton.setTexture(tex);
    backButton.setScale(scaleX, scaleY);
}

void ContinueMenu::setModeBoxTexture(const sf::Texture& tex)
{
    modeBoxTexture = &tex;
}

void ContinueMenu::setSlotTexture(const sf::Texture& tex)
{
    slotBoxTexture = &tex;
}

//==================================================
// Refresh - read 4 save slots
//==================================================

void ContinueMenu::refresh()
{
    // load character textures once
    if (!charTexturesLoaded)
    {
        const std::string paths[4] = {
            "Character/Chicken_character.png",
            "Character/Knight_character.png",
            "Character/Dog_character.png",
            "Character/Luffy_character.png"
        };
        for (int i = 0; i < 4; ++i)
        {
            charTextures[i].loadFromFile(paths[i]);
        }
        charTexturesLoaded = true;
    }

    slotButtons.clear();
    slotButtons.resize(SLOT_COUNT);

    float slotW = W * SLOT_W_FRAC;
    float x = W / 2.f - slotW / 2.f;
    float y = START_Y;

    for (int i = 0; i < SLOT_COUNT; ++i)
    {
        CGAME::SaveData sd;
        std::string path = CGAME::GetSavePathForSlot(i);
        if (CGAME::PeekSaveData(path, sd))
        {
            slotInfo[i].exists = true;
            slotInfo[i].characterIndex = sd.characterIndex;
            slotInfo[i].playerName = sd.playerName;
            slotInfo[i].mapIndex = sd.mapIndex;
            slotInfo[i].score = sd.score;
            slotInfo[i].difficultyMode = sd.difficultyMode;
            slotInfo[i].level = sd.level;
            slotInfo[i].saveTime = sd.saveTime;
        }
        else
        {
            slotInfo[i] = SlotInfo();
        }

        Button btn;
        if (buttonTexture)
        {
            btn.setTexture(*buttonTexture);
            btn.setScale(btnScaleX, btnScaleY);
        }
        if (font) btn.setFont(*font);

        btn.setCharacterSize(18);
        btn.setPosition(x, y);
        btn.setFocused(false);
        slotButtons[i] = btn;

        y += SLOT_H + SLOT_GAP;
    }

    // back button
    backButton.setPosition(W / 2.f - btnRenderW / 2.f, H - 80.f);

    // select first slot by default
    selectedIndex = 0;
    onBack = false;
    selectedSlotIndex = -1;
    result = ContinueMenuResult::None;

    updateFocus();
}

//==================================================
// Focus / Navigation
//==================================================

void ContinueMenu::updateFocus()
{
    for (auto& b : slotButtons)
        b.setFocused(false);

    backButton.setFocused(false);

    if (!onBack && selectedIndex >= 0 && selectedIndex < static_cast<int>(slotButtons.size()))
    {
        slotButtons[selectedIndex].setFocused(true);
    }
    else
    {
        backButton.setFocused(true);
    }
}

void ContinueMenu::moveVertical(int dir)
{
    if (onBack)
    {
        // move from back to last slot (or first)
        selectedIndex = (dir < 0) ? static_cast<int>(slotButtons.size()) - 1 : 0;
        onBack = false;
    }
    else
    {
        selectedIndex += dir;
        if (selectedIndex < 0)
        {
            onBack = true;
        }
        else if (selectedIndex >= static_cast<int>(slotButtons.size()))
        {
            onBack = true;
        }
    }

    // clamp
    if (selectedIndex < 0) selectedIndex = 0;
    if (selectedIndex >= static_cast<int>(slotButtons.size())) selectedIndex = static_cast<int>(slotButtons.size()) - 1;

    updateFocus();
}

void ContinueMenu::activateSelected()
{
    if (onBack)
    {
        backButton.press();
        if (audio) audio->playSound("select");
        result = ContinueMenuResult::Back;
        return;
    }

    if (selectedIndex < 0 || selectedIndex >= static_cast<int>(slotButtons.size()))
        return;

    slotButtons[selectedIndex].press();
    if (audio) audio->playSound("select");
    selectedSlotIndex = selectedIndex;
    result = ContinueMenuResult::Selected;
}

//==================================================
// Result
//==================================================

ContinueMenuResult ContinueMenu::getResult() const
{
    return result;
}

void ContinueMenu::clearResult()
{
    result = ContinueMenuResult::None;
}

//==================================================
// Menu
//==================================================

void ContinueMenu::processEvent(const sf::Event& event,
    const sf::RenderWindow& window)
{
    //-----------------------------
    // Keyboard
    //-----------------------------

    if (event.type == sf::Event::KeyPressed)
    {
        switch (event.key.code)
        {
        case sf::Keyboard::Up:
        case sf::Keyboard::W:
            moveVertical(-1);
            break;

        case sf::Keyboard::Down:
        case sf::Keyboard::S:
            moveVertical(1);
            break;

        case sf::Keyboard::Enter:
            activateSelected();
            break;

        case sf::Keyboard::Escape:
            backButton.press();
            if (audio) audio->playSound("select");
            result = ContinueMenuResult::Back;
            break;

        default:
            break;
        }
    }

    //-----------------------------
    //-----------------------------
    for (auto& b : slotButtons)
        b.processEvent(event, window);

    backButton.processEvent(event, window);

    if (event.type == sf::Event::MouseButtonReleased &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mp = window.mapPixelToCoords(
            { event.mouseButton.x, event.mouseButton.y });

        for (std::size_t i = 0; i < slotButtons.size(); ++i)
        {
            if (slotButtons[i].contains(mp))
            {
                selectedIndex = static_cast<int>(i);
                onBack = false;
                activateSelected();
                return;
            }
        }

        if (backButton.contains(mp))
        {
            onBack = true;
            activateSelected();
        }
    }
}

void ContinueMenu::update(float dt)
{
    (void)dt;
    for (auto& b : slotButtons) b.update();
    backButton.update();
}

void ContinueMenu::draw(sf::RenderWindow& window) const
{
    background.draw(window);
    window.draw(titleText);

    float slotW = W * SLOT_W_FRAC;
    float x = W / 2.f - slotW / 2.f;
    float y = START_Y;

    for (int i = 0; i < SLOT_COUNT; ++i)
    {
        // draw slot background using three-slice to preserve corners
        if (slotBoxTexture)
        {
            drawThreeSliceLocal(window, *slotBoxTexture, SILVERBOX_CONTENT, x, y, slotW, SLOT_H);
        }
        else if (buttonTexture)
        {
            sf::Sprite s;
            s.setTexture(*buttonTexture);
            float sx = slotW / btnRenderW;
            float sy = SLOT_H / btnRenderH;
            s.setScale(sx, sy);
            s.setPosition(x, y);
            window.draw(s);
        }
        else
        {
            sf::RectangleShape rect({ slotW, SLOT_H });
            rect.setPosition(x, y);
            rect.setFillColor(sf::Color(40,40,40));
            window.draw(rect);
        }

        // draw character image
        if (slotInfo[i].exists && slotInfo[i].characterIndex >= 0 && slotInfo[i].characterIndex < 4)
        {
            sf::Sprite cs;
            cs.setTexture(charTextures[slotInfo[i].characterIndex]);
            float scale = CHAR_IMG_SIZE / static_cast<float>(charTextures[slotInfo[i].characterIndex].getSize().y);
            cs.setScale(scale, scale);
            cs.setPosition(x + 12.f, y + (SLOT_H - CHAR_IMG_SIZE) / 2.f);
            window.draw(cs);
        }

        // draw texts
        if (font)
        {
            sf::Text nameText;
            nameText.setFont(*font);
            nameText.setCharacterSize(22);
            nameText.setFillColor(sf::Color::White);
            if (slotInfo[i].exists && !slotInfo[i].playerName.empty())
                nameText.setString(slotInfo[i].playerName);
            else if (!slotInfo[i].exists)
                nameText.setString("EMPTY SLOT");
            else
                nameText.setString(" ");

            nameText.setPosition(x + CHAR_IMG_SIZE + 24.f, y + 8.f);
            window.draw(nameText);

            if (slotInfo[i].exists)
            {
                sf::Text infoText;
                infoText.setFont(*font);
                infoText.setCharacterSize(18);
                infoText.setFillColor(sf::Color(200,200,200));

                std::ostringstream oss;
                int mi = slotInfo[i].mapIndex;
                std::string mname = (mi>=0 && mi<4)? MAP_NAMES[mi] : "UNKNOWN";
                oss << "MAP: " << mname;
                infoText.setString(oss.str());
                infoText.setPosition(x + CHAR_IMG_SIZE + 24.f, y + 36.f);
                window.draw(infoText);

                oss.str(""); oss.clear();
                oss << "MODE: " << MODE_NAMES[slotInfo[i].difficultyMode];
                infoText.setString(oss.str());
                infoText.setPosition(x + CHAR_IMG_SIZE + 24.f, y + 58.f);
                window.draw(infoText);

                oss.str(""); oss.clear();
                oss << "LEVEL: " << slotInfo[i].level;
                infoText.setString(oss.str());
                infoText.setPosition(x + CHAR_IMG_SIZE + 260.f, y + 36.f);
                window.draw(infoText);

                oss.str(""); oss.clear();
                oss << "SCORE: " << slotInfo[i].score;
                infoText.setString(oss.str());
                infoText.setPosition(x + CHAR_IMG_SIZE + 260.f, y + 58.f);
                window.draw(infoText);

                sf::Text timeText;
                timeText.setFont(*font);
                timeText.setCharacterSize(16);
                timeText.setFillColor(sf::Color(170,170,170));
                timeText.setString(slotInfo[i].saveTime);
                timeText.setPosition(x + CHAR_IMG_SIZE + 24.f, y + 82.f);
                window.draw(timeText);
            }

            // draw selection highlight
            if (!onBack && i == selectedIndex)
            {
                sf::RectangleShape frame({ slotW, SLOT_H });
                frame.setPosition(x, y);
                frame.setFillColor(sf::Color::Transparent);
                frame.setOutlineColor(sf::Color(255, 220, 80));
                frame.setOutlineThickness(3.f);
                window.draw(frame);
            }
        }

        y += SLOT_H + SLOT_GAP;
    }

    backButton.draw(window);
}
