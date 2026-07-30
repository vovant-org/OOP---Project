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
    // ===== CHANGED: origin o CHINH GIUA sprite (thay vi goc tren-trai) -
    // giup (x,y) dai dien dung TAM obstacle, dat lane chinh xac hon.
    // Lat trai/phai gio chi can doi dau scale, khong can doi origin nua =====
    sprite.setOrigin(frameWidth / 2.f, frameHeight / 2.f);
    sprite.setScale(isMovingRight ? 0.28f : -0.28f, 0.28f);

    sprite.setPosition(x, y);
}

void CTRAIN::Update(float deltaTime)
{
    OBSTACLE_UPDATE(4)
}