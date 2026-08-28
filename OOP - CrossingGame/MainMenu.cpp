#include "MainMenu.h"
#include "AudioManager.h"

// ADDED Audio
void MainMenu::setAudioManager(AudioManager* manager)
{
    audio = manager;
}

//==================================================
// Constructor
//==================================================

MainMenu::MainMenu()
{
    buttons.resize(7);   // ===== CHANGED: +1 nut MOD =====

    updateFocus();
}

//==================================================
// Texture
//==================================================

void MainMenu::setBackgroundTexture(const sf::Texture& texture)
{
    background.setTexture(texture);
}

void MainMenu::setBackgroundScale(float scaleX, float scaleY)
{
    background.setScale(scaleX, scaleY);
}

void MainMenu::setLogoTexture(const sf::Texture& texture)
{
    logo.setTexture(texture);
}

//==================================================
// Logo
//==================================================

void MainMenu::setLogoPosition(float x, float y)
{
    logo.setPosition(x, y);
}

void MainMenu::setLogoScale(float scaleX, float scaleY)
{
    logo.setScale(scaleX, scaleY);
}

//==================================================
// Button
//==================================================
Button& MainMenu::getButton(MainMenuButton button)
{
    return buttons.at(static_cast<std::size_t>(button));
}

const Button& MainMenu::getButton(MainMenuButton button) const
{
    return buttons.at(static_cast<std::size_t>(button));
}

std::size_t MainMenu::getButtonCount() const
{
    return buttons.size();
}

void MainMenu::updateFocus()
{
    for (size_t i = 0; i < buttons.size(); i++)
    {
        buttons[i].setFocused(false);
    }

    if (!buttons.empty())
    {
        buttons[selectedIndex].setFocused(true);
    }
}

//==================================================
// Keyboard Navigation
//==================================================

void MainMenu::moveSelectionUp()
{
    if (buttons.empty())
        return;

    selectedIndex--;

    if (selectedIndex < 0)
        selectedIndex = static_cast<int>(buttons.size()) - 1;

    updateFocus();
}

void MainMenu::moveSelectionDown()
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

MainMenuResult MainMenu::getResult() const
{
    return result;
}

void MainMenu::clearResult()
{
    result = MainMenuResult::None;
}

//==================================================
// Menu
//==================================================

void MainMenu::processEvent(const sf::Event& event,
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

            if (audio) audio->playSound("select");   // ===== ADDED =====

            result = static_cast<MainMenuResult>(selectedIndex + 1);

            break;
        }

        default:
            break;
        }
    }

    //-----------------------------
    // Mouse
    //-----------------------------

    for (auto& button : buttons)
        button.processEvent(event, window);

    if (event.type == sf::Event::MouseButtonReleased &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mousePos =
            window.mapPixelToCoords(
                { event.mouseButton.x, event.mouseButton.y });

        for (size_t i = 0; i < buttons.size(); i++)
        {
            if (buttons[i].contains(mousePos))
            {
                buttons[i].press();

                if (audio) audio->playSound("select");   // ===== ADDED =====

                selectedIndex = static_cast<int>(i);

                updateFocus();

                result = static_cast<MainMenuResult>(i + 1);

                break;
            }
        }
    }
}

void MainMenu::update(float)
{
    for (auto& button : buttons)
        button.update();
}

void MainMenu::draw(sf::RenderWindow& window) const
{
    background.draw(window);

    window.draw(logo);

    for (const auto& button : buttons)
        button.draw(window);
}