// MeteoriteManager.cpp
#include "MeteoriteManager.h"
#include "CPEOPLE.h"
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <algorithm>

namespace
{
    constexpr float WARNING_DURATION = 2.f;
    constexpr float IDLE_COOLDOWN = 5.f;    // cho 5s giua 2 lan spawn

    constexpr float METEORITE_FALL_SPEED = 500.f;   // px/s, tu tren xuong
    constexpr float METEORITE_TARGET_WIDTH = 320.f; // px hien thi
    constexpr float METEORITE_FRAME_TIME = 0.07f;   // toc do lap frame 0-1-2 luc roi

    constexpr float IMPACT_DURATION = 0.15f;   // frame 4 (impact flash) hien thi trong bao lau
    constexpr float LANDED_DURATION = 5.f;     // frame 5 (ho da) ton tai + chan duong trong 5s

    // Hitbox gay damage luc VUA cham dat (chi kiem tra 1 lan duy nhat)
    constexpr float METEORITE_HIT_HALF_W = 55.f;
    constexpr float METEORITE_HIT_HALF_H = 40.f;

    // Vung "ho da" chan khong cho player buoc vao trong suot Landed (5s)
    constexpr float METEORITE_BLOCK_HALF_W = 75.f;
    constexpr float METEORITE_BLOCK_HALF_H = 45.f;

    constexpr float SIGN_SIZE = 120.f;   // px hien thi MeteoriteSign

    // ===== ADDED: origin Y cua sprite Meteorite KHONG nam giua khung hinh
    // (vi lua/vet chay keo dai len tren rat khac nhau giua cac frame) - diem
    // "cham dat" thuc te (hon da/ho da) nam gan day khung hinh hon. Neu dung
    // origin = giua khung hinh, hoat anh se bi ve LECH XUONG duoi so voi
    // targetY (vi tri MeteoriteSign vua bien mat) =====
    constexpr float METEORITE_GROUND_RATIO = 0.7f;

    // Luoi vi tri de chon spawn spot ngau nhien khong de trung/de len nhau
    constexpr int SPOT_COLS = 6;
    constexpr int SPOT_ROWS = 3;
    constexpr float SPOT_TOP_MARGIN = 150.f;
    constexpr float SPOT_BOTTOM_MARGIN = 150.f;
}

//==================================================
// Constructor
//==================================================

MeteoriteManager::MeteoriteManager()
{
    cooldownTimer = IDLE_COOLDOWN;
}

//==================================================
// Setup
//==================================================

bool MeteoriteManager::LoadTextures(const std::string& meteoritePath,
    const std::string& signPath)
{
    bool ok = true;

    if (!meteoriteTexture.loadFromFile(meteoritePath))
    {
        std::cout << "[MeteoriteManager] Cannot load: " << meteoritePath << "\n";
        ok = false;
    }
    else
    {
        meteoriteTexture.setSmooth(true);
        meteoriteFrameWidth = meteoriteTexture.getSize().x;
        meteoriteFrameHeight = meteoriteTexture.getSize().y / METEORITE_FRAME_COUNT;
    }

    if (!signTexture.loadFromFile(signPath))
    {
        std::cout << "[MeteoriteManager] Cannot load: " << signPath << "\n";
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

void MeteoriteManager::SetActive(bool isActive)
{
    active = isActive;
}

void MeteoriteManager::Reset()
{
    phase = Phase::Idle;
    phaseTimer = 0.f;
    cooldownTimer = IDLE_COOLDOWN;
    warningSpots.clear();
    meteorites.clear();
    pendingHit = false;
}

//==================================================
// Chon 2-4 vi tri ngau nhien, khong trung nhau (giong quy uoc
// PickColumns() cua RollingRockManager, mo rong sang luoi 2D)
//==================================================

void MeteoriteManager::PickSpawnSpots()
{
    warningSpots.clear();

    int count = 3 + (rand() % 6);   // 2..4

    float colWidth = canvasW / SPOT_COLS;
    float rowHeight = (canvasH - SPOT_TOP_MARGIN - SPOT_BOTTOM_MARGIN) / SPOT_ROWS;

    std::vector<int> slots;
    for (int i = 0; i < SPOT_COLS * SPOT_ROWS; i++)
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
        int col = slots[i] % SPOT_COLS;
        int row = slots[i] / SPOT_COLS;

        float sx = col * colWidth + colWidth / 2.f;
        float sy = SPOT_TOP_MARGIN + row * rowHeight + rowHeight / 2.f;

        warningSpots.push_back(sf::Vector2f(sx, sy));
    }
}

void MeteoriteManager::StartWarning()
{
    PickSpawnSpots();
    phase = Phase::Warning;
    phaseTimer = 0.f;
}

void MeteoriteManager::StartFalling()
{
    meteorites.clear();

    float scale = METEORITE_TARGET_WIDTH / (float)meteoriteFrameWidth;
    float startY = -(float)meteoriteFrameHeight * scale;

    for (const sf::Vector2f& spot : warningSpots)
    {
        MeteoriteInstance m;
        m.targetX = spot.x;
        m.targetY = spot.y;
        m.y = startY;
        m.frame = 0;
        m.frameTimer = 0.f;
        m.state = MetState::Falling;
        m.stateTimer = 0.f;
        m.hasDamagedPlayer = false;
        meteorites.push_back(m);
    }

    phase = Phase::Active;
    phaseTimer = 0.f;
}

//==================================================
// Update
//==================================================

void MeteoriteManager::Update(float dt, CPEOPLE* player)
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
            StartFalling();
        break;
    }

    case Phase::Active:
    {
        for (auto& m : meteorites)
        {
            switch (m.state)
            {
            case MetState::Falling:
            {
                m.y += METEORITE_FALL_SPEED * dt;

                m.frameTimer += dt;
                if (m.frameTimer >= METEORITE_FRAME_TIME)
                {
                    m.frameTimer = 0.f;
                    m.frame = (m.frame + 1) % 3;   // chi lap 3 frame dau (0,1,2) luc dang roi
                }

                if (m.y >= m.targetY)
                {
                    m.y = m.targetY;   // dung dung vi tri dich

                    // ===== Vua cham dat: kiem tra 1 lan duy nhat neu player
                    // dang dung dung tai day thi bao CGAME tru 1 mang =====
                    if (player && !m.hasDamagedPlayer)
                    {
                        sf::FloatRect hitBox(
                            m.targetX - METEORITE_HIT_HALF_W,
                            m.targetY - METEORITE_HIT_HALF_H,
                            METEORITE_HIT_HALF_W * 2.f,
                            METEORITE_HIT_HALF_H * 2.f);

                        if (hitBox.intersects(player->GetBoundingBox()))
                            pendingHit = true;

                        m.hasDamagedPlayer = true;
                    }

                    m.state = MetState::Impact;
                    m.stateTimer = 0.f;
                    m.frame = 3;   // frame thu 4 (impact, hien thi ngan)
                }
                break;
            }

            case MetState::Impact:
            {
                m.stateTimer += dt;
                if (m.stateTimer >= IMPACT_DURATION)
                {
                    m.state = MetState::Landed;
                    m.stateTimer = 0.f;
                    m.frame = 4;   // frame thu 5 (ho da) - chan duong trong LANDED_DURATION
                }
                break;
            }

            case MetState::Landed:
            {
                m.stateTimer += dt;
                break;
            }
            }
        }

        // Xoa cac Meteorite da het thoi gian Landed (5s)
        meteorites.erase(
            std::remove_if(meteorites.begin(), meteorites.end(),
                [&](const MeteoriteInstance& m) {
                    return m.state == MetState::Landed && m.stateTimer >= LANDED_DURATION;
                }),
            meteorites.end());

        if (meteorites.empty())
        {
            phase = Phase::Idle;
            cooldownTimer = IDLE_COOLDOWN;
        }
        break;
    }
    }
}

//==================================================
// Draw
//==================================================

void MeteoriteManager::Draw(sf::RenderWindow& window) const
{
    if (!active)
        return;

    if (phase == Phase::Warning && signFrameWidth > 0)
    {
        // Nhap nhay dong bo tat ca MeteoriteSign trong suot WARNING_DURATION
        constexpr float BLINK_INTERVAL = 0.15f;

        bool blinkOn = std::fmod(phaseTimer, BLINK_INTERVAL * 2.f) < BLINK_INTERVAL;
        sf::Uint8 alpha = blinkOn ? 255 : 60;

        sf::Sprite sp(signTexture);
        float s = SIGN_SIZE / (float)signFrameWidth;
        sp.setScale(s, s);
        sp.setOrigin(signFrameWidth / 2.f, signFrameHeight / 2.f);
        sp.setColor(sf::Color(255, 255, 255, alpha));

        for (const sf::Vector2f& spot : warningSpots)
        {
            sp.setPosition(spot.x, spot.y);
            window.draw(sp);
        }
    }
    else if (phase == Phase::Active && meteoriteFrameWidth > 0)
    {
        float scale = METEORITE_TARGET_WIDTH / (float)meteoriteFrameWidth;

        sf::Sprite sp(meteoriteTexture);
        sp.setScale(scale, scale);
        sp.setOrigin(meteoriteFrameWidth / 2.f, meteoriteFrameHeight * METEORITE_GROUND_RATIO);

        for (const auto& m : meteorites)
        {
            sp.setTextureRect(sf::IntRect(
                0, m.frame * meteoriteFrameHeight, meteoriteFrameWidth, meteoriteFrameHeight));
            sp.setPosition(m.targetX, m.y);
            window.draw(sp);
        }
    }
}

//==================================================
// Truy van cho CGAME
//==================================================

bool MeteoriteManager::ConsumePlayerHit()
{
    bool result = pendingHit;
    pendingHit = false;
    return result;
}

bool MeteoriteManager::IsPositionBlocked(float x, float y) const
{
    if (!active)
        return false;

    for (const auto& m : meteorites)
    {
        if (m.state != MetState::Landed)
            continue;

        sf::FloatRect blockBox(
            m.targetX - METEORITE_BLOCK_HALF_W,
            m.targetY - METEORITE_BLOCK_HALF_H,
            METEORITE_BLOCK_HALF_W * 2.f,
            METEORITE_BLOCK_HALF_H * 2.f);

        if (blockBox.contains(x, y))
            return true;
    }

    return false;
}