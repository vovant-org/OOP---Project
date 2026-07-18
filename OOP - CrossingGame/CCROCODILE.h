#pragma once
#include "CANIMAL.h"

class CCROCODILE : public CANIMAL {
public:
    CCROCODILE(float startX, float startY, float spd, bool moveRight);

    void Update(float deltaTime) override;
};