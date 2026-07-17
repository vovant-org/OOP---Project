#pragma once
// CBIKE.h
#pragma once
#include "CVEHICLE.h"

class CBIKE : public CVEHICLE {
public:
    CBIKE(float startX, float startY, float spd, bool moveRight);

    void Update(float deltaTime) override;
    // Tùy chỉnh kích thước frameWidth, frameHeight cho phù hợp sprite xe đạp
};