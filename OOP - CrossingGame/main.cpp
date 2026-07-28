#include <SFML/Graphics.hpp>
#include <iostream>

#include "MainMenu.h"

int main()
{
    sf::RenderWindow window(
        sf::VideoMode(1280, 720),
        "Main Menu Test"
    );

    window.setFramerateLimit(60);

    //--------------------------------------------------
    // Load Background
    //--------------------------------------------------

    sf::Texture backgroundTexture;

    if (!backgroundTexture.loadFromFile(
        "Resource Files/ui/Background/CrossingGame_background.png"))
    {
        std::cout << "Cannot load background!\n";
        return -1;
    }

    //--------------------------------------------------
    // Load Logo
    //--------------------------------------------------

    sf::Texture logoTexture;

    if (!logoTexture.loadFromFile(
        "Resource Files/ui/Logo/CrossingGame_Logo.png"))
    {
        std::cout << "Cannot load logo!\n";
        return -1;
    }

    //--------------------------------------------------
    // Load Button Texture
    //--------------------------------------------------

    sf::Texture buttonTexture;

    if (!buttonTexture.loadFromFile(
        "Resource Files/ui/Button/button_normal.png"))
    {
        std::cout << "Cannot load button texture!\n";
        return -1;
    }

    //--------------------------------------------------
    // Load Font
    //--------------------------------------------------

    sf::Font font;

    if (!font.loadFromFile(
        "Resource Files/Font/PixelOperator.ttf"))
    {
        std::cout << "Cannot load font!\n";
        return -1;
    }

    //--------------------------------------------------
    // Create Main Menu
    //--------------------------------------------------

    MainMenu menu;

    //-------------------------
    // Background
    //-------------------------

    menu.setBackgroundTexture(backgroundTexture);

    float bgScaleX =
        static_cast<float>(window.getSize().x) /
        backgroundTexture.getSize().x;

    float bgScaleY =
        static_cast<float>(window.getSize().y) /
        backgroundTexture.getSize().y;

    menu.setBackgroundScale(bgScaleX, bgScaleY);

    //-------------------------
    // Logo
    //-------------------------

    menu.setLogoTexture(logoTexture);

    menu.setLogoScale(0.40f, 0.40f);

    menu.setLogoPosition(330.f, -50.f);

    //-------------------------
    // Button Common Size
    //-------------------------

    float buttonWidth = 240.f;
    float buttonHeight = 100.f;

    float scaleX =
        buttonWidth / buttonTexture.getSize().x;

    float scaleY =
        buttonHeight / buttonTexture.getSize().y;

    float buttonX = 515.f;
    float startY = 250.f;   // vị trí button đầu tiên, đẩy xuống 1 chút để không đụng logo
    float spacing = 55.f;

    //--------------------------------------------------
    // PLAY
    //--------------------------------------------------

    Button& play =
        menu.getButton(MainMenuButton::Play);

    play.setTexture(buttonTexture);
    play.setFont(font);
    play.setText("PLAY");
    play.setCharacterSize(28);
    play.setScale(scaleX, scaleY);
    play.setPosition(buttonX, startY);
    play.setFocused(false);

    //--------------------------------------------------
    // CONTINUE
    //--------------------------------------------------

    Button& cont =
        menu.getButton(MainMenuButton::Continue);

    cont.setTexture(buttonTexture);
    cont.setFont(font);
    cont.setText("CONTINUE");
    cont.setCharacterSize(28);
    cont.setScale(scaleX, scaleY);
    cont.setPosition(buttonX, startY + spacing);
    cont.setFocused(false);

    //--------------------------------------------------
    // SETTINGS
    //--------------------------------------------------

    Button& settings =
        menu.getButton(MainMenuButton::Settings);

    settings.setTexture(buttonTexture);
    settings.setFont(font);
    settings.setText("SETTINGS");
    settings.setCharacterSize(28);
    settings.setScale(scaleX, scaleY);
    settings.setPosition(buttonX, startY + spacing * 2);
    settings.setFocused(false);

    //--------------------------------------------------
    // LEADERBOARD
    //--------------------------------------------------

    Button& leaderboard =
        menu.getButton(MainMenuButton::Leaderboard);

    leaderboard.setTexture(buttonTexture);
    leaderboard.setFont(font);
    leaderboard.setText("LEADERBOARD");
    leaderboard.setCharacterSize(28);
    leaderboard.setScale(scaleX, scaleY);
    leaderboard.setPosition(buttonX, startY + spacing * 3);
    leaderboard.setFocused(false);

    //--------------------------------------------------
    // ABOUT
    //--------------------------------------------------

    Button& about =
        menu.getButton(MainMenuButton::About);

    about.setTexture(buttonTexture);
    about.setFont(font);
    about.setText("ABOUT");
    about.setCharacterSize(28);
    about.setScale(scaleX, scaleY);
    about.setPosition(buttonX, startY + spacing * 4);
    about.setFocused(false);

    //--------------------------------------------------
    // EXIT
    //--------------------------------------------------

    Button& exit =
        menu.getButton(MainMenuButton::Exit);

    exit.setTexture(buttonTexture);
    exit.setFont(font);
    exit.setText("EXIT");
    exit.setCharacterSize(28);
    exit.setScale(scaleX, scaleY);
    exit.setPosition(buttonX, startY + spacing * 5);
    exit.setFocused(false);

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
            menu.clearResult();
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