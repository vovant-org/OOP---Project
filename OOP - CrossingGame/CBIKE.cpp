// CBIKE.cpp
#include "CBIKE.h"

CBIKE::CBIKE(float startX, float startY, float spd, bool moveRight)
    : CVEHICLE(startX, startY, spd, moveRight)
{
    frameWidth = 112;
    frameHeight = 112;
    frameTime = 0.1f;      // Xe đạp đảo chân khá nhanh
    elapsedTime = 0.f;
    currentFrame = 0;

    texture.loadFromFile("assets/Bke_Obstacle.png"); // sheet 448x112, 4 frame 112x112
    sprite.setTexture(texture);
    sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
    sprite.setOrigin(frameWidth / 2.f, frameHeight / 2.f); // tâm sprite để lật hình không lệch vị trí
    sprite.setPosition(x, y);
}

void CBIKE::Update(float deltaTime)
{
    // Di chuyển: moveRight = true -> từ trái sang phải, ngược lại -> từ phải sang trái
    if (isMovingRight)
        x += speed * deltaTime;
    else
        x -= speed * deltaTime;

    sprite.setPosition(x, y);

    // Lật hình theo hướng di chuyển (giả sử sprite gốc quay mặt phải)
    sprite.setScale(isMovingRight ? 1.f : -1.f, 1.f);

    // Cập nhật animation (chạy bánh xe)
    elapsedTime += deltaTime;
    if (elapsedTime >= frameTime) {
        currentFrame = (currentFrame + 1) % 4; // dải 4 frame
        sprite.setTextureRect(sf::IntRect(currentFrame * frameWidth, 0, frameWidth, frameHeight));
        elapsedTime = 0.f;
    }
}