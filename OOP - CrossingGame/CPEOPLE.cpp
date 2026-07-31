// CPEOPLE.cpp
#include "CPEOPLE.h"
#include <iostream>
#include <algorithm>

namespace
{
    // Spritesheet nhan vat: 4 cot (frame) x 5 hang (huong)
    // Hang: 0=UP 1=DOWN 2=LEFT 3=RIGHT 4=DIE (khop voi enum direction)
    constexpr int FRAME_COLUMNS = 4;
    constexpr int DIRECTION_ROWS = 5;

    constexpr float DEFAULT_FRAME_TIME = 0.12f;

    // Scale hien thi nhan vat tren canvas 1280x720 (frame goc ~260x220px
    // qua to neu ve nguyen kich thuoc)
    constexpr float CHAR_SCALE = 0.25f;   // ===== CHANGED: to hon (0.25 → 0.35) =====

    // 1 buoc di chuyen (tam thoi dung gia tri co dinh - se doi lai
    // theo kich thuoc o luoi thuc te cua map khi lam collision/tile)
    constexpr float MOVE_STEP = 25.f;     // ===== CHANGED: ngan hon (64 → 40) =====

    // Canvas gameplay - khop voi WIN_W/WIN_H (1280x720) trong main.cpp
    constexpr float CANVAS_W = 1280.f;
    constexpr float CANVAS_H = 720.f;

    // ===== ADDED (Bước 2): giu nhan vat khong di ra ngoai man hinh =====
    // Origin cua sprite dat o giua-duoi (xem loadTexture), nen:
    //   - be ngang sprite chiem [x - halfW, x + halfW]
    //   - be doc sprite chiem   [y - fullH, y]
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
    speed(200.f),
    isAlive(true),
    currentFrame(0),
    direction(1),           // mac dinh: quay mat ra (DOWN)
    frameWidth(0),
    frameHeight(0),
    frameTime(DEFAULT_FRAME_TIME),
    elapsedTime(0.f)
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

    // Origin o giua-duoi sprite, de x,y dai dien cho vi tri "chan
    // dung" cua nhan vat tren o luoi - thuan tien cho collision/tile sau nay
    sprite.setOrigin(frameWidth / 2.f, (float)frameHeight);

    sprite.setScale(CHAR_SCALE, CHAR_SCALE);
    sprite.setPosition(x, y);

    return true;
}

//==================================================
// Movement
// (Di chuyen theo o luoi - doi huong + nhay 1 buoc ngay lap tuc,
//  gioi han trong canvas. CGAME goi cac ham nay tu HandleInput())
//==================================================

void CPEOPLE::MoveUp()
{
    direction = 0;
    y -= MOVE_STEP;
    ClampToCanvas(x, y, frameWidth, frameHeight);

    currentFrame = 0;
    elapsedTime = 0.f;
    sprite.setTextureRect(sf::IntRect(
        0, direction * frameHeight, frameWidth, frameHeight));
    sprite.setPosition(x, y);
}

void CPEOPLE::MoveDown()
{
    direction = 1;
    y += MOVE_STEP;
    ClampToCanvas(x, y, frameWidth, frameHeight);

    currentFrame = 0;
    elapsedTime = 0.f;
    sprite.setTextureRect(sf::IntRect(
        0, direction * frameHeight, frameWidth, frameHeight));
    sprite.setPosition(x, y);
}

void CPEOPLE::MoveLeft()
{
    direction = 2;
    x -= MOVE_STEP;
    ClampToCanvas(x, y, frameWidth, frameHeight);

    currentFrame = 0;
    elapsedTime = 0.f;
    sprite.setTextureRect(sf::IntRect(
        0, direction * frameHeight, frameWidth, frameHeight));
    sprite.setPosition(x, y);
}

void CPEOPLE::MoveRight()
{
    direction = 3;
    x += MOVE_STEP;
    ClampToCanvas(x, y, frameWidth, frameHeight);

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

    sprite.setTextureRect(sf::IntRect(
        0, direction * frameHeight, frameWidth, frameHeight));
    sprite.setPosition(x, y);
}

// ===== ADDED (Bước 4) =====
void CPEOPLE::TriggerDeath()
{
    isAlive = false;
    direction = 4;   // DIE
    currentFrame = 0;
    elapsedTime = 0.f;

    sprite.setTextureRect(sf::IntRect(
        0, direction * frameHeight, frameWidth, frameHeight));
}