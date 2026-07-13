
#include "CANIMAL.h"

class CBEAST : public CANIMAL {
public:
    CBEAST(float startX, float startY, float spd, bool moveRight);

    void Update(float deltaTime) override;
    // Tùy chỉnh kích thước frameWidth, frameHeight cho phù hợp sprite Dialga
};