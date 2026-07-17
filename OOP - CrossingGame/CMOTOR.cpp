// CMOTOR.cpp
#include "CMOTOR.h"

CMOTOR::CMOTOR(float startX, float startY, float spd, bool moveRight)
    : CVEHICLE(startX, startY, spd, moveRight)
{
    frameWidth = 104;
    frameHeight = 112;
    frameTime = 0.08f;     // Xe máy chạy nhanh, đổi frame nhanh hơn
    elapsedTime = 0.f;
    currentFrame = 0;

    texture.loadFromFile("assets/MOTOR.png"); // sheet 416x112, 4 frame 104x112
    sprite.setTexture(texture);
    sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
    sprite.setOrigin(frameWidth / 2.f, frameHeight / 2.f);
    sprite.setPosition(x, y);
}

void CMOTOR::Update(float deltaTime)
{
    // Di chuyển: moveRight = true -> từ trái sang phải, ngược lại -> từ phải sang trái
    if (isMovingRight)
        x += speed * deltaTime;
    else
        x -= speed * deltaTime;

    sprite.setPosition(x, y);

    // Lật hình theo hướng di chuyển
    sprite.setScale(isMovingRight ? 1.f : -1.f, 1.f);

    // Cập nhật animation
    elapsedTime += deltaTime;
    if (elapsedTime >= frameTime) {
        currentFrame = (currentFrame + 1) % 4;
        sprite.setTextureRect(sf::IntRect(currentFrame * frameWidth, 0, frameWidth, frameHeight));
        elapsedTime = 0.f;
    }
}