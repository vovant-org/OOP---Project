#pragma once
// CPIGLIN.h
#include "CANIMAL.h"

class CPIGLIN : public CANIMAL {
public:
    CPIGLIN(float startX, float startY, float spd, bool moveRight);

    void Update(float deltaTime) override;
    // Tùy chỉnh kích thước frameWidth, frameHeight cho phù hợp sprite piglin
};