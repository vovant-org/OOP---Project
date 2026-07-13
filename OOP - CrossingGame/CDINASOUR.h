#pragma once
#include "CANIMAL.h"

class CDINOSAUR : public CANIMAL {
public:
    CDINOSAUR(float startX, float startY, float spd, bool moveRight);

    void Update(float deltaTime) override;
    // Khủng long chạy dưới đất, tốc độ animation trung bình
};