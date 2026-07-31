#pragma once
// CVEHICLE.h
#include "CObstacle.h"

class TrafficLight;   // ===== ADDED (Bước 5): forward declare, chỉ cần con trỏ =====

class CVEHICLE : public CObstacle {
protected:
    // Có thể thêm thuộc tính đặc trưng cho phương tiện ở đây
    // Ví dụ: int laneIndex; (Làn đường xe đang chạy)

    // ===== ADDED (Bước 5): đèn giao thông điều khiển lane của xe này =====
    TrafficLight* controllingLight = nullptr;
    bool isStopped = false;

public:
    CVEHICLE(float startX, float startY, float spd, bool moveRight);
    virtual ~CVEHICLE() = default;

    // Vẫn ép các lớp con phải tự định nghĩa cách di chuyển
    // (Xe đạp, xe máy, xe tải mỗi loại tốc độ/animation khác nhau)
    virtual void Update(float deltaTime) = 0;

    // Bạn có thể thêm hàm ảo như: virtual void Honk() = 0;

    // ===== ADDED (Bước 5) =====
    void SetTrafficLight(TrafficLight* light) { controllingLight = light; }
    TrafficLight* GetTrafficLight() const { return controllingLight; }

    void SetStopped(bool stopped) { isStopped = stopped; }
    bool IsStopped() const { return isStopped; }
};