// CCLOUD.cpp // CCLOUD.cpp — Sky map
// Sprite: Skycloud_Obstacle.png
// Đám mây: di chuyển chậm, không có animation phức tạp
#include "CCLOUD.h"
#include "ObstacleHelper.h"

CCLOUD::CCLOUD(float startX, float startY, float spd, bool moveRight)
    : CANIMAL(startX, startY, spd, moveRight)
{
    if (!texture.loadFromFile("Obstacles/Skycloud_Obstacle.png")) return;

    sprite.setTexture(texture);

    // Cloud có thể chỉ có 1-2 frame (lắc lư nhẹ)
    int totalFrames = 4;
    frameWidth = texture.getSize().x / totalFrames;
    frameHeight = texture.getSize().y;
    frameTime = 0.3f;  // Cloud: đổi frame rất chậm, lơ lửng

    sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));

    // ===== CHANGED: origin o CHINH GIUA sprite (thay vi goc tren-trai) -
    // giup (x,y) dai dien dung TAM obstacle, dat lane chinh xac hon.
    // Lat trai/phai gio chi can doi dau scale, khong can doi origin nua =====
    sprite.setOrigin(frameWidth / 2.f, frameHeight / 2.f);
    sprite.setScale(isMovingRight ? -0.9f : 0.9f, 0.9f);

    sprite.setPosition(x, y);
}

void CCLOUD::Update(float deltaTime)
{
    // ===== ADDED: dừng hẳn (không di chuyển, không animate) khi đèn
    // giao thông điều khiển road này đang đỏ (giống CVEHICLE) =====
    if (isStopped)
        return;

    OBSTACLE_UPDATE(4)
}