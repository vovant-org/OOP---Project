// RollingRockManager.cpp
#include "RollingRockManager.h"
#include "CPEOPLE.h"
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <algorithm>

namespace
{
    constexpr float WARNING_DURATION = 2.f;
    constexpr float COOLDOWN_DURATION = 10.f;

    constexpr float ROCK_SPEED = 320.f;          // px/s, tu tren xuong
    constexpr float ROCK_TARGET_WIDTH = 300.f;    // px hien thi
    constexpr float ROCK_FRAME_TIME = 0.06f;

    // Hon da co nhieu khoang trong (khoi/lua) quanh vien, nen thu nho
    // hitbox lai so voi kich thuoc hien thi de va cham cam giac chinh
    // xac hon (1.f = full sprite, 0.6f = 60% kich thuoc hien thi)
    constexpr float ROCK_HITBOX_SCALE = 0.15f;

    constexpr float SIGN_SIZE = 100.f;            // px hien thi moi mui ten
    constexpr float SIGN_SPACING = 35.f;

    // Chia be rong canvas thanh cac "cot" cach deu, chon ngau nhien
    // (khong lap) trong so nay -> dam bao cac hon da khong de len nhau
    constexpr int COLUMN_SLOT_COUNT = 6;
}

//==================================================
// Constructor
//==================================================

RollingRockManager::RollingRockManager()
{
    cooldownTimer = COOLDOWN_DURATION;
}

//==================================================
// Setup
//==================================================

bool RollingRockManager::LoadTextures(const std::string& rockPath,
    const std::string& signPath)
{
    bool ok = true;

    if (!rockTexture.loadFromFile(rockPath))
    {
        std::cout << "[RollingRockManager] Cannot load: " << rockPath << "\n";
        ok = false;
    }
    else
    {
        rockTexture.setSmooth(true);
        rockFrameWidth = rockTexture.getSize().x;
        rockFrameHeight = rockTexture.getSize().y / ROCK_FRAME_COUNT;
    }

    if (!signTexture.loadFromFile(signPath))
    {
        std::cout << "[RollingRockManager] Cannot load: " << signPath << "\n";
        ok = false;
    }
    else
    {
        signTexture.setSmooth(true);
        signFrameWidth = signTexture.getSize().x;
        signFrameHeight = signTexture.getSize().y;
    }

    return ok;
}

void RollingRockManager::SetActive(bool isActive)
{
    active = isActive;
}

void RollingRockManager::Reset()
{
    phase = Phase::Idle;
    phaseTimer = 0.f;
    cooldownTimer = COOLDOWN_DURATION;
    warningColumns.clear();
    rocks.clear();
}

//==================================================
// Chon cot ngau nhien, khong trung nhau
//==================================================

void RollingRockManager::PickColumns()
{
    warningColumns.clear();

    int count = 2 + (rand() % 3);   // 2..4

    float slotWidth = canvasW / COLUMN_SLOT_COUNT;
    std::vector<int> slots;
    for (int i = 0; i < COLUMN_SLOT_COUNT; i++)
        slots.push_back(i);

    // Xao slot roi lay `count` phan tu dau (Fisher-Yates rut gon)
    for (int i = (int)slots.size() - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        std::swap(slots[i], slots[j]);
    }

    if (count > (int)slots.size())
        count = (int)slots.size();

    for (int i = 0; i < count; i++)
    {
        float slotCenter = slots[i] * slotWidth + slotWidth / 2.f;
        warningColumns.push_back(slotCenter);
    }
}

void RollingRockManager::StartWarning()
{
    PickColumns();
    phase = Phase::Warning;
    phaseTimer = 0.f;
}

void RollingRockManager::StartRolling()
{
    rocks.clear();

    for (float col : warningColumns)
    {
        Rock r;
        r.x = col;
        r.y = -(float)rockFrameHeight * (ROCK_TARGET_WIDTH / (float)rockFrameWidth);
        r.speed = ROCK_SPEED;
        r.frame = 0;
        r.frameTimer = 0.f;
        r.hasHitPlayer = false;
        rocks.push_back(r);
    }

    phase = Phase::Rolling;
    phaseTimer = 0.f;
}

//==================================================
// Update
//==================================================

void RollingRockManager::Update(float dt, CPEOPLE* player)
{
    if (!active)
        return;

    switch (phase)
    {
    case Phase::Idle:
    {
        cooldownTimer -= dt;
        if (cooldownTimer <= 0.f)
            StartWarning();
        break;
    }

    case Phase::Warning:
    {
        phaseTimer += dt;
        if (phaseTimer >= WARNING_DURATION)
            StartRolling();
        break;
    }

    case Phase::Rolling:
    {
        float scale = ROCK_TARGET_WIDTH / (float)rockFrameWidth;

        for (auto& r : rocks)
        {
            r.y += r.speed * dt;

            r.frameTimer += dt;
            if (r.frameTimer >= ROCK_FRAME_TIME)
            {
                r.frameTimer = 0.f;
                r.frame = (r.frame + 1) % ROCK_FRAME_COUNT;
            }

            if (!r.hasHitPlayer && player && !player->IsStunned())
            {
                float hitW = rockFrameWidth * scale * ROCK_HITBOX_SCALE;
                float hitH = rockFrameHeight * scale * ROCK_HITBOX_SCALE;

                sf::FloatRect rockBox(
                    r.x - hitW / 2.f,
                    r.y - hitH / 2.f,
                    hitW, hitH);

                if (rockBox.intersects(player->GetBoundingBox()))
                {
                    bool bounceRight = (rand() % 2) == 0;
                    player->TriggerStun(bounceRight);
                    r.hasHitPlayer = true;
                }
            }
        }

        // Xoa cac hon da da lan qua het man hinh
        rocks.erase(
            std::remove_if(rocks.begin(), rocks.end(),
                [&](const Rock& r) {
                    return r.y - rockFrameHeight * scale / 2.f > canvasH;
                }),
            rocks.end());

        if (rocks.empty())
        {
            phase = Phase::Idle;
            cooldownTimer = COOLDOWN_DURATION;
        }
        break;
    }
    }
}

//==================================================
// Draw
//==================================================

void RollingRockManager::Draw(sf::RenderWindow& window) const
{
    if (!active)
        return;

    if (phase == Phase::Warning && signFrameWidth > 0)
    {
        // Ve 1 duong mui ten lien tuc tu tren xuong duoi map (khong con
        // kieu "song" quet tuan tu nua), ca duong nhap nhay (blink) dong
        // bo voi nhau trong suot WARNING_DURATION
        constexpr float BLINK_INTERVAL = 0.15f;   // giay moi lan doi sang/toi

        bool blinkOn = std::fmod(phaseTimer, BLINK_INTERVAL * 2.f) < BLINK_INTERVAL;
        sf::Uint8 alpha = blinkOn ? 255 : 60;

        int signCount = (int)(canvasH / SIGN_SPACING) + 1;

        sf::Sprite sp(signTexture);
        float s = SIGN_SIZE / (float)signFrameWidth;
        sp.setScale(s, s);
        sp.setOrigin(signFrameWidth / 2.f, signFrameHeight / 2.f);
        sp.setColor(sf::Color(255, 255, 255, alpha));

        for (float col : warningColumns)
        {
            for (int i = 0; i < signCount; i++)
            {
                sp.setPosition(col, i * SIGN_SPACING + SIGN_SPACING / 2.f);
                window.draw(sp);
            }
        }
    }
    else if (phase == Phase::Rolling && rockFrameWidth > 0)
    {
        float scale = ROCK_TARGET_WIDTH / (float)rockFrameWidth;

        sf::Sprite sp(rockTexture);
        sp.setScale(scale, scale);
        sp.setOrigin(rockFrameWidth / 2.f, rockFrameHeight / 2.f);

        for (const auto& r : rocks)
        {
            sp.setTextureRect(sf::IntRect(
                0, r.frame * rockFrameHeight, rockFrameWidth, rockFrameHeight));
            sp.setPosition(r.x, r.y);
            window.draw(sp);
        }
    }
}