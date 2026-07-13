// TrafficLight.h
#pragma once
#include <SFML/Graphics.hpp>

// Enum để phân biệt trạng thái đèn cho dễ nhìn code
enum LightState {
    RED,
    GREEN
};

class TrafficLight {
private:
    float x, y;
    LightState currentState;

    sf::Texture texture;
    sf::Sprite sprite;

    // Animation/Timer quản lý việc đổi đèn
    float lightTimer;       // Đếm thời gian hiện tại
    float redDuration;      // Thời lượng đèn đỏ (VD: 5 giây)
    float greenDuration;    // Thời lượng đèn xanh (VD: 5 giây)

    int frameWidth, frameHeight;

public:
    // Khởi tạo đèn ở một tọa độ cụ thể
    TrafficLight(float startX, float startY);

    void Update(float deltaTime); // Hàm này đếm thời gian và chuyển trạng thái
    void Draw(sf::RenderWindow& window);

    // Hàm này cho phép tụi xe cộ/động vật "nhìn" thấy đèn màu gì để dừng lại
    LightState GetCurrentState() const { return currentState; }
};
