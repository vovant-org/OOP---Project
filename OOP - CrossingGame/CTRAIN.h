#pragma once
// CTRAIN.h
#include "CVEHICLE.h"

class CTRAIN : public CVEHICLE {
public:
    CTRAIN(float startX, float startY, float spd, bool moveRight);

    void Update(float deltaTime) override;
    // Tùy chỉnh kích thước frameWidth, frameHeight cho phù hợp sprite tàu hỏi (thường to hơn)
};