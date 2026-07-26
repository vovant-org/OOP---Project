// CHELLMOTOR.cpp — Hell map
// Sprite: HellMotor_Obstacle.png
#include "CHELLMOTOR.h"
#include "ObstacleHelper.h"

CHELLMOTOR::CHELLMOTOR(float startX, float startY, float spd, bool moveRight)
    : CVEHICLE(startX, startY, spd, moveRight)
{
    if (!texture.loadFromFile("Obstacles/HellMotor_Obstacle.png")) return;

    sprite.setTexture(texture);

    int totalFrames = 4;
    frameWidth = texture.getSize().x / totalFrames;
    frameHeight = texture.getSize().y;
    frameTime = 0.06f;  // Hell motor: nhanh và hung hãn nhất

    sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
    sprite.setScale(0.17f, 0.17f);

    if (!isMovingRight) {
        sprite.setScale(-0.17f, 0.17f);
        sprite.setOrigin((float)frameWidth, 0.f);
    }

    sprite.setPosition(x, y);
}

void CHELLMOTOR::Update(float deltaTime)
{
    OBSTACLE_UPDATE(4)
}