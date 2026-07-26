// CBIRD.cpp — Sky map
// Sprite: Skybird_Obstacle.png
#include "CBIRD.h"
#include "ObstacleHelper.h"

CBIRD::CBIRD(float startX, float startY, float spd, bool moveRight)
    : CANIMAL(startX, startY, spd, moveRight)
{
    if (!texture.loadFromFile("Obstacles/Skybird_Obstacle.png")) return;

    sprite.setTexture(texture);

    int totalFrames = 4;
    frameWidth = texture.getSize().x / totalFrames;
    frameHeight = texture.getSize().y;
    frameTime = 0.06f;  // Chim sky: vẫy cánh rất nhanh

    sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
    sprite.setScale(0.14f, 0.14f);

    if (!isMovingRight) {
        sprite.setScale(-0.14f, 0.14f);
        sprite.setOrigin((float)frameWidth, 0.f);
    }

    sprite.setPosition(x, y);
}

void CBIRD::Update(float deltaTime)
{
    OBSTACLE_UPDATE(4)
}