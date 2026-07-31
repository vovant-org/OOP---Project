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
    frameTime = 0.3f;  // Tàu hỏa: animation bánh xe chậm

    sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));

    // Tàu to hơn tất cả — chiếm gần cả lane
    // ===== CHANGED: origin o CHINH GIUA sprite (thay vi goc tren-trai) -
    // giup (x,y) dai dien dung TAM obstacle, dat lane chinh xac hon.
    // Lat trai/phai gio chi can doi dau scale, khong can doi origin nua =====
    sprite.setOrigin(frameWidth / 2.f, frameHeight / 2.f);
    sprite.setScale(isMovingRight ? -0.17f : 0.17f, 0.17f);

    sprite.setPosition(x, y);
}

void CTRAIN::Update(float deltaTime)
{
    // ===== ADDED (Bước 5): dừng hẳn (không di chuyển, không animate)
    // khi đèn giao thông điều khiển lane này đang đỏ =====
    if (isStopped)
        return;

    OBSTACLE_UPDATE(4)
}