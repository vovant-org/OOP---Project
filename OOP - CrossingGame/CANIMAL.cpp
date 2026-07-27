// CANIMAL.cpp
#include "CANIMAL.h"

CANIMAL::CANIMAL(float startX, float startY, float spd, bool moveRight)
    : CObstacle(startX, startY, spd, moveRight)
{
    // Lớp base không load texture ở đây
    // Các lớp con tự load texture phù hợp với sprite của chúng
}