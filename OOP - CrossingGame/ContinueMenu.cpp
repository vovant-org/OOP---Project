// ContinueMenu.cpp
#include "ContinueMenu.h"
#include "AudioManager.h"
#include "CGAME.h"

#include <sstream>

namespace
{
    const char* MAP_NAMES[4] = { "CITY", "ANCIENT", "HELL", "SKY" };
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
    centerText(titleText, W / 2.f, H * 0.08f);

    hintText.setFont(f);
    hintText.setString("Chua co tien trinh nao duoc luu");
    hintText.setCharacterSize(22);
    hintText.setFillColor(sf::Color(200, 200, 200));
    centerText(hintText, W / 2.f, H * 0.45f);

    backButton.setFont(f);
    backButton.setText("BACK");
    backButton.setCharacterSize(26);
}

void ContinueMenu::setButtonTexture(const sf::Texture& tex, float scaleX, float scaleY)
{
    buttonTexture = &tex;
    btnScaleX = scaleX;
    btnScaleY = scaleY;

    backButton.setTexture(tex);
    backButton.setScale(scaleX, scaleY);
}

//==================================================
// Refresh - quet lai 4 file save
//==================================================

void ContinueMenu::refresh()
{
    saveButtons.clear();
    saveButtonMapIndex.clear();

    const float bx = W / 2.f - 125.f;
    const float startY = H * 0.28f;
    const float spacing = 70.f;
    float y = startY;

    for (int m = 0; m < 4; ++m)
    {
        int outMap = 0, outChar = 0, outLevel = 1, outScore = 0, outMode = 1;

        if (!CGAME::PeekSaveInfo(CGAME::GetSavePathForMap(m),
            outMap, outChar, outLevel, outScore, outMode))
            continue;

        Button btn;

        if (buttonTexture)
        {
            btn.setTexture(*buttonTexture);
            btn.setScale(btnScaleX, btnScaleY);
        }

        if (font)
            btn.setFont(*font);

        std::ostringstream oss;
        oss << MAP_NAMES[m] << "  Lv" << outLevel << "  " << outScore << "pts";
        btn.setText(oss.str());
        btn.setCharacterSize(22);
        btn.setPosition(bx, y);
        btn.setFocused(false);

        saveButtons.push_back(btn);
        saveButtonMapIndex.push_back(m);

        y += spacing;
    }

    backButton.setPosition(bx, y + 20.f);

    selectedIndex = 0;
    result = ContinueMenuResult::None;
    selectedMapIndex = -1;

    updateFocus();
}

//==================================================
// Focus / Navigation
//==================================================

void ContinueMenu::updateFocus()
{
    for (auto& b : saveButtons)
        b.setFocused(false);
    backButton.setFocused(false);

    if (selectedIndex < static_cast<int>(saveButtons.size()))
        saveButtons[selectedIndex].setFocused(true);
    else
        backButton.setFocused(true);
}

void ContinueMenu::moveSelectionUp()
{
    int total = static_cast<int>(saveButtons.size()) + 1; // +1 cho Back
    if (total <= 0)
        return;

    selectedIndex--;
    if (selectedIndex < 0)
        selectedIndex = total - 1;

    updateFocus();
}

void ContinueMenu::moveSelectionDown()
{
    int total = static_cast<int>(saveButtons.size()) + 1;
    if (total <= 0)
        return;

    selectedIndex++;
    if (selectedIndex >= total)
        selectedIndex = 0;

    updateFocus();
}

void ContinueMenu::activateSelected()
{
    if (selectedIndex < static_cast<int>(saveButtons.size()))
    {
        saveButtons[selectedIndex].press();

        if (audio)
            audio->playSound("select");

        selectedMapIndex = saveButtonMapIndex[selectedIndex];
        result = ContinueMenuResult::Selected;
    }
    else
    {
        backButton.press();

        if (audio)
            audio->playSound("select");

        result = ContinueMenuResult::Back;
    }
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
            moveSelectionUp();
            break;

        case sf::Keyboard::Down:
            moveSelectionDown();
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
    // Mouse
    //-----------------------------

    for (auto& b : saveButtons)
        b.processEvent(event, window);
    backButton.processEvent(event, window);

    if (event.type == sf::Event::MouseButtonReleased &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mp = window.mapPixelToCoords(
            { event.mouseButton.x, event.mouseButton.y });

        for (std::size_t i = 0; i < saveButtons.size(); i++)
        {
            if (saveButtons[i].contains(mp))
            {
                selectedIndex = static_cast<int>(i);
                activateSelected();
                return;
            }
        }

        if (backButton.contains(mp))
        {
            selectedIndex = static_cast<int>(saveButtons.size());
            activateSelected();
        }
    }
}

void ContinueMenu::update(float dt)
{
    (void)dt;

    for (auto& b : saveButtons)
        b.update();
    backButton.update();
}

void ContinueMenu::draw(sf::RenderWindow& window) const
{
    background.draw(window);

    window.draw(titleText);

    if (saveButtons.empty())
        window.draw(hintText);

    for (const auto& b : saveButtons)
        b.draw(window);

    backButton.draw(window);
}