// WindGustManager.cpp
#include "WindGustManager.h"
#include "CPEOPLE.h"
#include "AudioManager.h"
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <algorithm>

namespace
{
    // ===== ADDED: ten dinh danh am thanh trong AudioManager - dung
    // playControlledSound/stopControlledSound (khong loop, nhung can stop
    // duoc dung luc de khop voi ACTIVE_DURATION) =====
    const std::string WIND_GUST_SOUND_NAME = "wind_gust";

    constexpr float WARNING_DURATION = 1.5f;
    constexpr float ACTIVE_DURATION = 1.8f;
    constexpr float IDLE_COOLDOWN = 6.f;

    constexpr float WIND_PUSH_SPEED = 190.f;   // px/s, day nhan vat troi ngang
    constexpr float WIND_FRAME_TIME = 0.13f;   // toc do xen ke frame1/frame2

    constexpr float SIGN_SIZE = 70.f;
    constexpr int   SIGN_PER_ROAD = 2;         // so icon xep doc trong PHAM VI 1 road
    constexpr float SIGN_MARGIN_X = 60.f;

    // ===== ADDED: Sky map co 4 "road" (moi road tuong ung 2 gia tri trong
    // SKY_LANE_Y ben CGAME.cpp - vi du Road0 = {125,180}). Toa do bien duoi
    // day la trung diem giua cac road lien tiep, giup gio giat + push chi
    // gioi han DUNG trong 1 road, khong con anh huong toan map nua =====
    constexpr int ROAD_COUNT = 4;
    constexpr float ROAD_TOP[ROAD_COUNT] = { 125.f, 255.f, 375.f, 510.f };
    constexpr float ROAD_BOTTOM[ROAD_COUNT] = { 255.f, 375.f, 510.f, 570.f };
}

//==================================================
// Constructor
//==================================================

WindGustManager::WindGustManager()
{
    cooldownTimer = IDLE_COOLDOWN;
}

//==================================================
// Setup
//==================================================

bool WindGustManager::LoadTextures(const std::string& windPath1,
    const std::string& windPath2, const std::string& signPath)
{
    bool ok = true;

    if (!windTexture1.loadFromFile(windPath1))
    {
        std::cout << "[WindGustManager] Cannot load: " << windPath1 << "\n";
        ok = false;
    }
    else
    {
        windTexture1.setSmooth(true);
        windWidth = windTexture1.getSize().x;
        windHeight = windTexture1.getSize().y;
    }

    if (!windTexture2.loadFromFile(windPath2))
    {
        std::cout << "[WindGustManager] Cannot load: " << windPath2 << "\n";
        ok = false;
    }
    else
    {
        windTexture2.setSmooth(true);
    }

    if (!signTexture.loadFromFile(signPath))
    {
        std::cout << "[WindGustManager] Cannot load: " << signPath << "\n";
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

// ===== ADDED: gan AudioManager dung chung =====
void WindGustManager::SetAudioManager(AudioManager* manager)
{
    audio = manager;
}

// ===== CHANGED: nap am thanh vao AudioManager (yeu cau SetAudioManager()
// da duoc goi truoc) thay vi tu giu buffer rieng =====
bool WindGustManager::LoadSound(const std::string& gustSoundPath)
{
    if (!audio)
    {
        std::cout << "[WindGustManager] Chua gan AudioManager, khong the nap am thanh\n";
        return false;
    }

    return audio->loadSound(WIND_GUST_SOUND_NAME, gustSoundPath);
}

void WindGustManager::SetActive(bool isActive)
{
    active = isActive;
}

void WindGustManager::Reset()
{
    phase = Phase::Idle;
    phaseTimer = 0.f;
    cooldownTimer = IDLE_COOLDOWN;
    windFrame = 0;
    windFrameTimer = 0.f;
    gusts.clear();

    if (audio) audio->stopControlledSound(WIND_GUST_SOUND_NAME);   // ===== CHANGED
}

//==================================================
// Chon 1-3 road ngau nhien (khong trung), moi road 1 huong ngau nhien
// DOC LAP (khong bi ep cung huong voi nhau - giong quy uoc PickColumns()
// cua RollingRockManager, mo rong them: moi phan tu co thuoc tinh huong
// rieng thay vi dung chung 1 huong cho ca nhom)
//==================================================

void WindGustManager::PickGusts()
{
    gusts.clear();

    int count = 1 + (rand() % 3);   // 1..3

    std::vector<int> roads;
    for (int i = 0; i < ROAD_COUNT; i++)
        roads.push_back(i);

    // Xao roads roi lay `count` phan tu dau (Fisher-Yates rut gon)
    for (int i = (int)roads.size() - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        std::swap(roads[i], roads[j]);
    }

    if (count > (int)roads.size())
        count = (int)roads.size();

    for (int i = 0; i < count; i++)
    {
        GustInstance g;
        g.roadIndex = roads[i];
        g.blowsRight = (rand() % 2) == 0;   // moi luong tu boc xam huong rieng
        gusts.push_back(g);
    }
}

//==================================================
// Phase transitions
//==================================================

void WindGustManager::StartWarning()
{
    PickGusts();
    phase = Phase::Warning;
    phaseTimer = 0.f;
}

void WindGustManager::StartActive()
{
    // gusts (road + huong) da duoc chon tu StartWarning(), giu nguyen -
    // luong gio thoi dung nhung road vua canh bao
    phase = Phase::Active;
    phaseTimer = 0.f;
    windFrame = 0;
    windFrameTimer = 0.f;

    // ===== CHANGED: qua AudioManager (playControlledSound, khong loop -
    // chi de co the stopControlledSound() cat dung luc neu file dai hon
    // ACTIVE_DURATION) =====
    if (audio) audio->playControlledSound(WIND_GUST_SOUND_NAME, false);
}

//==================================================
// Update
//==================================================

void WindGustManager::Update(float dt, CPEOPLE* player)
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
            StartActive();
        break;
    }

    case Phase::Active:
    {
        phaseTimer += dt;

        windFrameTimer += dt;
        if (windFrameTimer >= WIND_FRAME_TIME)
        {
            windFrameTimer = 0.f;
            windFrame = 1 - windFrame;
        }

        if (player)
        {
            // ===== CHANGED: chi cong don van toc day cho nhung luong gio
            // ma nhan vat DANG DUNG dung trong road cua no - nhan vat o
            // road khac hoan toan khong bi anh huong =====
            float py = player->getY();
            float pushV = 0.f;

            for (const auto& g : gusts)
            {
                if (py >= ROAD_TOP[g.roadIndex] && py <= ROAD_BOTTOM[g.roadIndex])
                    pushV += g.blowsRight ? WIND_PUSH_SPEED : -WIND_PUSH_SPEED;
            }

            player->ApplyWindPush(pushV);
        }

        if (phaseTimer >= ACTIVE_DURATION)
        {
            if (player)
                player->ApplyWindPush(0.f);

            phase = Phase::Idle;
            cooldownTimer = IDLE_COOLDOWN;
            gusts.clear();

            // ===== CHANGED: cat am qua AudioManager neu file dai hon ACTIVE_DURATION
            if (audio) audio->stopControlledSound(WIND_GUST_SOUND_NAME);
        }
        break;
    }
    }
}

//==================================================
// Draw
//==================================================

void WindGustManager::Draw(sf::RenderWindow& window) const
{
    if (!active)
        return;

    if (phase == Phase::Warning && signFrameWidth > 0)
    {
        // Nhap nhay dong bo cac icon canh bao. Voi MOI luong gio, chi ve
        // icon trong pham vi Y cua DUNG road ma no se thoi, o canh man
        // hinh phia gio sap thoi toi (gio thoi sang phai -> canh bao ben trai)
        constexpr float BLINK_INTERVAL = 0.15f;
        bool blinkOn = std::fmod(phaseTimer, BLINK_INTERVAL * 2.f) < BLINK_INTERVAL;
        sf::Uint8 alpha = blinkOn ? 255 : 60;

        sf::Sprite sp(signTexture);
        float s = SIGN_SIZE / (float)signFrameWidth;
        sp.setScale(s, s);
        sp.setOrigin(signFrameWidth / 2.f, signFrameHeight / 2.f);
        sp.setColor(sf::Color(255, 255, 255, alpha));

        for (const auto& g : gusts)
        {
            float roadTop = ROAD_TOP[g.roadIndex];
            float roadBottom = ROAD_BOTTOM[g.roadIndex];
            float roadHeight = roadBottom - roadTop;

            float signX = g.blowsRight ? SIGN_MARGIN_X : canvasW - SIGN_MARGIN_X;
            float spacing = roadHeight / (SIGN_PER_ROAD + 1);

            for (int i = 1; i <= SIGN_PER_ROAD; i++)
            {
                sp.setPosition(signX, roadTop + spacing * i);
                window.draw(sp);
            }
        }
    }
    else if (phase == Phase::Active && windWidth > 0)
    {
        const sf::Texture& tex = (windFrame == 0) ? windTexture1 : windTexture2;

        // Mo dan luc bat dau/ket thuc gust cho mem mai - dung chung cho
        // tat ca luong gio vi tat ca cung bat dau/ket thuc dong thoi
        float fadeIn = std::min(phaseTimer / 0.3f, 1.f);
        float fadeOut = std::min((ACTIVE_DURATION - phaseTimer) / 0.3f, 1.f);
        sf::Uint8 alpha = (sf::Uint8)(160 * std::min(fadeIn, fadeOut));

        for (const auto& g : gusts)
        {
            float roadTop = ROAD_TOP[g.roadIndex];
            float roadBottom = ROAD_BOTTOM[g.roadIndex];
            float roadHeight = roadBottom - roadTop;

            // ===== CHANGED: scale Y theo dung chieu cao cua road (thay vi
            // ca canvas), nen chi 1 sprite duy nhat la phu du 1 road, khong
            // can lap lai WIND_BAND_COUNT nhu truoc =====
            float scaleX = canvasW / (float)windWidth;
            float scaleY = roadHeight / (float)windHeight;

            sf::Sprite sp(tex);
            sp.setScale(g.blowsRight ? scaleX : -scaleX, scaleY);
            sp.setColor(sf::Color(255, 255, 255, alpha));
            sp.setPosition(g.blowsRight ? 0.f : canvasW, roadTop);
            window.draw(sp);
        }
    }
}