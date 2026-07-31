// CANIMAL.h
#pragma once
#include "CObstacle.h"

class TrafficLight;   // ===== ADDED: forward declare, chỉ cần con trỏ =====

class CANIMAL : public CObstacle {
protected:
    // Có thể thêm thuộc tính đặc trưng cho động vật ở đây
    // Ví dụ: int soundType; (1: Tiếng chim, 2: Tiếng khủng long)

    // ===== ADDED: đèn giao thông điều khiển road của con vật này =====
    TrafficLight* controllingLight = nullptr;
    bool isStopped = false;

public:
    CANIMAL(float startX, float startY, float spd, bool moveRight);
    virtual ~CANIMAL() = default;

    // Vẫn ép tụi con phải tự định nghĩa cách di chuyển (Chim bay khác Khủng long chạy)
    virtual void Update(float deltaTime) = 0;

    // Bạn có thể thêm hàm ảo như: virtual void MakeSound() = 0;

    // ===== ADDED: y hệt CVEHICLE, cho phép CGAME gán/đọc đèn giao thông =====
    void SetTrafficLight(TrafficLight* light) { controllingLight = light; }
    TrafficLight* GetTrafficLight() const { return controllingLight; }

    void SetStopped(bool stopped) { isStopped = stopped; }
    bool IsStopped() const { return isStopped; }
};