// TrafficLight.cpp
#include "TrafficLight.h"
#include <iostream>

TrafficLight::TrafficLight(float startX, float startY)
    : x(startX), y(startY),
    currentState(GREEN),
    lightTimer(0.f),
    redDuration(4.f),    // Đèn đỏ 4 giây
    greenDuration(5.f),  // Đèn xanh 5 giây
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

    // Sprite sheet đèn: 2 frame ngang (frame 0 = xanh, frame 1 = đỏ)
    frameWidth = texture.getSize().x / 2;
    frameHeight = texture.getSize().y;

    // Bắt đầu bằng đèn xanh (frame 0)
    sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
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
        // Chuyển sang frame đỏ (frame 1)
        sprite.setTextureRect(sf::IntRect(frameWidth, 0, frameWidth, frameHeight));
    }
    else if (currentState == RED && lightTimer >= redDuration) {
        currentState = GREEN;
        lightTimer = 0.f;
        // Chuyển sang frame xanh (frame 0)
        sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
    }
}

void TrafficLight::Draw(sf::RenderWindow& window)
{
    window.draw(sprite);
}