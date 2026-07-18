#pragma once
// CBRUTE.h
#include "CANIMAL.h"

class CBRUTE : public CANIMAL {
public:
    CBRUTE(float startX, float startY, float spd, bool moveRight);

    void Update(float deltaTime) override;
    // Tùy chỉnh kích thước frameWidth, frameHeight cho phù hợp sprite BRUTE
};