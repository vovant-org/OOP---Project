// TrainManager.cpp
#include "TrainManager.h"
#include "CPEOPLE.h"
#include "AudioManager.h"
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <algorithm>

namespace
{
    // ===== ADDED: ten dinh danh am thanh trong AudioManager =====
    const std::string TRAIN_SIGN_SOUND_NAME = "train_sign";
    const std::string TRAIN_RUN_SOUND_NAME = "train_run";

    constexpr float WARNING_DURATION = 2.f;
    constexpr float RUNNING_DURATION = 8.f;
    constexpr float COOLDOWN_DURATION = 8.f;

    // Chieu cao vung anh huong cua 1 road (rail + tau + vung chan
    // vehicles/animals), canh giua tai RoadInfo::centerY
    constexpr float ROAD_BAND_HEIGHT = 110.f;

    // So toa than (frame2) o giua 2 dau tau (frame1, dau cuoi lat ngang)
    constexpr int TRAIN_BODY_CAR_COUNT = 9;

    constexpr float SIGN_SIZE = 100.f;    // px hien thi TrainSign
    constexpr float SIGN_MARGIN = 30.f;  // le tu canh road toi TrainSign

    constexpr float BLINK_INTERVAL = 0.1f;   // giay moi lan doi sang/toi

    // ===== ADDED: thu nho hitbox va cham cua tau lai so voi kich thuoc
    // HIEN THI that (trainLength x ROAD_BAND_HEIGHT), tranh cam giac bi
    // tinh la trung dan chi vi cham nhe vien ngoai/phan trang tri cua
    // sprite. 1.f = giu nguyen kich thuoc that, 0.5f = con nua kich thuoc
    // (cang nho cang "khoan dung", de nguoi choi lach qua hon).
    // MUON TU CHINH: sua truc tiep con so nay.
    constexpr float TRAIN_HITBOX_SCALE = 0.6f;
}

//==================================================
// Constructor
//==================================================

TrainManager::TrainManager()
{
    cooldownTimer = COOLDOWN_DURATION;
}

//==================================================
// ===== ADDED: tinh bounding box vung pixel khong trong suot, dung de
// cat bo phan canvas thua (trong suot) quanh hinh ve trong texture, giup
// xep cac toa tau sat nhau thay vi bi hut khoang trong =====
//==================================================

sf::IntRect TrainManager::ComputeOpaqueBounds(const sf::Texture& tex)
{
    sf::Image img = tex.copyToImage();
    sf::Vector2u size = img.getSize();

    if (size.x == 0 || size.y == 0)
        return sf::IntRect(0, 0, 0, 0);

    int minX = (int)size.x, minY = (int)size.y;
    int maxX = -1, maxY = -1;

    constexpr sf::Uint8 ALPHA_THRESHOLD = 8; // bo qua pixel gan nhu trong suot (anti-alias mo)

    for (unsigned y = 0; y < size.y; y++)
    {
        for (unsigned x = 0; x < size.x; x++)
        {
            if (img.getPixel(x, y).a > ALPHA_THRESHOLD)
            {
                if ((int)x < minX) minX = (int)x;
                if ((int)x > maxX) maxX = (int)x;
                if ((int)y < minY) minY = (int)y;
                if ((int)y > maxY) maxY = (int)y;
            }
        }
    }

    // Toan bo texture trong suot -> fallback dung nguyen ca texture
    if (maxX < 0 || maxY < 0)
        return sf::IntRect(0, 0, (int)size.x, (int)size.y);

    return sf::IntRect(minX, minY, maxX - minX + 1, maxY - minY + 1);
}

//==================================================
// Setup
//==================================================

bool TrainManager::LoadTextures(const std::string& headPath, const std::string& bodyPath,
    const std::string& railPath, const std::string& signPath)
{
    bool ok = true;

    if (!headTexture.loadFromFile(headPath))
    {
        std::cout << "[TrainManager] Cannot load: " << headPath << "\n";
        ok = false;
    }
    else
    {
        headTexture.setSmooth(true);
        headTexW = headTexture.getSize().x;
        headTexH = headTexture.getSize().y;
        // ===== ADDED: cat bo vien trong suot thua quanh hinh ve
        headContentRect = ComputeOpaqueBounds(headTexture);
    }

    if (!bodyTexture.loadFromFile(bodyPath))
    {
        std::cout << "[TrainManager] Cannot load: " << bodyPath << "\n";
        ok = false;
    }
    else
    {
        bodyTexture.setSmooth(true);
        bodyTexW = bodyTexture.getSize().x;
        bodyTexH = bodyTexture.getSize().y;
        // ===== ADDED: cat bo vien trong suot thua quanh hinh ve
        bodyContentRect = ComputeOpaqueBounds(bodyTexture);
    }

    if (!railTexture.loadFromFile(railPath))
    {
        std::cout << "[TrainManager] Cannot load: " << railPath << "\n";
        ok = false;
    }
    else
    {
        railTexture.setSmooth(true);
        railTexW = railTexture.getSize().x;
        railTexH = railTexture.getSize().y;
    }

    if (!signTexture.loadFromFile(signPath))
    {
        std::cout << "[TrainManager] Cannot load: " << signPath << "\n";
        ok = false;
    }
    else
    {
        signTexture.setSmooth(true);
        signTexW = signTexture.getSize().x;
        signTexH = signTexture.getSize().y;
    }

    return ok;
}

// ===== ADDED: gan AudioManager dung chung =====
void TrainManager::SetAudioManager(AudioManager* manager)
{
    audio = manager;
}

// ===== CHANGED: nap 2 am thanh vao AudioManager (yeu cau SetAudioManager()
// da duoc goi truoc) thay vi tu giu buffer rieng =====
bool TrainManager::LoadSounds(const std::string& signSoundPath, const std::string& trainSoundPath)
{
    if (!audio)
    {
        std::cout << "[TrainManager] Chua gan AudioManager, khong the nap am thanh\n";
        return false;
    }

    bool ok = true;

    if (!audio->loadSound(TRAIN_SIGN_SOUND_NAME, signSoundPath))
        ok = false;

    if (!audio->loadSound(TRAIN_RUN_SOUND_NAME, trainSoundPath))
        ok = false;

    return ok;
}

void TrainManager::SetActive(bool isActive)
{
    active = isActive;
}

void TrainManager::SetRoadCenters(const float centers[ROAD_COUNT])
{
    for (int i = 0; i < ROAD_COUNT; i++)
        roads[i].centerY = centers[i];
}

void TrainManager::Reset()
{
    phase = Phase::Idle;
    phaseTimer = 0.f;
    cooldownTimer = COOLDOWN_DURATION;

    activeRoad = -1;
    fromLeft = true;

    trainX = 0.f;
    trainSpeed = 0.f;
    trainLength = 0.f;
    runningDuration = 0.f;   // ===== ADDED

    trainHasHitPlayer = false;
    pendingPlayerHit = false;

    // ===== CHANGED: cat ca 2 am tau qua AudioManager luc Reset() - truoc
    // day chi stop TRAIN_RUN_SOUND_NAME, con TRAIN_SIGN_SOUND_NAME (coi
    // bao) van la fire-and-forget nen doi map/choi lai giua chung Warning
    // co the con nghe sot lai tieng coi cu =====
    if (audio) audio->stopControlledSound(TRAIN_RUN_SOUND_NAME);
    if (audio) audio->stopControlledSound(TRAIN_SIGN_SOUND_NAME);

    // Luu y: railHeight, railYOffset va trainSpeedMultiplier KHONG bi
    // reset o day, vi day la tuy chinh cua nguoi choi/nguoi lap trinh
    // (persist qua cac lan Init() lai map), khong phai trang thai luot
    // chay hien tai.
}

//==================================================
// Chon road + huong ngau nhien cho luot canh bao nay
//==================================================

void TrainManager::PickRoadAndSide()
{
    activeRoad = rand() % ROAD_COUNT;
    fromLeft = (rand() % 2 == 0);
}

void TrainManager::StartWarning()
{
    PickRoadAndSide();
    phase = Phase::Warning;
    phaseTimer = 0.f;
    trainHasHitPlayer = false;

    // ===== CHANGED: doi tu playSound() (fire-and-forget, phat het do dai
    // file bat ke WARNING_DURATION la bao nhieu) sang playControlledSound()
    // de co the chu dong stopControlledSound() dung luc TrainSign bien mat
    // (StartRunning()) - truoc day file dai hon WARNING_DURATION se bi
    // "keo dai them" sang ca luc tau da chay =====
    if (audio) audio->playControlledSound(TRAIN_SIGN_SOUND_NAME, false);
}

void TrainManager::StartRunning()
{
    phase = Phase::Running;
    phaseTimer = 0.f;

    // ===== ADDED: TrainSign (va tieng coi cua no) bien mat dung luc nay -
    // cat tieng coi truoc khi phat tieng tau chay, tranh ca 2 chong len nhau
    if (audio) audio->stopControlledSound(TRAIN_SIGN_SOUND_NAME);

    // ===== CHANGED: dung chieu cao/rong cua CONTENT (da cat bo trong
    // suot thua) thay vi ca texture, de headW/bodyW phan anh dung kich
    // thuoc hinh ve that su -> cac toa xep sat nhau, khong con khoang ho
    float headContentH = (headContentRect.height > 0) ? (float)headContentRect.height : (float)headTexH;
    float bodyContentW = (bodyContentRect.width > 0) ? (float)bodyContentRect.width : (float)bodyTexW;
    float headContentW = (headContentRect.width > 0) ? (float)headContentRect.width : (float)headTexW;

    // gia dinh headTexH == bodyTexH (2 frame ModernTrain cung chieu cao)
    float scale = (headContentH > 0.f) ? ROAD_BAND_HEIGHT / headContentH : 1.f;
    float headW = headContentW * scale;
    float bodyW = bodyContentW * scale;

    trainLength = headW * 2.f + bodyW * TRAIN_BODY_CAR_COUNT;

    // ===== ADDED: toc do co ban (chua nhan he so), luon duong
    float baseSpeed = (canvasW + trainLength) / RUNNING_DURATION;
    float mult = (trainSpeedMultiplier > 0.f) ? trainSpeedMultiplier : 1.f;

    if (fromLeft)
    {
        // Bat dau an hoan toan ben trai man hinh, chay sang phai cho toi
        // khi thoat het ben phai
        trainX = -trainLength;
        trainSpeed = baseSpeed * mult;
    }
    else
    {
        trainX = canvasW;
        trainSpeed = -baseSpeed * mult;
    }

    // ===== ADDED: thoi luong Running THUC TE, dam bao tau di het man
    // hinh dung luc phaseTimer cham nguong nay, bat ke toc do la bao nhieu
    runningDuration = (canvasW + trainLength) / std::fabs(trainSpeed);

    trainHasHitPlayer = false;

    // ===== CHANGED: qua AudioManager (playControlledSound, loop=true) -
    // dung 1 instance duy nhat cho "train_run", stopControlledSound() se
    // cat dung instance nay khi Running ket thuc =====
    if (audio) audio->playControlledSound(TRAIN_RUN_SOUND_NAME, true);
}

//==================================================
// Update
//==================================================

void TrainManager::Update(float dt, CPEOPLE* player)
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
            StartRunning();
        break;
    }

    case Phase::Running:
    {
        phaseTimer += dt;
        trainX += trainSpeed * dt;

        if (!trainHasHitPlayer && player && player->IsAlive() && activeRoad >= 0)
        {
            float top = roads[activeRoad].centerY - ROAD_BAND_HEIGHT / 2.f;

            // ===== CHANGED: hitbox thu nho lai (TRAIN_HITBOX_SCALE), can
            // giua trong vung hien thi that cua tau, thay vi dung nguyen
            // trainLength x ROAD_BAND_HEIGHT (kich thuoc hien thi day du) =====
            float hitW = trainLength * TRAIN_HITBOX_SCALE;
            float hitH = ROAD_BAND_HEIGHT * TRAIN_HITBOX_SCALE;
            float hitX = trainX + (trainLength - hitW) / 2.f;
            float hitY = top + (ROAD_BAND_HEIGHT - hitH) / 2.f;

            sf::FloatRect trainBox(hitX, hitY, hitW, hitH);

            if (trainBox.intersects(player->GetBoundingBox()))
            {
                trainHasHitPlayer = true;
                pendingPlayerHit = true;
            }
        }

        // ===== CHANGED: dung runningDuration (tinh dong theo toc do thuc
        // te trong StartRunning/ApplySpeedMultiplierNow) thay vi hang so
        // RUNNING_DURATION, de tau luon vua thoat man hinh dung luc nay
        if (phaseTimer >= runningDuration)
        {
            phase = Phase::Idle;
            cooldownTimer = COOLDOWN_DURATION;
            activeRoad = -1;

            // ===== CHANGED: cat am tau qua AudioManager
            if (audio) audio->stopControlledSound(TRAIN_RUN_SOUND_NAME);
        }
        break;
    }
    }
}

//==================================================
// Truy van / tuong tac tu CGAME
//==================================================

bool TrainManager::IsRoadActive(float y) const
{
    if (!active || activeRoad < 0)
        return false;

    if (phase != Phase::Warning && phase != Phase::Running)
        return false;

    return std::fabs(y - roads[activeRoad].centerY) <= ROAD_BAND_HEIGHT / 2.f;
}

bool TrainManager::IsRoadRunning(float y) const
{
    if (!active || activeRoad < 0 || phase != Phase::Running)
        return false;

    return std::fabs(y - roads[activeRoad].centerY) <= ROAD_BAND_HEIGHT / 2.f;
}

bool TrainManager::TryBlockEntry(CPEOPLE* player, float currentY, float nextY) const
{
    // Chi chan khi dang O NGOAI road (currentY khong nam trong road dang
    // Running) MA buoc di chuyen tiep theo lai ROI VAO road do - tuc la
    // "co tinh chay vao" theo dung yeu cau
    if (!IsRoadRunning(nextY) || IsRoadRunning(currentY))
        return false;

    if (!player)
        return false;

    // Dang di len (nextY < currentY) -> day nguoc xuong duoi, va nguoc lai
    bool pushDown = (nextY < currentY);
    player->TriggerPushback(pushDown);

    return true;
}

bool TrainManager::ConsumePlayerHit()
{
    if (!pendingPlayerHit)
        return false;

    pendingPlayerHit = false;
    return true;
}

//==================================================
// ===== ADDED: dieu chinh toc do tau =====
//==================================================

void TrainManager::SetTrainSpeedMultiplier(float multiplier)
{
    if (multiplier > 0.f)
        trainSpeedMultiplier = multiplier;
}

void TrainManager::ApplySpeedMultiplierNow()
{
    if (phase != Phase::Running || trainSpeed == 0.f)
        return;

    // Quang duong con lai tau phai di de thoat het man hinh, tinh theo
    // toc do CU (truoc khi doi he so)
    float remainingDistance = (canvasW + trainLength) - std::fabs(trainX - (fromLeft ? -trainLength : canvasW));

    // Toc do co ban (khong dau) tu cong thuc goc, roi nhan he so MOI
    float baseSpeed = (canvasW + trainLength) / RUNNING_DURATION;
    float mult = (trainSpeedMultiplier > 0.f) ? trainSpeedMultiplier : 1.f;
    float newSpeedAbs = baseSpeed * mult;

    trainSpeed = fromLeft ? newSpeedAbs : -newSpeedAbs;

    // Tinh lai moc thoi gian ket thuc Running (runningDuration) sao cho
    // voi toc do MOI, tau van di dung quang duong con lai ke tu bay gio
    if (newSpeedAbs > 0.f)
        runningDuration = phaseTimer + (remainingDistance / newSpeedAbs);
}

//==================================================
// Draw
//==================================================

void TrainManager::Draw(sf::RenderWindow& window) const
{
    if (!active || activeRoad < 0)
        return;

    if (phase != Phase::Warning && phase != Phase::Running)
        return;

    float centerY = roads[activeRoad].centerY;
    float top = centerY - ROAD_BAND_HEIGHT / 2.f;

    //--------------------------------------------
    // Rail: hien dan tu 1 canh trong Warning, giu FULL trong Running
    // ===== CHANGED: chieu cao Rail lay tu railHeight (chinh duoc qua
    // SetRailHeight()) thay vi luon = ROAD_BAND_HEIGHT, va CAN GIUA theo
    // centerY cua road (khong dinh theo "top" cua vung va cham nua) =====
    //--------------------------------------------
    if (railTexW > 0)
    {
        float revealFraction = (phase == Phase::Warning)
            ? std::min(phaseTimer / WARNING_DURATION, 1.f)
            : 1.f;

        int visiblePixW = (int)(railTexW * revealFraction);

        if (visiblePixW > 0)
        {
            float scaleX = canvasW / (float)railTexW;
            float scaleY = railHeight / (float)railTexH;
            float railTop = centerY - railHeight / 2.f + railYOffset;

            sf::Sprite sp(railTexture);
            sp.setScale(scaleX, scaleY);

            if (fromLeft)
            {
                sp.setTextureRect(sf::IntRect(0, 0, visiblePixW, railTexH));
                sp.setPosition(0.f, railTop);
            }
            else
            {
                sp.setTextureRect(sf::IntRect(railTexW - visiblePixW, 0, visiblePixW, railTexH));
                sp.setPosition(canvasW - visiblePixW * scaleX, railTop);
            }

            window.draw(sp);
        }
    }

    //--------------------------------------------
    // TrainSign: nhap nhay, chi trong Warning, dat o canh vua chon
    //--------------------------------------------
    if (phase == Phase::Warning && signTexW > 0)
    {
        bool blinkOn = std::fmod(phaseTimer, BLINK_INTERVAL * 2.f) < BLINK_INTERVAL;

        if (blinkOn)
        {
            sf::Sprite sp(signTexture);
            float s = SIGN_SIZE / (float)signTexW;
            sp.setScale(s, s);
            sp.setOrigin(signTexW / 2.f, signTexH / 2.f);

            float signX = fromLeft ? SIGN_MARGIN : (canvasW - SIGN_MARGIN);
            sp.setPosition(signX, centerY);
            window.draw(sp);
        }
    }

    //--------------------------------------------
    // Tau: chi trong Running - [dau tau][than x N][dau tau lat ngang]
    // ===== CHANGED: dung headContentRect/bodyContentRect (da cat bo
    // khoang trong suot thua quanh hinh ve trong 2 file PNG) thay vi ca
    // texture, va can giua theo chieu doc trong ROAD_BAND_HEIGHT - day la
    // ly do cac toa bi hut xa nhau truoc day (texture co le trong lon
    // hon hinh ve that su) =====
    //--------------------------------------------
    if (phase == Phase::Running && headTexW > 0 && bodyTexW > 0)
    {
        sf::IntRect headRect = (headContentRect.width > 0 && headContentRect.height > 0)
            ? headContentRect : sf::IntRect(0, 0, headTexW, headTexH);
        sf::IntRect bodyRect = (bodyContentRect.width > 0 && bodyContentRect.height > 0)
            ? bodyContentRect : sf::IntRect(0, 0, bodyTexW, bodyTexH);

        float scale = (headRect.height > 0) ? ROAD_BAND_HEIGHT / (float)headRect.height : 1.f;
        float headW = headRect.width * scale;
        float bodyW = bodyRect.width * scale;
        float bodyH = bodyRect.height * scale;

        // Can giua theo chieu doc rieng cho toa than, phong khi
        // bodyRect.height khac headRect.height (vd toa than thap/cao hon dau tau)
        float bodyTop = top + (ROAD_BAND_HEIGHT - bodyH) / 2.f;

        float cursorX = trainX;

        // Dau tau ben trai doan (frame1, giu nguyen huong)
        sf::Sprite headSp(headTexture);
        headSp.setTextureRect(headRect);
        headSp.setScale(scale, scale);
        headSp.setPosition(cursorX, top);
        window.draw(headSp);
        cursorX += headW;

        // Cac toa than (frame2) noi tiep nhau, dinh sat vao nhau (khong
        // con khoang trong thua giua cac toa)
        sf::Sprite bodySp(bodyTexture);
        bodySp.setTextureRect(bodyRect);
        bodySp.setScale(scale, scale);
        for (int i = 0; i < TRAIN_BODY_CAR_COUNT; i++)
        {
            bodySp.setPosition(cursorX, bodyTop);
            window.draw(bodySp);
            cursorX += bodyW;
        }

        // Dau tau ben phai doan (frame1 LAT NGANG de mui quay ra ngoai)
        sf::Sprite tailSp(headTexture);
        tailSp.setTextureRect(headRect);
        tailSp.setScale(-scale, scale);
        tailSp.setPosition(cursorX + headW, top);
        window.draw(tailSp);
    }
}