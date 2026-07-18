#pragma once
// CHELLMOTOR.h
#include "CVEHICLE.h"

class CHELLMOTOR : public CVEHICLE {
public:
    CHELLMOTOR(float startX, float startY, float spd, bool moveRight);

    void Update(float deltaTime) override;
    // Tùy chỉnh kích thước frameWidth, frameHeight cho phù hợp sprite xe máy
};