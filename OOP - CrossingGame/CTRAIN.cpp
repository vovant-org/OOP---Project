// CTRAIN.cpp — Hell map
// Sprite: HellTrain_Obstacle.png
#include "CTRAIN.h"
#include "ObstacleHelper.h"

CTRAIN::CTRAIN(float startX, float startY, float spd, bool moveRight)
    : CVEHICLE(startX, startY, spd, moveRight)
{
    if (!texture.loadFromFile("Obstacles/HellTrain_Obstacle.png")) return;

    sprite.setTexture(texture);

    // Tàu hỏa thường chỉ có 1 frame (hoặc ít frame hơn)
    int totalFrames = 4;
    frameWidth = texture.getSize().x / totalFrames;
    frameHeight = texture.getSize().y;
    frameTime = 0.15f;  // Tàu hỏa: animation bánh xe chậm

    sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));

    // Tàu to hơn tất cả — chiếm gần cả lane
    sprite.setScale(0.28f, 0.28f);

    if (!isMovingRight) {
        sprite.setScale(-0.28f, 0.28f);
        sprite.setOrigin((float)frameWidth, 0.f);
    }

    sprite.setPosition(x, y);
}

void CTRAIN::Update(float deltaTime)
{
    OBSTACLE_UPDATE(4)
}