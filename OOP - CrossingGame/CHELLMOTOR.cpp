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

    // ===== CHANGED: origin o CHINH GIUA sprite (thay vi goc tren-trai) -
    // giup (x,y) dai dien dung TAM obstacle, dat lane chinh xac hon.
    // Lat trai/phai gio chi can doi dau scale, khong can doi origin nua =====
    sprite.setOrigin(frameWidth / 2.f, frameHeight / 2.f);
    sprite.setScale(isMovingRight ? 0.17f : -0.17f, 0.17f);

    sprite.setPosition(x, y);
}

void CHELLMOTOR::Update(float deltaTime)
{
    OBSTACLE_UPDATE(4)
}