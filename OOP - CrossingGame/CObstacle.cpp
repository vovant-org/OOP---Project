
#include "CObstacle.h"

CObstacle::CObstacle(float startX, float startY, float spd, bool moveRight)
    : x(startX), y(startY), speed(spd), isMovingRight(moveRight),
    currentFrame(0), frameWidth(0), frameHeight(0),
    frameTime(0.12f), elapsedTime(0.f)
{
}

void CObstacle::Draw(sf::RenderWindow& window)
{
    sprite.setPosition(x, y);
    window.draw(sprite);
}

sf::FloatRect CObstacle::GetBoundingBox() const
{
    // Thu nhỏ bounding box 20% mỗi cạnh để va chạm cảm giác công bằng hơn
    sf::FloatRect bounds = sprite.getGlobalBounds();
    float shrinkX = bounds.width * 0.2f;
    float shrinkY = bounds.height * 0.2f;
    return sf::FloatRect(
        bounds.left + shrinkX,
        bounds.top + shrinkY,
        bounds.width - shrinkX * 2.f,
        bounds.height - shrinkY * 2.f
    );
}