#pragma once
// CBIRD.h
#include "CANIMAL.h"

class CBIRD : public CANIMAL {
public:
    CBIRD(float startX, float startY, float spd, bool moveRight);

    void Update(float deltaTime) override;
};