#pragma once
// CMOTOR.h
#pragma once
#include "CVEHICLE.h"

class CMOTOR : public CVEHICLE {
public:
    CMOTOR(float startX, float startY, float spd, bool moveRight);

    void Update(float deltaTime) override;
    // Tùy chỉnh kích thước frameWidth, frameHeight cho phù hợp sprite xe máy
};