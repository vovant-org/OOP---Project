#pragma once
#include "CANIMAL.h"

class CFISH : public CANIMAL {
public:
    CFISH(float startX, float startY, float spd, bool moveRight);

    void Update(float deltaTime) override;
};