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
    frameTime = 0.18f;  // Angle: bay nhẹ nhàng

    sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));

    // ===== CHANGED: origin o CHINH GIUA sprite (thay vi goc tren-trai) -
    // giup (x,y) dai dien dung TAM obstacle, dat lane chinh xac hon.
    // Lat trai/phai gio chi can doi dau scale, khong can doi origin nua =====
    sprite.setOrigin(frameWidth / 2.f, frameHeight / 2.f);
    sprite.setScale(isMovingRight ? -0.52f : 0.52f, 0.52f);

    sprite.setPosition(x, y);
}

void CANGLE::Update(float deltaTime)
{
    // ===== ADDED: dừng hẳn (không di chuyển, không animate) khi đèn
    // giao thông điều khiển road này đang đỏ (giống CVEHICLE) =====
    if (isStopped)
        return;

    OBSTACLE_UPDATE(4)
}