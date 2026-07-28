#include <SFML/Graphics.hpp>
#include <iostream>

#include "MainMenu.h"
#include "ResourceManager.h"

int main()
{
    //--------------------------------------------------
    // Window
    //--------------------------------------------------

    sf::RenderWindow window(
        sf::VideoMode(1280, 720),
        "Main Menu Test");

    window.setFramerateLimit(60);

    //--------------------------------------------------
    // Resource Manager
    //--------------------------------------------------

    ResourceManager resources;

    if (!resources.loadTexture(
        "ui/background",
        "ui/Background/CrossingGame_background.png"))
    {
        std::cout << "Cannot load background!\n";
        return -1;
    }

    if (!resources.loadTexture(
        "ui/logo",
        "ui/Logo/CrossingGame_Logo.png"))
    {
        std::cout << "Cannot load logo!\n";
        return -1;
    }

    if (!resources.loadTexture(
        "ui/button",
        "ui/Button/button_normal.png"))
    {
        std::cout << "Cannot load button!\n";
        return -1;
    }

    if (!resources.loadFont(
        "font/pixel",
        "Font/PixelOperator.ttf"))
    {
        std::cout << "Cannot load font!\n";
        return -1;
    }

    //--------------------------------------------------
    // Main Menu
    //--------------------------------------------------

    MainMenu menu;

    //--------------------------------------------------
    // Background
    //--------------------------------------------------

    menu.setBackgroundTexture(
        resources.getTexture("ui/background"));

    const sf::Texture& background =
        resources.getTexture("ui/background");

    float bgScaleX =
        static_cast<float>(window.getSize().x) /
        background.getSize().x;

    float bgScaleY =
        static_cast<float>(window.getSize().y) /
        background.getSize().y;

    menu.setBackgroundScale(bgScaleX, bgScaleY);

    //--------------------------------------------------
    // Logo
    //--------------------------------------------------

    menu.setLogoTexture(
        resources.getTexture("ui/logo"));

    menu.setLogoScale(0.40f, 0.40f);

    menu.setLogoPosition(330.f, -50.f);

    //--------------------------------------------------
    // Button Common Settings
    //--------------------------------------------------

    const sf::Texture& buttonTexture =
        resources.getTexture("ui/button");

    const sf::Font& font =
        resources.getFont("font/pixel");

    constexpr float buttonWidth = 240.f;
    constexpr float buttonHeight = 100.f;

    float scaleX =
        buttonWidth /
        buttonTexture.getSize().x;

    float scaleY =
        buttonHeight /
        buttonTexture.getSize().y;

    constexpr float buttonX = 515.f;
    constexpr float startY = 250.f;
    constexpr float spacing = 55.f;

    //--------------------------------------------------
    // Button Data
    //--------------------------------------------------

    const std::string buttonNames[] =
    {
        "PLAY",
        "CONTINUE",
        "SETTINGS",
        "LEADERBOARD",
        "ABOUT",
        "EXIT"
    };

    //--------------------------------------------------
    // Create Buttons
    //--------------------------------------------------

    for (std::size_t i = 0; i < menu.getButtonCount(); i++)
    {
        Button& button =
            menu.getButton(static_cast<MainMenuButton>(i));

        button.setTexture(buttonTexture);

        button.setFont(font);

        button.setText(buttonNames[i]);

        button.setCharacterSize(28);

        button.setScale(scaleX, scaleY);

        button.setPosition(
            buttonX,
            startY + spacing * static_cast<float>(i));

        button.setFocused(false);
    }

    //--------------------------------------------------
    // Game Loop
    //--------------------------------------------------

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            menu.processEvent(event, window);
        }

        menu.update();

        switch (menu.getResult())
        {
        case MainMenuResult::Play:
            std::cout << "PLAY\n";
            menu.clearResult();
            break;

        case MainMenuResult::Continue:
            std::cout << "CONTINUE\n";
            menu.clearResult();
            break;

        case MainMenuResult::Settings:
            std::cout << "SETTINGS\n";
            menu.clearResult();
            break;

        case MainMenuResult::Leaderboard:
            std::cout << "LEADERBOARD\n";
            menu.clearResult();
            break;

        case MainMenuResult::About:
            std::cout << "ABOUT\n";
            menu.clearResult();
            break;

        case MainMenuResult::Exit:
            std::cout << "EXIT\n";
            window.close();
            break;

        default:
            break;
        }

        window.clear();

        menu.draw(window);

        window.display();
    }

    return 0;
}