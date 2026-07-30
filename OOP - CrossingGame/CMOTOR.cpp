// CMOTOR.cpp — City map
// Sprite: Citymotor_Obstacle.png
#include "CMOTOR.h"
#include "ObstacleHelper.h"

CMOTOR::CMOTOR(float startX, float startY, float spd, bool moveRight)
    : CVEHICLE(startX, startY, spd, moveRight)
{
    if (!texture.loadFromFile("Obstacles/Citymotor_Obstacle.png")) return;

    sprite.setTexture(texture);

    int totalFrames = 4;
    frameWidth = texture.getSize().x / totalFrames;
    frameHeight = texture.getSize().y;
    frameTime = 0.2f;  // Xe máy: animation nhanh hơn xe đạp

    sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));

    // ===== CHANGED: origin o CHINH GIUA sprite (thay vi goc tren-trai) -
    // giup (x,y) dai dien dung TAM obstacle, dat lane chinh xac hon.
    // Lat trai/phai gio chi can doi dau scale, khong can doi origin nua =====
    sprite.setOrigin(frameWidth / 2.f, frameHeight / 2.f);
    sprite.setScale(isMovingRight ? -0.35f : 0.35f, 0.35f);

    sprite.setPosition(x, y);
}

void CMOTOR::Update(float deltaTime)
{
    OBSTACLE_UPDATE(4)
}