// PauseMenu.cpp
#include "PauseMenu.h"
#include "AudioManager.h"

namespace
{
    void centerText(sf::Text& t, float cx, float y)
    {
        sf::FloatRect b = t.getLocalBounds();
        t.setOrigin(b.left + b.width / 2.f, b.top);
        t.setPosition(cx, y);
    }
}

//==================================================
// Constructor
//==================================================

PauseMenu::PauseMenu()
{
    buttons.resize(5);   // ===== CHANGED: 4 -> 5 (them nut "SAVE GAME") =====
    updateFocus();
}

//==================================================
// Audio
//==================================================

void PauseMenu::setAudioManager(AudioManager* manager)
{
    audio = manager;
}

//==================================================
// Overlay / Font
//==================================================

void PauseMenu::setOverlaySize(float canvasWIn, float canvasHIn)
{
    canvasW = canvasWIn;
    canvasH = canvasHIn;

    overlay.setSize({ canvasW, canvasH });
    overlay.setFillColor(sf::Color(0, 0, 0, 160));
    overlay.setPosition(0.f, 0.f);

    if (hasFont)
        centerText(titleText, canvasW / 2.f, canvasH * 0.18f);
}

void PauseMenu::setFont(const sf::Font& font)
{
    titleText.setFont(font);
    titleText.setString("PAUSED");
    titleText.setCharacterSize(52);
    titleText.setStyle(sf::Text::Bold);
    titleText.setFillColor(sf::Color(255, 220, 80));

    hasFont = true;

    centerText(titleText, canvasW / 2.f, canvasH * 0.18f);
}

//==================================================
// Button
//==================================================

Button& PauseMenu::getButton(PauseMenuButton button)
{
    return buttons.at(static_cast<std::size_t>(button));
}

const Button& PauseMenu::getButton(PauseMenuButton button) const
{
    return buttons.at(static_cast<std::size_t>(button));
}

std::size_t PauseMenu::getButtonCount() const
{
    return buttons.size();
}

void PauseMenu::updateFocus()
{
    for (auto& b : buttons)
        b.setFocused(false);

    if (!buttons.empty())
        buttons[selectedIndex].setFocused(true);
}

void PauseMenu::moveSelectionUp()
{
    if (buttons.empty())
        return;

    selectedIndex--;
    if (selectedIndex < 0)
        selectedIndex = static_cast<int>(buttons.size()) - 1;

    updateFocus();
}

void PauseMenu::moveSelectionDown()
{
    if (buttons.empty())
        return;

    selectedIndex++;
    if (selectedIndex >= static_cast<int>(buttons.size()))
        selectedIndex = 0;

    updateFocus();
}

//==================================================
// Result
//==================================================

PauseMenuResult PauseMenu::getResult() const
{
    return result;
}

void PauseMenu::clearResult()
{
    result = PauseMenuResult::None;
}

//==================================================
// Menu
//==================================================

void PauseMenu::processEvent(const sf::Event& event,
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
        {
            buttons[selectedIndex].press();

            if (audio)
                audio->playSound("select");

            result = static_cast<PauseMenuResult>(selectedIndex + 1);
            break;
        }

        // ESC khi đang Pause -> Resume ngay (main.cpp đã dùng ESC để
        // MỞ pause từ Playing, nên ở đây dùng ESC để ĐÓNG lại cho đối xứng)
        case sf::Keyboard::Escape:
            result = PauseMenuResult::Resume;
            break;

        default:
            break;
        }
    }

    //-----------------------------
    // Mouse
    //-----------------------------

    for (auto& b : buttons)
        b.processEvent(event, window);

    if (event.type == sf::Event::MouseButtonReleased &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mousePos = window.mapPixelToCoords(
            { event.mouseButton.x, event.mouseButton.y });

        for (std::size_t i = 0; i < buttons.size(); i++)
        {
            if (buttons[i].contains(mousePos))
            {
                buttons[i].press();

                if (audio)
                    audio->playSound("select");

                selectedIndex = static_cast<int>(i);
                updateFocus();

                result = static_cast<PauseMenuResult>(i + 1);
                break;
            }
        }
    }
}

void PauseMenu::update(float dt)
{
    (void)dt;

    for (auto& b : buttons)
        b.update();
}

void PauseMenu::draw(sf::RenderWindow& window) const
{
    window.draw(overlay);

    if (hasFont)
        window.draw(titleText);

    for (const auto& b : buttons)
        b.draw(window);
}