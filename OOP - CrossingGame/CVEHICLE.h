#pragma once
// CVEHICLE.h
#pragma once
#include "CObstacle.h"

class CVEHICLE : public CObstacle {
protected:
    // Có thể thêm thuộc tính đặc trưng cho phương tiện ở đây
    // Ví dụ: int laneIndex; (Làn đường xe đang chạy)

public:
    CVEHICLE(float startX, float startY, float spd, bool moveRight);
    virtual ~CVEHICLE() = default;

    // Vẫn ép các lớp con phải tự định nghĩa cách di chuyển
    // (Xe đạp, xe máy, xe tải mỗi loại tốc độ/animation khác nhau)
    virtual void Update(float deltaTime) = 0;

    // Bạn có thể thêm hàm ảo như: virtual void Honk() = 0;
};