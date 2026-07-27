// CCLOUD.cpp — Sky map
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
    frameTime = 0.20f;  // Cloud: đổi frame rất chậm, lơ lửng

    sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
    sprite.setScale(0.20f, 0.20f);

    // Cloud không cần lật vì đối xứng
    // Nhưng vẫn set origin nếu đi trái để tránh bị nhảy vị trí
    if (!isMovingRight) {
        sprite.setScale(-0.20f, 0.20f);
        sprite.setOrigin((float)frameWidth, 0.f);
    }

    sprite.setPosition(x, y);
}

void CCLOUD::Update(float deltaTime)
{
    OBSTACLE_UPDATE(4)
}