// GameOverMenu.cpp
#include "GameOverMenu.h"
#include "AudioManager.h"
#include <sstream>

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

GameOverMenu::GameOverMenu()
{
    buttons.resize(2);
    updateFocus();
}

//==================================================
// Audio
//==================================================

void GameOverMenu::setAudioManager(AudioManager* manager)
{
    audio = manager;
}

//==================================================
// Overlay / Font / Stats
//==================================================

void GameOverMenu::setOverlaySize(float canvasWIn, float canvasHIn)
{
    canvasW = canvasWIn;
    canvasH = canvasHIn;

    overlay.setSize({ canvasW, canvasH });
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    overlay.setPosition(0.f, 0.f);

    if (hasFont)
    {
        centerText(titleText, canvasW / 2.f, canvasH * 0.20f);
        centerText(scoreText, canvasW / 2.f, canvasH * 0.32f);
    }
}

void GameOverMenu::setFont(const sf::Font& font)
{
    titleText.setFont(font);
    titleText.setString("GAME OVER");
    titleText.setCharacterSize(56);
    titleText.setStyle(sf::Text::Bold);
    titleText.setFillColor(sf::Color(220, 60, 60));

    scoreText.setFont(font);
    scoreText.setCharacterSize(26);
    scoreText.setFillColor(sf::Color::White);

    hasFont = true;

    centerText(titleText, canvasW / 2.f, canvasH * 0.20f);
    centerText(scoreText, canvasW / 2.f, canvasH * 0.32f);
}

void GameOverMenu::setStats(int score, int level)
{
    std::ostringstream oss;
    oss << "Score: " << score << "   Level: " << level;
    scoreText.setString(oss.str());

    if (hasFont)
        centerText(scoreText, canvasW / 2.f, canvasH * 0.32f);
}

//==================================================
// Button
//==================================================

Button& GameOverMenu::getButton(GameOverMenuButton button)
{
    return buttons.at(static_cast<std::size_t>(button));
}

const Button& GameOverMenu::getButton(GameOverMenuButton button) const
{
    return buttons.at(static_cast<std::size_t>(button));
}

std::size_t GameOverMenu::getButtonCount() const
{
    return buttons.size();
}

void GameOverMenu::updateFocus()
{
    for (auto& b : buttons)
        b.setFocused(false);

    if (!buttons.empty())
        buttons[selectedIndex].setFocused(true);
}

void GameOverMenu::moveSelectionUp()
{
    if (buttons.empty())
        return;

    selectedIndex--;
    if (selectedIndex < 0)
        selectedIndex = static_cast<int>(buttons.size()) - 1;

    updateFocus();
}

void GameOverMenu::moveSelectionDown()
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

GameOverMenuResult GameOverMenu::getResult() const
{
    return result;
}

void GameOverMenu::clearResult()
{
    result = GameOverMenuResult::None;
}

//==================================================
// Menu
//==================================================

void GameOverMenu::processEvent(const sf::Event& event,
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

            result = static_cast<GameOverMenuResult>(selectedIndex + 1);
            break;
        }

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

                result = static_cast<GameOverMenuResult>(i + 1);
                break;
            }
        }
    }
}

void GameOverMenu::update(float dt)
{
    (void)dt;

    for (auto& b : buttons)
        b.update();
}

void GameOverMenu::draw(sf::RenderWindow& window) const
{
    window.draw(overlay);

    if (hasFont)
    {
        window.draw(titleText);
        window.draw(scoreText);
    }

    for (const auto& b : buttons)
        b.draw(window);
}