// CCROCODILE.cpp — Ancient map
// Sprite: Ancientcrocodile_Obstacle.png
#include "CCROCODILE.h"
#include "ObstacleHelper.h"

CCROCODILE::CCROCODILE(float startX, float startY, float spd, bool moveRight)
    : CANIMAL(startX, startY, spd, moveRight)
{
    if (!texture.loadFromFile("Obstacles/Ancientcrocodile_Obstacle.png")) return;

    sprite.setTexture(texture);

    int totalFrames = 4;
    frameWidth = texture.getSize().x / totalFrames;
    frameHeight = texture.getSize().y;
    frameTime = 0.15f;  // Cá sấu: di chuyển chậm, nguy hiểm

    sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
    sprite.setScale(0.18f, 0.18f);

    if (!isMovingRight) {
        sprite.setScale(-0.18f, 0.18f);
        sprite.setOrigin((float)frameWidth, 0.f);
    }

    sprite.setPosition(x, y);
}

void CCROCODILE::Update(float deltaTime)
{
    OBSTACLE_UPDATE(4)
}