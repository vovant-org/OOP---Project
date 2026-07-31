// TrafficLight.cpp
#include "TrafficLight.h"
#include <iostream>

TrafficLight::TrafficLight(float startX, float startY, float redDur, float greenDur)
    : x(startX), y(startY),
    currentState(GREEN),
    lightTimer(0.f),
    redDuration(redDur),      // Thời lượng đèn đỏ - truyền riêng cho từng đèn
    greenDuration(greenDur),  // Thời lượng đèn xanh - truyền riêng cho từng đèn
    frameWidth(0), frameHeight(0)
{
}

bool TrafficLight::loadTexture(const std::string& path)
{
    if (!texture.loadFromFile(path)) {
        std::cout << "[TrafficLight] Cannot load: " << path << "\n";
        return false;
    }
    sprite.setTexture(texture);

    // Sprite sheet đèn: 4 frame ngang
    // frame 0 = đỏ tĩnh, 1 = đỏ nhấp nháy, 2 = xanh tĩnh, 3 = xanh nhấp nháy
    // (chỉ dùng frame tĩnh 0 và 2, bỏ qua 2 frame nhấp nháy cho đơn giản)
    frameWidth = texture.getSize().x / 4;
    frameHeight = texture.getSize().y;

    // Bắt đầu bằng đèn xanh (frame 2)
    sprite.setTextureRect(sf::IntRect(frameWidth * 2, 0, frameWidth, frameHeight));
    sprite.setScale(0.12f, 0.12f);
    sprite.setPosition(x, y);
    return true;
}

void TrafficLight::Update(float deltaTime)
{
    lightTimer += deltaTime;

    if (currentState == GREEN && lightTimer >= greenDuration) {
        currentState = RED;
        lightTimer = 0.f;
        // Chuyển sang frame đỏ tĩnh (frame 0)
        sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
    }
    else if (currentState == RED && lightTimer >= redDuration) {
        currentState = GREEN;
        lightTimer = 0.f;
        // Chuyển sang frame xanh tĩnh (frame 2)
        sprite.setTextureRect(sf::IntRect(frameWidth * 2, 0, frameWidth, frameHeight));
    }
}

void TrafficLight::Draw(sf::RenderWindow& window)
{
    window.draw(sprite);
}