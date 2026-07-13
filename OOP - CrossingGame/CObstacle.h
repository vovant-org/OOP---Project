// CObstacle.h
#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

using namespace std;

class CObstacle {
protected:
    float x, y;          // Tọa độ hiện tại
    float speed;         // Tốc độ di chuyển
    bool isMovingRight;  // Cờ kiểm tra hướng di chuyển

    // SFML Components để vẽ
    sf::Texture texture;
    sf::Sprite sprite;

    // Quản lý Animation (Dải 4 hình của ông)
    int currentFrame;
    int frameWidth;
    int frameHeight;
    float frameTime;     // Thời gian giữ 1 frame (để canh tốc độ vẫy cánh/chạy)
    float elapsedTime;   // Thời gian đã trôi qua

public:
    CObstacle(float startX, float startY, float spd, bool moveRight);
    virtual ~CObstacle() = default;

    // Các hàm ảo để con tự định nghĩa
    virtual void Update(float deltaTime) = 0; // Cập nhật tọa độ và Animation
    virtual void Draw(sf::RenderWindow& window); // Vẽ ra màn hình

    // Hàm này siêu quan trọng để CGAME check đụng xe/chim
    virtual sf::FloatRect GetBoundingBox() const;

    // Getters & Setters cơ bản
    float getX() const { return x; }
    float getY() const { return y; }
    bool getDirection() const { return isMovingRight; }
};
