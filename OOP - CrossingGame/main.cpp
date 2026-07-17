#include <SFML/Graphics.hpp>

int main()
{
    // Tạo cửa sổ
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Crossing Game");
    window.setFramerateLimit(60);

    //======================
    // Load Map
    //======================
    sf::Texture mapTexture;
    if (!mapTexture.loadFromFile("D:/OOP_Project/OOP - CrossingGame/Map/City_map.png"))
        return -1;

    Sprite map(mapTexture);

    map.setScale(
        1920.f / mapSize.x,
        1080.f / mapSize.y
    );

    map.setPosition(0.f, 0.f);

    if (!chickenTexture.loadFromFile("D:/OOP_Project/OOP - CrossingGame/Character/Chicken_character.png"))
        return -1;

    sf::Sprite chicken(chickenTexture);

    // Sprite sheet có 4 cột × 5 hàng
    int frameWidth = chickenTexture.getSize().x / 4;
    int frameHeight = chickenTexture.getSize().y / 5;

    // Lấy frame đầu tiên của hàng DOWN
    chicken.setTextureRect(sf::IntRect(
        0,
        frameHeight,
        frameWidth,
        frameHeight
    ));

    // Phóng to nhân vật
    chicken.setScale(3.f, 3.f);

    // Đặt giữa màn hình
    chicken.setPosition(
        1920 / 2.f - frameWidth * 1.5f,
        1080 - 180.f
    );

    //======================
    // Game Loop
    //======================
    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();

        window.draw(map);
        window.draw(chicken);

        window.display();
    }

    return 0;
}