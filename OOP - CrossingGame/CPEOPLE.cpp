// CPEOPLE.cpp
#include "CPEOPLE.h"
#include <iostream>
#include <algorithm>

namespace
{
    // Spritesheet: 4 cot (frame) x 5 hang (huong), hang khop enum direction
    constexpr int FRAME_COLUMNS = 4;
    constexpr int DIRECTION_ROWS = 5;

    constexpr float DEFAULT_FRAME_TIME = 0.12f;
    constexpr float DEFAULT_MOVE_COOLDOWN = 0.2f;

    // Scale hien thi tren canvas 1280x720
    constexpr float CHAR_SCALE = 0.25f;

    // 1 buoc di chuyen (o luoi)
    constexpr float MOVE_STEP = 28.f;

    constexpr float CANVAS_W = 1280.f;
    constexpr float CANVAS_H = 720.f;

    // Origin sprite o giua-duoi, nen:
    //   be ngang chiem [x - halfW, x + halfW], be doc chiem [y - fullH, y]
    void ClampToCanvas(float& px, float& py, int frameWidth, int frameHeight)
    {
        float halfW = frameWidth * CHAR_SCALE / 2.f;
        float fullH = frameHeight * CHAR_SCALE;

        px = std::max(halfW, std::min(px, CANVAS_W - halfW));
        py = std::max(fullH, std::min(py, CANVAS_H));
    }
}

//==================================================
// Constructor
//==================================================

CPEOPLE::CPEOPLE(float startX, float startY)
    : x(startX), y(startY),
    isAlive(true),
    currentFrame(0),
    direction(1),           // mac dinh: quay mat ra (DOWN)
    frameWidth(0),
    frameHeight(0),
    frameTime(DEFAULT_FRAME_TIME),
    elapsedTime(0.f),
    moveCooldown(DEFAULT_MOVE_COOLDOWN),
    moveCooldownTimer(0.f)
{
}

//==================================================
// Texture
//==================================================

bool CPEOPLE::loadTexture(const std::string& path)
{
    if (!texture.loadFromFile(path))
    {
        std::cout << "[CPEOPLE] Cannot load: " << path << "\n";
        return false;
    }

    sprite.setTexture(texture);

    frameWidth = texture.getSize().x / FRAME_COLUMNS;
    frameHeight = texture.getSize().y / DIRECTION_ROWS;

    sprite.setTextureRect(sf::IntRect(
        0, direction * frameHeight, frameWidth, frameHeight));

    // Origin giua-duoi: x,y dai dien vi tri "chan dung" cua nhan vat
    sprite.setOrigin(frameWidth / 2.f, (float)frameHeight);

    sprite.setScale(CHAR_SCALE, CHAR_SCALE);
    sprite.setPosition(x, y);

    return true;
}

//==================================================
// Movement (di theo o luoi, gioi han cooldown theo tung nhan vat)
//==================================================

void CPEOPLE::MoveUp()
{
    if (moveCooldownTimer > 0.f)
        return;

    direction = 0;
    y -= MOVE_STEP;
    ClampToCanvas(x, y, frameWidth, frameHeight);
    moveCooldownTimer = moveCooldown;

    currentFrame = 0;
    elapsedTime = 0.f;
    sprite.setTextureRect(sf::IntRect(
        0, direction * frameHeight, frameWidth, frameHeight));
    sprite.setPosition(x, y);
}

void CPEOPLE::MoveDown()
{
    if (moveCooldownTimer > 0.f)
        return;

    direction = 1;
    y += MOVE_STEP;
    ClampToCanvas(x, y, frameWidth, frameHeight);
    moveCooldownTimer = moveCooldown;

    currentFrame = 0;
    elapsedTime = 0.f;
    sprite.setTextureRect(sf::IntRect(
        0, direction * frameHeight, frameWidth, frameHeight));
    sprite.setPosition(x, y);
}

void CPEOPLE::MoveLeft()
{
    if (moveCooldownTimer > 0.f)
        return;

    direction = 2;
    x -= MOVE_STEP;
    ClampToCanvas(x, y, frameWidth, frameHeight);
    moveCooldownTimer = moveCooldown;

    currentFrame = 0;
    elapsedTime = 0.f;
    sprite.setTextureRect(sf::IntRect(
        0, direction * frameHeight, frameWidth, frameHeight));
    sprite.setPosition(x, y);
}

void CPEOPLE::MoveRight()
{
    if (moveCooldownTimer > 0.f)
        return;

    direction = 3;
    x += MOVE_STEP;
    ClampToCanvas(x, y, frameWidth, frameHeight);
    moveCooldownTimer = moveCooldown;

    currentFrame = 0;
    elapsedTime = 0.f;
    sprite.setTextureRect(sf::IntRect(
        0, direction * frameHeight, frameWidth, frameHeight));
    sprite.setPosition(x, y);
}

//==================================================
// Update / Draw
//==================================================

void CPEOPLE::Update(float dt)
{
    if (moveCooldownTimer > 0.f)
        moveCooldownTimer -= dt;

    if (frameWidth <= 0 || frameHeight <= 0)
        return;

    elapsedTime += dt;

    if (elapsedTime < frameTime)
        return;

    elapsedTime = 0.f;

    if (direction == 4)
    {
        // DIE: choi hoat hinh 1 lan roi dung o frame cuoi, khong lap
        if (currentFrame < FRAME_COLUMNS - 1)
            currentFrame++;
    }
    else
    {
        currentFrame = (currentFrame + 1) % FRAME_COLUMNS;
    }

    sprite.setTextureRect(sf::IntRect(
        currentFrame * frameWidth, direction * frameHeight,
        frameWidth, frameHeight));
}

void CPEOPLE::Draw(sf::RenderWindow& window)
{
    window.draw(sprite);
}

//==================================================
// Bounding box / Reset
//==================================================

sf::FloatRect CPEOPLE::GetBoundingBox() const
{
    return sprite.getGlobalBounds();
}

void CPEOPLE::Reset(float startX, float startY)
{
    x = startX;
    y = startY;
    isAlive = true;
    direction = 1;   // DOWN
    currentFrame = 0;
    elapsedTime = 0.f;
    moveCooldownTimer = 0.f;

    sprite.setTextureRect(sf::IntRect(
        0, direction * frameHeight, frameWidth, frameHeight));
    sprite.setPosition(x, y);
}

void CPEOPLE::TriggerDeath()
{
    isAlive = false;
    direction = 4;   // DIE
    currentFrame = 0;
    elapsedTime = 0.f;

    sprite.setTextureRect(sf::IntRect(
        0, direction * frameHeight, frameWidth, frameHeight));
}