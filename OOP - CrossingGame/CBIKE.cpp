// CBIKE.cpp — City map
// Sprite: Citybike_Obstacle.png
#include "CBIKE.h"
// CBIKE.cpp — City map
// Sprite: Citybike_Obstacle.png
#include "CBIKE.h"
#include "ObstacleHelper.h"

CBIKE::CBIKE(float startX, float startY, float spd, bool moveRight)
    : CVEHICLE(startX, startY, spd, moveRight)
{
    if (!texture.loadFromFile("Obstacles/Citybike_Obstacle.png")) return;

    sprite.setTexture(texture);

    // Sprite sheet ngang: N frame cùng hàng
    int totalFrames = 4;
    frameWidth = texture.getSize().x / totalFrames;
    frameHeight = texture.getSize().y;
    frameTime = 0.18f;  // Xe đạp: animation vừa phải

    sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));

    // ===== CHANGED: origin o CHINH GIUA sprite (thay vi goc tren-trai) -
    // giup (x,y) dai dien dung TAM obstacle, dat lane chinh xac hon.
    // Lat trai/phai gio chi can doi dau scale, khong can doi origin nua =====
    sprite.setOrigin(frameWidth / 2.f, frameHeight / 2.f);
    sprite.setScale(isMovingRight ? -0.31f : 0.31f, 0.31f);

    sprite.setPosition(x, y);
}

void CBIKE::Update(float deltaTime)
{
    OBSTACLE_UPDATE(4)
}