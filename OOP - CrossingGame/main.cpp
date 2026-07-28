#include <SFML/Graphics.hpp>
#include <iostream>

#include "Button.h"

int main()
{
    sf::RenderWindow window(
        sf::VideoMode(1280, 720),
        "Button Test");

    window.setFramerateLimit(60);

    //--------------------------------------------------
    // Load Texture
    //--------------------------------------------------

    sf::Texture buttonTexture;

    if (!buttonTexture.loadFromFile(
        "D:/OOP_Project/OOP - CrossingGame/ui/Button/button_normal.png"))
    {
        std::cout << "Cannot load button texture!\n";
        return -1;
    }

    //--------------------------------------------------
    // Load Font
    //--------------------------------------------------

    sf::Font font;

    if (!font.loadFromFile(
        "D:/OOP_Project/OOP - CrossingGame/Font/PixelOperator.ttf"))
    {
        std::cout << "Cannot load font!\n";
        return -1;
    }

    //--------------------------------------------------
    // Create Button
    //--------------------------------------------------

    Button playButton;

    playButton.setFont(font);

    playButton.setText("PLAY");

    playButton.setCharacterSize(24);

    // Kích thước button mong muốn, ví dụ 240 x 70 px
    float targetWidth = 240.f;
    float targetHeight = 70.f;

    playButton.setTexture(buttonTexture);
    playButton.setScale(
        targetWidth / buttonTexture.getSize().x,   // 240 / 1536
        targetHeight / buttonTexture.getSize().y    // 70 / 1024
    );

    playButton.setPosition(440.f, 250.f);

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

            playButton.processEvent(event, window);
        }

        playButton.update();

        window.clear(sf::Color(30, 30, 30));

        playButton.draw(window);

        window.display();
    }

    return 0;
}