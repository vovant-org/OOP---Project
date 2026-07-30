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

    // ===== CHANGED: origin o CHINH GIUA sprite (thay vi goc tren-trai) -
    // giup (x,y) dai dien dung TAM obstacle, dat lane chinh xac hon.
    // Lat trai/phai gio chi can doi dau scale, khong can doi origin nua =====
    sprite.setOrigin(frameWidth / 2.f, frameHeight / 2.f);
    sprite.setScale(isMovingRight ? -0.15f : 0.15f, 0.15f);

    sprite.setPosition(x, y);
}

void COLDBIRD::Update(float deltaTime)
{
    OBSTACLE_UPDATE(4)
}