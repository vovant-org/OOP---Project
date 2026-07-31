// TrafficLight.h
#pragma once
#include <SFML/Graphics.hpp>
#include <string>

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
    // ===== CHANGED: cho phep tuy chinh thoi gian do/xanh rieng cho tung den.
    // Neu khong truyen thi dung mac dinh (do 4s, xanh 5s) nhu truoc =====
    TrafficLight(float startX, float startY, float redDur = 4.f, float greenDur = 5.f);

    // Nạp texture (sprite sheet 4 frame: đỏ tĩnh - đỏ nhấp nháy - xanh tĩnh - xanh nhấp nháy)
    bool loadTexture(const std::string& path);

    void Update(float deltaTime); // Hàm này đếm thời gian và chuyển trạng thái
    void Draw(sf::RenderWindow& window);

    // Hàm này cho phép tụi xe cộ/động vật "nhìn" thấy đèn màu gì để dừng lại
    LightState GetCurrentState() const { return currentState; }
};