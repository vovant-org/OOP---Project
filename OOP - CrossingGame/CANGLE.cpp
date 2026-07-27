// CANGLE.cpp — Sky map
// Sprite: Skyangle_Obstacle.png
#include "CANGLE.h"
#include "ObstacleHelper.h"

CANGLE::CANGLE(float startX, float startY, float spd, bool moveRight)
    : CANIMAL(startX, startY, spd, moveRight)
{
    if (!texture.loadFromFile("Obstacles/Skyangle_Obstacle.png")) return;

    sprite.setTexture(texture);

    int totalFrames = 4;
    frameWidth = texture.getSize().x / totalFrames;
    frameHeight = texture.getSize().y;
    frameTime = 0.08f;  // Angle: bay nhẹ nhàng

    sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
    sprite.setScale(0.16f, 0.16f);

    if (!isMovingRight) {
        sprite.setScale(-0.16f, 0.16f);
        sprite.setOrigin((float)frameWidth, 0.f);
    }

    sprite.setPosition(x, y);
}

void CANGLE::Update(float deltaTime)
{
    OBSTACLE_UPDATE(4)
}