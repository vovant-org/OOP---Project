#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Crossing Game");
    window.setFramerateLimit(60);

    sf::Texture tex;
    if (!tex.loadFromFile("D:/OOP_Project/OOP - CrossingGame/Character/Kight_Character.png")) {
        return -1;
    }

    sf::Sprite sprite(tex);

    // Scale vừa phải để thấy toàn bộ sprite sheet
    sprite.setScale(3.f, 3.f);

    // Đặt vị trí giữa màn hình
    sprite.setPosition(105.f, 25.f);

    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e))
            if (e.type == sf::Event::Closed) window.close();

        window.clear(sf::Color(20, 30, 50));
        window.draw(sprite);
        window.display();
    }
    return 0;
}