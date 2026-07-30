// CBRUTE.cpp — Hell map
// Sprite: Hellbrute_Obstacle.png
#include "CBRUTE.h"
#include "ObstacleHelper.h"

CBRUTE::CBRUTE(float startX, float startY, float spd, bool moveRight)
    : CANIMAL(startX, startY, spd, moveRight)
{
    if (!texture.loadFromFile("Obstacles/Hellbrute_Obstacle.png")) return;

    sprite.setTexture(texture);

    int totalFrames = 4;
    frameWidth = texture.getSize().x / totalFrames;
    frameHeight = texture.getSize().y;
    frameTime = 0.10f;  // Brute: chạy nặng nề, hung hãn

    sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));

    // ===== CHANGED: origin o CHINH GIUA sprite (thay vi goc tren-trai) -
    // giup (x,y) dai dien dung TAM obstacle, dat lane chinh xac hon.
    // Lat trai/phai gio chi can doi dau scale, khong can doi origin nua =====
    sprite.setOrigin(frameWidth / 2.f, frameHeight / 2.f);
    sprite.setScale(isMovingRight ? 0.22f : -0.22f, 0.22f);

    sprite.setPosition(x, y);
}

void CBRUTE::Update(float deltaTime)
{
    OBSTACLE_UPDATE(4)
}