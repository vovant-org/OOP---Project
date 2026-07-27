// ObstacleHelper.h
// Macro dùng chung cho Update() của tất cả obstacle
// Tránh lặp đi lặp lại cùng một đoạn code animation + movement
#pragma once

// Kích thước màn hình
#define SCREEN_WIDTH  1920.f
#define SCREEN_HEIGHT 1080.f

// Macro Update chung: animation + di chuyển + wrap-around
// frameCount: số frame trong sprite sheet (thường là 4)
#define OBSTACLE_UPDATE(frameCount)                                     \
    elapsedTime += deltaTime;                                           \
    if (elapsedTime >= frameTime) {                                     \
        elapsedTime = 0.f;                                              \
        currentFrame = (currentFrame + 1) % (frameCount);              \
        sprite.setTextureRect(sf::IntRect(                              \
            currentFrame * frameWidth, 0, frameWidth, frameHeight));    \
    }                                                                   \
    x += (isMovingRight ? 1.f : -1.f) * speed * deltaTime;            \
    if (isMovingRight  && x >  SCREEN_WIDTH)               x = -(float)frameWidth * sprite.getScale().x;  \
    if (!isMovingRight && x < -(float)frameWidth * sprite.getScale().x) x = SCREEN_WIDTH;                 \
    sprite.setPosition(x, y);
