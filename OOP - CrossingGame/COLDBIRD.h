#pragma once
#include "CANIMAL.h"

class CBIRD : public CANIMAL {
public:
    CBIRD(float startX, float startY, float spd, bool moveRight);

    void Update(float deltaTime) override;
    // Chim bay, có thể vẫy cánh nhanh hơn (frameTime nhỏ hơn)
};