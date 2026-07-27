// COLDBIRD.cpp — Ancient map
// Sprite: Ancientbird_Obstacle.png
#include "COLDBIRD.h"
#include "ObstacleHelper.h"

COLDBIRD::COLDBIRD(float startX, float startY, float spd, bool moveRight)
    : CANIMAL(startX, startY, spd, moveRight)
{
    if (!texture.loadFromFile("Obstacles/Ancientbird_Obstacle.png")) return;

    sprite.setTexture(texture);

    int totalFrames = 4;
    frameWidth = texture.getSize().x / totalFrames;
    frameHeight = texture.getSize().y;
    frameTime = 0.07f;  // Chim cổ đại: vẫy cánh nhanh

    sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
    sprite.setScale(0.15f, 0.15f);

    if (!isMovingRight) {
        sprite.setScale(-0.15f, 0.15f);
        sprite.setOrigin((float)frameWidth, 0.f);
    }

    sprite.setPosition(x, y);
}

void COLDBIRD::Update(float deltaTime)
{
    OBSTACLE_UPDATE(4)
}