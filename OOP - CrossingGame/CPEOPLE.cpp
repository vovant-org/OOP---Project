// CPEOPLE.cpp
#include "CPEOPLE.h"
#include <iostream>
#include <algorithm>
#include <cmath>

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
    constexpr float MOVE_STEP = 21.f;

    constexpr float CANVAS_W = 1280.f;
    constexpr float CANVAS_H = 720.f;

    // ===== ADDED: thong so cho trang thai "choang" (khong doi animation,
    // chi khoa di chuyen + bat ra trai/phai theo quan tinh giam dan) =====
    constexpr float STUN_DURATION = 1.5f;   // choang dung 1.5 giay (RollingRockManager)
    constexpr float BOUNCE_SPEED = 260.f;  // px/s, van toc bat ra ban dau
    constexpr float BOUNCE_DECAY = 6.f;    // he so giam dan van toc bat ra

    // ===== ADDED: choang rieng khi bi TrainManager day lui (co tinh chay
    // vao road dang co tau chay) - 1 giay, ngan hon STUN_DURATION o tren =====
    constexpr float PUSHBACK_STUN_DURATION = 1.f;

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
    moveCooldownTimer(0.f),
    isStunned(false),
    stunTimer(0.f),
    bounceVelocity(0.f),
    bounceVelocityY(0.f),
    windPushVelocity(0.f)
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
    if (isStunned)         // ===== ADDED: dang choang thi khong di chuyen duoc =====
        return;

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
    if (isStunned)
        return;

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
    if (isStunned)
        return;

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
    if (isStunned)
        return;

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

    // ===== ADDED: dang choang - bi day (bounce) trai/phai theo quan tinh
    // giam dan, khong nhan input di chuyen (xem MoveUp/Down/Left/Right).
    // Khong doi animation - giu nguyen frame/huong dang co =====
    if (isStunned)
    {
        stunTimer -= dt;

        x += bounceVelocity * dt;
        y += bounceVelocityY * dt;

        bounceVelocity *= std::exp(-BOUNCE_DECAY * dt);
        bounceVelocityY *= std::exp(-BOUNCE_DECAY * dt);

        ClampToCanvas(x, y, frameWidth, frameHeight);
        sprite.setPosition(x, y);

        if (stunTimer <= 0.f)
        {
            isStunned = false;
            bounceVelocity = 0.f;
            bounceVelocityY = 0.f;
        }

        return;
    }

    // ===== ADDED: gio day dat lien tuc (khong khoa di chuyen, chi troi
    // them ngang) - ap dung ca khi dang bam phim di chuyen binh thuong =====
    if (windPushVelocity != 0.f)
    {
        x += windPushVelocity * dt;
        ClampToCanvas(x, y, frameWidth, frameHeight);
        sprite.setPosition(x, y);
    }

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
    sf::FloatRect box = sprite.getGlobalBounds();

    // Thu nhỏ hitbox theo % chiều rộng/cao để tránh va chạm "oan"
    float shrinkX = box.width * 0.25f;   // giảm 25% mỗi bên ngang
    float shrinkY = box.height * 0.25f;  // giảm 25% mỗi bên dọc

    box.left += shrinkX;
    box.top += shrinkY;
    box.width -= shrinkX * 2.f;
    box.height -= shrinkY * 2.f;

    return box;
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

    // ===== ADDED: huy trang thai choang neu dang bi (vi du CGAME::OnHit
    // dua player ve vach xuat phat trong luc dang choang) =====
    isStunned = false;
    stunTimer = 0.f;
    bounceVelocity = 0.f;
    bounceVelocityY = 0.f;
    windPushVelocity = 0.f;

    sprite.setTextureRect(sf::IntRect(
        0, direction * frameHeight, frameWidth, frameHeight));
    sprite.setPosition(x, y);
}

// ===== ADDED (Load Game - khoi phuc dung vi tri/trang thai) =====
void CPEOPLE::RestoreState(float px, float py, int dir, int frame)
{
    x = px;
    y = py;
    ClampToCanvas(x, y, frameWidth, frameHeight);

    if (dir < 0 || dir > 4) dir = 1;

    // DIE: khong the "Continue" o trang thai dang chet - tra ve DOWN
    // dung yen, giong fallback da dung o preview Continue Menu
    if (dir == 4) dir = 1;
    direction = dir;

    if (frame < 0 || frame >= FRAME_COLUMNS) frame = 0;
    currentFrame = frame;

    isAlive = true;
    elapsedTime = 0.f;
    moveCooldownTimer = 0.f;

    isStunned = false;
    stunTimer = 0.f;
    bounceVelocity = 0.f;
    bounceVelocityY = 0.f;
    windPushVelocity = 0.f;

    sprite.setTextureRect(sf::IntRect(
        currentFrame * frameWidth, direction * frameHeight,
        frameWidth, frameHeight));
    sprite.setPosition(x, y);
}

void CPEOPLE::TriggerDeath()
{
    isAlive = false;

    // ===== ADDED: huy choang neu dang bi khi chet =====
    isStunned = false;

    direction = 4;   // DIE
    currentFrame = 0;
    elapsedTime = 0.f;

    sprite.setTextureRect(sf::IntRect(
        0, direction * frameHeight, frameWidth, frameHeight));
}

// ===== ADDED: kich hoat trang thai choang 1.5 giay - goi tu
// RollingRockManager::Update() khi hon da va cham player. Khong doi
// animation, chi khoa di chuyen va bat nhan vat ra trai/phai =====
void CPEOPLE::TriggerStun(bool bounceRight)
{
    if (!isAlive || isStunned)
        return;

    isStunned = true;
    stunTimer = STUN_DURATION;
    bounceVelocity = (bounceRight ? 1.f : -1.f) * BOUNCE_SPEED;
    bounceVelocityY = 0.f;
}

// ===== ADDED: kich hoat trang thai choang 1 giay THEO TRUC DOC - goi tu
// TrainManager::TryBlockEntry() khi player co tinh di chuyen vao 1 road
// dang co tau chay. Khac TriggerStun (bat trai/phai), ham nay day
// nguoc len/xuong tuy huong player vua co gang di =====
void CPEOPLE::TriggerPushback(bool pushDown)
{
    if (!isAlive || isStunned)
        return;

    isStunned = true;
    stunTimer = PUSHBACK_STUN_DURATION;
    bounceVelocity = 0.f;
    bounceVelocityY = (pushDown ? 1.f : -1.f) * BOUNCE_SPEED;
}

// ===== ADDED: du doan vi tri neu di chuyen 1 buoc (MOVE_STEP) theo huong
// dir, dung chung logic clamp voi Move...() nhung KHONG thay doi state
// cua nhan vat - de CGAME::HandleInput() kiem tra truoc voi MeteoriteManager =====
void CPEOPLE::PeekNextPosition(int dir, float& outX, float& outY) const
{
    outX = x;
    outY = y;

    switch (dir)
    {
    case 0: outY -= MOVE_STEP; break;   // UP
    case 1: outY += MOVE_STEP; break;   // DOWN
    case 2: outX -= MOVE_STEP; break;   // LEFT
    case 3: outX += MOVE_STEP; break;   // RIGHT
    default: break;
    }

    ClampToCanvas(outX, outY, frameWidth, frameHeight);
}