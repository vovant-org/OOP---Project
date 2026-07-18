#pragma once
// CANGLE.h
#include "CANIMAL.h"

class CANGLE : public CANIMAL {
public:
    CANGLE(float startX, float startY, float spd, bool moveRight);

    void Update(float deltaTime) override;
    // Tùy chỉnh kích thước frameWidth, frameHeight cho phù hợp sprite angle
};