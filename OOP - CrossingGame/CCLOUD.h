#pragma once
// CCLOUD.h
#include "CANIMAL.h"

class CCLOUD : public CANIMAL {
public:
    CCLOUD(float startX, float startY, float spd, bool moveRight);

    void Update(float deltaTime) override;
    // Tùy chỉnh kích thước frameWidth, frameHeight cho phù hợp sprite cloud
};