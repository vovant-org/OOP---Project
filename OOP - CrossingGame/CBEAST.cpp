// CBeast.cpp — Ancient map
// Sprite: Ancientbeast_Obstacle.png
#include "CBeast.h"
#include "ObstacleHelper.h"

CBEAST::CBEAST(float startX, float startY, float spd, bool moveRight)
    : CANIMAL(startX, startY, spd, moveRight)
{
    if (!texture.loadFromFile("Obstacles/Ancientbeast_Obstacle.png")) return;

    sprite.setTexture(texture);

    int totalFrames = 4;
    frameWidth = texture.getSize().x / totalFrames;
    frameHeight = texture.getSize().y;
    frameTime = 0.10f;  // Beast: chạy vừa, hung hãn

    sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
    sprite.setScale(0.22f, 0.22f);  // To nhất trong Ancient map

    if (!isMovingRight) {
        sprite.setScale(-0.22f, 0.22f);
        sprite.setOrigin((float)frameWidth, 0.f);
    }

    sprite.setPosition(x, y);
}

void CBEAST::Update(float deltaTime)
{
    OBSTACLE_UPDATE(4)
}