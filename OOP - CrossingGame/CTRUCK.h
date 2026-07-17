#pragma once
// CTRUCK.h
#pragma once
#include "CVEHICLE.h"

class CTRUCK : public CVEHICLE {
public:
    CTRUCK(float startX, float startY, float spd, bool moveRight);

    void Update(float deltaTime) override;
    // Tùy chỉnh kích thước frameWidth, frameHeight cho phù hợp sprite xe tải (thường to hơn)
};