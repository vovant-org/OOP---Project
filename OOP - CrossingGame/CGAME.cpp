// CGAME.cpp
#include "CGAME.h"
#include <iostream>
#include <fstream>   // ===== ADDED (Bước 6) =====
#include <sstream>   // ===== ADDED: build chuoi HUD =====
#include <cmath>     // ===== ADDED: std::ceil cho dong ho HUD =====
#include <filesystem> // ===== ADDED (Giai doan 1 - Continue Menu redesign): quet/xoa file save dong =====
#include <chrono>     // ===== ADDED: chuyen doi thoi gian sua doi file =====
#include <algorithm>  // ===== ADDED: std::sort ListAllSaves() =====

// ===== ADDED (Bước 3): các lớp obstacle cụ thể theo map =====
#include "CBIKE.h"
#include "CMOTOR.h"
#include "CTRUCK.h"
#include "CHELLMOTOR.h"
#include "CTRAIN.h"
#include "CBEAST.h"
#include "CCROCODILE.h"
#include "CDINASOUR.h"   // khai báo class CDINOSAUR (tên file khác tên class)
#include "COLDBIRD.h"
#include "CBRUTE.h"
#include "CPIGLIN.h"
#include "CANGLE.h"
#include "CBIRD.h"
#include "CCLOUD.h"
#include "TrafficLight.h"   // ===== ADDED (Bước 5) =====
#include "AudioManager.h"   // ===== ADDED: doi nhac nen theo map =====

namespace
{
    // Canvas logic cua man hinh gameplay - khop voi WIN_W/WIN_H
    // (1280x720) dang dung cho MainMenu/CharacterSelection/MapSelection
    // trong main.cpp
    constexpr float CANVAS_W = 1280.f;
    constexpr float CANVAS_H = 720.f;

    // ===== ADDED: cau hinh rieng cho HUD dong ho dem nguoc (Time) - chinh
    // 2 gia tri nay de doi size chu / khoang cach tu mep tren, KHONG can
    // tu tay chinh toa do X vi Draw() se tu can giua theo CANVAS_W =====
    constexpr unsigned int HUD_TIME_FONT_SIZE = 35;  // size chu (so voi 22 cua Score/Level/HP)
    constexpr float HUD_TIME_MARGIN_TOP = 14.f;      // khoang cach tu mep tren man hinh

    // Duong dan map, dung thu tu index giong MapSelection
    // (0=City 1=Ancient 2=Hell 3=Sky)
    const std::string MAP_PATHS[4] =
    {
        "Map/City_map.png",
        "Map/Ancient_map.png",
        "Map/Hell_map.png",
        "Map/Sky_map.png"
    };

    // ===== CHANGED: moi map GIO co 3 file save rieng theo difficultyMode
    // (0=Easy 1=Hard 2=Nightmare), khong con ghi de len nhau giua cac
    // mode nua. SAVE_PATHS[map][mode] =====
    const std::string SAVE_PATHS[4][3] =
    {
        { "Save/city_easy.sav",     "Save/city_hard.sav",     "Save/city_nightmare.sav" },
        { "Save/ancient_easy.sav",  "Save/ancient_hard.sav",  "Save/ancient_nightmare.sav" },
        { "Save/hell_easy.sav",     "Save/hell_hard.sav",     "Save/hell_nightmare.sav" },
        { "Save/sky_easy.sav",      "Save/sky_hard.sav",      "Save/sky_nightmare.sav" }
    };

    // New: 4 save slots (one per slot index 0..3)
    const std::string SAVE_SLOT_PATHS[4] =
    {
        "Save/save1.sav",
        "Save/save2.sav",
        "Save/save3.sav",
        "Save/save4.sav"
    };

    // ===== ADDED (Giai doan 1 - Continue Menu redesign): thu muc chua
    // TAT CA file save, dung cho ListAllSaves()/DeleteSave() quet dong
    // thay vi gioi han 4 slot co dinh nhu SAVE_SLOT_PATHS o tren =====
    const std::string SAVE_DIR = "Save";

    // Chuyen std::filesystem::file_time_type -> time_t (epoch giay).
    // Tach rieng thanh ham de PeekSaveData() khong bi vo neu clock_cast
    // nem ngoai le tren mot so trien khai STL cu.
    long long FileTimeToUnix(const std::filesystem::file_time_type& ftime)
    {
        try
        {
            auto sctp = std::chrono::clock_cast<std::chrono::system_clock>(ftime);
            return static_cast<long long>(std::chrono::system_clock::to_time_t(sctp));
        }
        catch (...)
        {
            return 0;
        }
    }

    // ===== ADDED: nhac nen rieng cho tung map, phat thay cho nhac nen
    // MainMenu ngay khi Init() (vao Playing) =====
    const std::string MAP_MUSIC_PATHS[4] =
    {
        "Sound/Citymap_backgroundmusic.mp3",
        "Sound/Ancientmap_backgroundmusic.mp3",
        "Sound/Hellmap_backgroundmusic.mp3",
        "Sound/Skymap_backgroundmusic.mp3"
    };

    // Duong dan nhan vat, dung thu tu index giong CharacterSelection
    // (0=Chicken 1=Knight 2=Dog 3=Luffy)
    const std::string CHARACTER_PATHS[4] =
    {
        "Character/Chicken_character.png",
        "Character/Knight_character.png",
        "Character/Dog_character.png",
        "Character/Luffy_character.png"
    };

    // HP toi da theo nhan vat - khop CharacterSelection::charInfos
    // (Chicken 3, Knight 5, Dog 2, Luffy 4)
    const int CHARACTER_MAX_HP[4] = { 3, 5, 2, 4 };

    // Cooldown (giay) giua 2 lan di chuyen - suy tu chi so Speed trong
    // CharacterSelection (Chicken 4, Knight 2, Dog 5, Luffy 3): speed
    // cang cao thi cooldown cang ngan (di lien tuc duoc)
    const float CHARACTER_MOVE_COOLDOWN[4] = { 0.15f, 0.25f, 0.10f, 0.20f };

    // ===== ADDED: gioi han thoi gian (giay) theo difficultyMode
    // (0=Easy 1=Hard 2=Nightmare). -1 = khong gioi han =====
    const float TIME_LIMIT_BY_MODE[3] = { -1.f, 90.f, 120.f };

    // ===== ADDED: so level can hoan thanh de Win, theo difficultyMode =====
    const int WIN_LEVEL_BY_MODE[3] = { 3, 5, 6 };

    // ===== CHANGED (fix lane sai + đè vào hàng spawn): toa do Y gio la
    // TAM obstacle (vi origin da chuan hoa ve giua sprite), va lane cuoi
    // cung duoc keo len xa hon hang spawn cua nguoi choi (y = CANVAS_H-40 = 680)
    // de chua du khong gian cho sprite cao (train, brute...) =====
    const float CITY_LANE_Y[] = { 125.f, 170.f, 255.f, 300.f, 385.f, 420.f, 510.f, 540.f };
    const float ANCIENT_LANE_Y[] = { 160.f, 280.f, 420.f, 550.f };
    const float HELL_LANE_Y[] = { 105.f, 137.f, 220.f, 270.f, 360.f, 400.f, 510.f, 550.f };
    const float SKY_LANE_Y[] = { 125.f, 180.f, 255.f, 310.f, 393.f, 433.f, 520.f, 570.f };

    // ===== ADDED (Bước 5): texture den giao thong rieng theo tung map =====
    const std::string LIGHT_PATHS[4] =
    {
        "Obstacles/City_light.png",
        "Obstacles/Ancient_light.png",
        "Obstacles/Hell_light.png",
        "Obstacles/Sky_light.png"
    };

    // ===== ADDED: texture cho co che canh bao + da lan (rieng Ancient +
    // Nightmare - xem CGAME::Init) =====
    const std::string ROCK_TEXTURE_PATH = "Obstacles/RollingRock.png";
    const std::string SIGN_TEXTURE_PATH = "ui/Icon/RollingSign.png";

    // ===== ADDED: texture cho co che thien thach (Hell + Nightmare) =====
    const std::string METEORITE_TEXTURE_PATH = "Obstacles/Meteorite.png";
    const std::string METEORITE_SIGN_TEXTURE_PATH = "ui/Icon/MeteoriteSign.png";

    // ===== ADDED: texture cho co che gio giat (Sky + Nightmare) =====
    const std::string WIND_TEXTURE_PATH_1 = "Obstacles/Wind_frame1.png";
    const std::string WIND_TEXTURE_PATH_2 = "Obstacles/Wind_frame2.png";
    const std::string WIND_SIGN_TEXTURE_PATH = "ui/Icon/WindSign.png";

    // ===== ADDED: texture cho co che tau hoa (City + Nightmare) =====
    const std::string TRAIN_HEAD_TEXTURE_PATH = "Obstacles/ModernTrain_frame1.png";
    const std::string TRAIN_BODY_TEXTURE_PATH = "Obstacles/ModernTrain_frame2.png";
    const std::string TRAIN_RAIL_TEXTURE_PATH = "Obstacles/Rail.png";
    const std::string TRAIN_SIGN_TEXTURE_PATH = "ui/Icon/TrainSign.png";

    // ===== ADDED: am thanh rieng cho tung hazard Nightmare, 1 file/map
    // (RollingRock=Ancient, Meteorite=Hell, Wind=Sky, Train=City - 2 file
    // vi tau co coi bao rieng + tieng chay rieng) =====
    const std::string ROCK_SOUND_PATH = "Sound/RollingRockSound.mp3";
    const std::string METEORITE_SOUND_PATH = "Sound/MeteoriteSound.mp3";
    const std::string WIND_SOUND_PATH = "Sound/WindGustSound.mp3";
    const std::string TRAIN_SIGN_SOUND_PATH = "Sound/TrainSignSound.mp3";
    const std::string TRAIN_SOUND_PATH = "Sound/TrainSound.mp3";
}

//==================================================
// Constructor / Destructor
//==================================================

CGAME::CGAME(sf::RenderWindow& window)
    : mWindow(window),
    currentMap(0),
    characterIndex(0),
    difficultyMode(1), // Hard mac dinh
    player(nullptr),
    level(1),
    score(0),
    playerHP(0),
    playerMaxHP(0),
    isGameOver(false),
    isPaused(false),
    isWin(false),
    winTargetLevel(0),   // se duoc Init() gan lai ngay khi vao game
    timeRemaining(-1.f),
    savePath("save.dat"),
    playerName("")
{
}

CGAME::~CGAME()
{
    delete player;
    player = nullptr;

    for (auto* v : vehicles)
        delete v;
    vehicles.clear();

    for (auto* a : animals)
        delete a;
    animals.clear();

    for (auto* l : lights)
        delete l;
    lights.clear();
}

//==================================================
// Init — Bước 1: load map + đặt player, chưa spawn
// vehicles/animals/lights (để Bước 3)
//==================================================

void CGAME::Init(int mapIndex, int characterIndex)
{
    //----------------------------------
    // Map
    //----------------------------------

    if (mapIndex < 0 || mapIndex > 3)
        mapIndex = 0;

    currentMap = mapIndex;

    // ===== CHANGED: luu vao file rieng theo CA map LAN difficultyMode
    // (difficultyMode phai duoc SetDifficultyMode()/LoadGame() gan truoc
    // khi goi Init(), xem main.cpp va LoadGame() ben duoi) =====
    {
        int m = difficultyMode;
        if (m < 0 || m > 2) m = 1;
        savePath = SAVE_PATHS[currentMap][m];
    }

    // ===== ADDED: doi nhac nen sang bai rieng cua map nay, thay cho
    // nhac nen MainMenu =====
    if (audio)
    {
        audio->loadMusic(MAP_MUSIC_PATHS[currentMap]);
        audio->playMusic(true);
    }

    if (!mapTexture.loadFromFile(MAP_PATHS[currentMap]))
    {
        std::cout << "[CGAME] Cannot load map: "
            << MAP_PATHS[currentMap] << "\n";
    }

    mapSprite.setTexture(mapTexture);

    float scaleX = CANVAS_W / mapTexture.getSize().x;
    float scaleY = CANVAS_H / mapTexture.getSize().y;
    mapSprite.setScale(scaleX, scaleY);
    mapSprite.setPosition(0.f, 0.f);

    //----------------------------------
    // Player
    //----------------------------------

    if (characterIndex < 0 || characterIndex > 3)
        characterIndex = 0;

    this->characterIndex = characterIndex;   // ===== ADDED (Bước 6) =====

    delete player; // phong khi Init duoc goi lai (choi lai / doi map)

    float startX = CANVAS_W / 2.f;
    float startY = CANVAS_H - 40.f; // gan mep duoi man hinh

    player = new CPEOPLE(startX, startY);
    player->SetMoveCooldown(CHARACTER_MOVE_COOLDOWN[characterIndex]);

    if (!player->loadTexture(CHARACTER_PATHS[characterIndex]))
    {
        std::cout << "[CGAME] Cannot load character: "
            << CHARACTER_PATHS[characterIndex] << "\n";
    }

    playerMaxHP = CHARACTER_MAX_HP[characterIndex];
    playerHP = playerMaxHP;

    //----------------------------------
    // Game state
    //----------------------------------

    level = 1;
    score = 0;
    isGameOver = false;
    isPaused = false;
    isWin = false;   // ===== ADDED =====

    // ===== CHANGED: gia tri mac dinh (Adventure - bat dau tu level 1).
    // Nightmare - Custom se duoc SetStartingLevel()/ReapplyCustomNightmare()
    // GHI DE lai gia tri nay SAU Init(), xem 2 ham do =====
    winTargetLevel = WIN_LEVEL_BY_MODE[difficultyMode];

    // ===== ADDED: gioi han thoi gian theo do kho (Easy = khong gioi han) =====
    timeRemaining = TIME_LIMIT_BY_MODE[difficultyMode];

    //----------------------------------
    // ===== ADDED (Bước 3): spawn vehicles/animals theo currentMap =====
    //----------------------------------

    for (auto* v : vehicles)
        delete v;
    vehicles.clear();

    for (auto* a : animals)
        delete a;
    animals.clear();

    // ===== ADDED (Bước 5) =====
    for (auto* l : lights)
        delete l;
    lights.clear();

    // ===== ADDED: helper ap dung do kho (Easy/Hard/Nightmare) dong nhat
    // cho MOI obstacle duoc spawn ben duoi - chi can wrap push_back bang
    // addVehicle()/addAnimal() thay vi goi truc tiep =====
    float speedDelta = 0.f;
    if (difficultyMode == 0)       speedDelta = -50.f;  // Easy
    else if (difficultyMode == 2)  speedDelta = 150.f;   // Nightmare

    auto addVehicle = [&](CVEHICLE* v, TrafficLight* light)
        {
            v->AdjustSpeed(speedDelta);
            vehicles.push_back(v);
            vehicles.back()->SetTrafficLight(light);
        };
    auto addAnimal = [&](CANIMAL* a, TrafficLight* light)
        {
            a->AdjustSpeed(speedDelta);
            animals.push_back(a);
            animals.back()->SetTrafficLight(light);
        };

    switch (currentMap)
    {
    case 0: // City — chỉ có xe cộ
    {
        const float* y = CITY_LANE_Y;

        // Map co 4 road (moi road = 2 lane), giua cac road la 1 vach an
        // toan cho character dung (tong 3 vach). Moi vach co 1 den giao
        // thong dieu khien CA ROAD (2 lane) NGAY DUOI no. Road0 (gan vach
        // dich, tren cung) khong co vach phia truoc nen luon chay tu do.
        float medianY[3] = { 120.f, 245.f, 372.f };
        float medianRedDur[3] = { 2.f, 3.f, 6.f };
        float medianGreenDur[3] = { 15.f, 10.f, 5.f };

        TrafficLight* roadLight[4] = { nullptr, nullptr, nullptr, nullptr };
        for (int i = 0; i < 3; ++i)
        {
            // ===== ADDED (Nightmare): chi giu DUY NHAT den giua (i==1),
            // xoa 2 den con lai (i==0 va i==2) =====
            if (difficultyMode == 2 && i != 1)
                continue;

            TrafficLight* tl = new TrafficLight(10.f, medianY[i], medianRedDur[i], medianGreenDur[i]);
            tl->loadTexture(LIGHT_PATHS[0]);
            lights.push_back(tl);
            roadLight[i + 1] = tl;
        }

        // Road 0 (y[0], y[1]) - khong den, luon chay
        addVehicle(new CMOTOR(2000.f, y[0] + 2.f, 330.f, false), roadLight[0]);
        addVehicle(new CMOTOR(2000.f, y[0], 450.f, false), roadLight[0]);
        if (difficultyMode != 0)
        {
            addVehicle(new CBIKE(1100.f, y[1], 250.f, true), roadLight[0]);
            addVehicle(new CTRUCK(1000.f, y[1], 380.f, true), roadLight[0]);
        }
        if (difficultyMode == 2)
        {
            addVehicle(new CBIKE(600.f, y[0], 200.f, true), roadLight[0]);
            addVehicle(new CTRUCK(1600.f, y[1], 350.f, false), roadLight[0]);
        }

        // Road 1 (y[2], y[3]) - dieu khien boi den o vach 1
        addVehicle(new CMOTOR(300.f, y[2], 390.f, true), roadLight[1]);
        addVehicle(new CTRUCK(1000.f, y[2], 280.f, true), roadLight[1]);
        if (difficultyMode != 0)
        {
            addVehicle(new CMOTOR(300.f, y[3], 390.f, false), roadLight[1]);
            addVehicle(new CTRUCK(1000.f, y[3], 270.f, false), roadLight[1]);
        }
        if (difficultyMode == 2)
        {
            addVehicle(new CMOTOR(700.f, y[2], 400.f, false), roadLight[1]);
            addVehicle(new CBIKE(1500.f, y[3], 210.f, true), roadLight[1]);
        }

        // Road 2 (y[4], y[5]) - dieu khien boi den o vach 2
        addVehicle(new CBIKE(150.f, y[4], 200.f, false), roadLight[2]);
        addVehicle(new CMOTOR(800.f, y[4], 390.f, false), roadLight[2]);
        if (difficultyMode != 0)
        {
            addVehicle(new CBIKE(150.f, y[5], 200.f, true), roadLight[2]);
            addVehicle(new CMOTOR(800.f, y[5], 390.f, true), roadLight[2]);
        }
        if (difficultyMode == 2)
        {
            addVehicle(new CTRUCK(1400.f, y[4], 360.f, true), roadLight[2]);
            addVehicle(new CBIKE(500.f, y[5], 260.f, false), roadLight[2]);
        }

        // Road 3 (y[6], y[7]) - dieu khien boi den o vach 3
        addVehicle(new CTRUCK(400.f, y[6], 230.f, true), roadLight[3]);
        addVehicle(new CTRUCK(400.f, y[6], 340.f, true), roadLight[3]);
        if (difficultyMode != 0)
        {
            addVehicle(new CTRUCK(400.f, y[7], 345.f, false), roadLight[3]);
            addVehicle(new CBIKE(1100.f, y[7] + 28.f, 260.f, false), roadLight[3]);
        }
        if (difficultyMode == 2)
        {
            addVehicle(new CMOTOR(900.f, y[6], 430.f, false), roadLight[3]);
            addVehicle(new CTRUCK(1700.f, y[7], 300.f, true), roadLight[3]);
        }
        break;
    }

    case 1: // Ancient — chỉ có thú vật
    {
        const float* y = ANCIENT_LANE_Y;

        // Map Ancient co 4 road (moi road chi 1 lane - duong mon dat),
        // giua cac road la 1 dai co/bui ran an toan (tong 3 dai). Moi dai
        // co 1 den dieu khien CA ROAD NGAY DUOI no. Road0 (tren cung, gan
        // vach dich) khong co dai phia truoc nen luon chay tu do.
        float medianY[3] = { 140.f, 265.f, 395.f };
        float medianRedDur[3] = { 2.f, 4.f, 5.f };
        float medianGreenDur[3] = { 14.f, 9.f, 6.f };

        TrafficLight* roadLight[4] = { nullptr, nullptr, nullptr, nullptr };
        for (int i = 0; i < 3; ++i)
        {
            if (difficultyMode == 2 && i != 1)
                continue;

            TrafficLight* tl = new TrafficLight(40.f, medianY[i], medianRedDur[i], medianGreenDur[i]);
            tl->loadTexture(LIGHT_PATHS[1]);
            lights.push_back(tl);
            roadLight[i + 1] = tl;
        }

        // Road 0 (y[0]) - khong den, luon chay
        addAnimal(new CCROCODILE(3100.f, y[0], 100.f, true), roadLight[0]);
        addAnimal(new CDINOSAUR(2700.f, y[0], 280.f, true), roadLight[0]);
        if (difficultyMode != 0)
        {
            addAnimal(new CDINOSAUR(2000.f, y[0] - 20.f, 300.f, false), roadLight[0]);
        }
        if (difficultyMode == 2)
        {
            addAnimal(new COLDBIRD(1500.f, y[0], 320.f, false), roadLight[0]);
        }

        // Road 1 (y[1]) - dieu khien boi den o dai 1
        addAnimal(new COLDBIRD(300.f, y[1], 350.f, false), roadLight[1]);
        addAnimal(new CBEAST(1000.f, y[1], 130.f, false), roadLight[1]);
        if (difficultyMode != 0)
        {
            addAnimal(new CCROCODILE(1000.f, y[1] - 10.f, 80.f, true), roadLight[1]);
        }
        if (difficultyMode == 2)
        {
            addAnimal(new CDINOSAUR(1800.f, y[1], 260.f, true), roadLight[1]);
        }

        // Road 2 (y[2]) - dieu khien boi den o dai 2
        addAnimal(new CCROCODILE(150.f, y[2], 85.f, true), roadLight[2]);
        addAnimal(new CBEAST(800.f, y[2], 180.f, true), roadLight[2]);
        if (difficultyMode != 0)
        {
            addAnimal(new CBEAST(800.f, y[2], 150.f, false), roadLight[2]);
        }
        if (difficultyMode == 2)
        {
            addAnimal(new COLDBIRD(1600.f, y[2], 300.f, false), roadLight[2]);
        }

        // Road 3 (y[3]) - dieu khien boi den o dai 3
        addAnimal(new CDINOSAUR(400.f, y[3], 285.f, false), roadLight[3]);
        addAnimal(new COLDBIRD(1100.f, y[3], 150.f, false), roadLight[3]);
        if (difficultyMode != 0)
        {
            addAnimal(new COLDBIRD(1100.f, y[3], 300.f, true), roadLight[3]);
        }
        if (difficultyMode == 2)
        {
            addAnimal(new CCROCODILE(1900.f, y[3], 110.f, true), roadLight[3]);
        }
        break;
    }

    case 2: // Hell — cả xe cộ lẫn thú vật (ít lane hơn, platform to hơn)
    {
        const float* y = HELL_LANE_Y;

        float medianY[3] = { 80.f, 222.f, 365.f };
        float medianRedDur[3] = { 2.f, 3.f, 6.f };
        float medianGreenDur[3] = { 15.f, 9.f, 5.f };

        TrafficLight* roadLight[4] = { nullptr, nullptr, nullptr, nullptr };
        for (int i = 0; i < 3; ++i)
        {
            if (difficultyMode == 2 && i != 1)
                continue;

            TrafficLight* tl = new TrafficLight(550.f, medianY[i], medianRedDur[i], medianGreenDur[i]);
            tl->loadTexture(LIGHT_PATHS[2]);
            lights.push_back(tl);
            roadLight[i + 1] = tl;
        }

        // Road 0 (y[0], y[1]) - khong den, luon chay
        addVehicle(new CHELLMOTOR(100.f, y[0], 290.f, false), roadLight[0]);
        addVehicle(new CHELLMOTOR(100.f, y[0], 430.f, false), roadLight[0]);
        if (difficultyMode != 0)
        {
            addVehicle(new CHELLMOTOR(100.f, y[1], 380.f, true), roadLight[0]);
            addVehicle(new CHELLMOTOR(100.f, y[1], 210.f, true), roadLight[0]);
        }
        if (difficultyMode == 2)
        {
            addVehicle(new CHELLMOTOR(700.f, y[0], 350.f, true), roadLight[0]);
            addVehicle(new CTRAIN(1400.f, y[1] - 8.f, 300.f, false), roadLight[0]);
        }

        // Road 1 (y[2], y[3]) - dieu khien boi den o vach 1
        addVehicle(new CHELLMOTOR(100.f, y[2], 320.f, false), roadLight[1]);
        addAnimal(new CPIGLIN(500.f, y[2], 120.f, false), roadLight[1]);
        if (difficultyMode != 0)
        {
            addVehicle(new CTRAIN(100.f, y[3] - 15.f, 390.f, true), roadLight[1]);
            addAnimal(new CPIGLIN(500.f, y[3], 180.f, true), roadLight[1]);
        }
        if (difficultyMode == 2)
        {
            addVehicle(new CHELLMOTOR(900.f, y[2], 300.f, true), roadLight[1]);
            addAnimal(new CBRUTE(1500.f, y[3], 90.f, false), roadLight[1]);
        }

        // Road 2 (y[4], y[5]) - dieu khien boi den o vach 2
        addAnimal(new CPIGLIN(500.f, y[4], 240.f, false), roadLight[2]);
        addAnimal(new CPIGLIN(500.f, y[4], 120.f, false), roadLight[2]);
        if (difficultyMode != 0)
        {
            addVehicle(new CTRAIN(300.f, y[5], 310.f, true), roadLight[2]);
            addAnimal(new CBRUTE(900.f, y[5], 100.f, true), roadLight[2]);
        }
        if (difficultyMode == 2)
        {
            addAnimal(new CPIGLIN(1300.f, y[4], 200.f, true), roadLight[2]);
            addVehicle(new CHELLMOTOR(1600.f, y[5], 280.f, false), roadLight[2]);
        }

        // Road 3 (y[6], y[7]) - dieu khien boi den o vach 3
        addVehicle(new CHELLMOTOR(150.f, y[6], 300.f, false), roadLight[3]);
        addAnimal(new CPIGLIN(150.f, y[6], 140.f, false), roadLight[3]);
        if (difficultyMode != 0)
        {
            addVehicle(new CHELLMOTOR(150.f, y[7], 170.f, true), roadLight[3]);
            addAnimal(new CBRUTE(700.f, y[7], 75.f, true), roadLight[3]);
        }
        if (difficultyMode == 2)
        {
            addVehicle(new CHELLMOTOR(1000.f, y[6], 260.f, true), roadLight[3]);
            addAnimal(new CBRUTE(1700.f, y[7], 95.f, false), roadLight[3]);
        }
        break;
    }

    case 3: // Sky — chỉ có thú vật (chim/mây/thiên thần)
    {
        const float* y = SKY_LANE_Y;

        float medianY[3] = { 120.f, 255.f, 380.f };
        float medianRedDur[3] = { 3.f, 4.f, 5.f };
        float medianGreenDur[3] = { 5.f, 4.f, 3.f };

        TrafficLight* roadLight[4] = { nullptr, nullptr, nullptr, nullptr };
        for (int i = 0; i < 3; ++i)
        {
            if (difficultyMode == 2 && i != 1)
                continue;

            TrafficLight* tl = new TrafficLight(50.f, medianY[i], medianRedDur[i], medianGreenDur[i]);
            tl->loadTexture(LIGHT_PATHS[3]);
            lights.push_back(tl);
            roadLight[i + 1] = tl;
        }

        // Road 0 (y[0], y[1]) - khong den, luon bay tu do
        addAnimal(new CBIRD(700.f, y[0], 380.f, false), roadLight[0]);
        addAnimal(new CANGLE(700.f, y[0], 210.f, false), roadLight[0]);
        if (difficultyMode != 0)
        {
            addAnimal(new CCLOUD(1000.f, y[1], 150.f, true), roadLight[0]);
            addAnimal(new CBIRD(700.f, y[1], 350.f, true), roadLight[0]);
        }
        if (difficultyMode == 2)
        {
            addAnimal(new CANGLE(1300.f, y[0], 260.f, true), roadLight[0]);
            addAnimal(new CCLOUD(1600.f, y[1], 180.f, false), roadLight[0]);
        }

        // Road 1 (y[2], y[3]) - dieu khien boi den o dai 1
        addAnimal(new CBIRD(300.f, y[2], 150.f, false), roadLight[1]);
        addAnimal(new CANGLE(300.f, y[2], 230.f, false), roadLight[1]);
        if (difficultyMode != 0)
        {
            addAnimal(new CANGLE(300.f, y[3], 160.f, true), roadLight[1]);
            addAnimal(new CCLOUD(1600.f, y[3], 95.f, true), roadLight[1]);
        }
        if (difficultyMode == 2)
        {
            addAnimal(new CBIRD(1900.f, y[2], 300.f, true), roadLight[1]);
            addAnimal(new CANGLE(2200.f, y[3], 220.f, false), roadLight[1]);
        }

        // Road 2 (y[4], y[5]) - dieu khien boi den o dai 2
        addAnimal(new CBIRD(150.f, y[4], 400.f, false), roadLight[2]);
        addAnimal(new CANGLE(800.f, y[4], 200.f, false), roadLight[2]);
        if (difficultyMode != 0)
        {
            addAnimal(new CBIRD(150.f, y[5], 260.f, true), roadLight[2]);
            addAnimal(new CANGLE(800.f, y[5], 120.f, true), roadLight[2]);
        }
        if (difficultyMode == 2)
        {
            addAnimal(new CCLOUD(1400.f, y[4], 190.f, true), roadLight[2]);
            addAnimal(new CBIRD(1700.f, y[5], 340.f, false), roadLight[2]);
        }

        // Road 3 (y[6], y[7]) - dieu khien boi den o dai 3
        addAnimal(new CBIRD(1500.f, y[6], 380.f, false), roadLight[3]);
        addAnimal(new CANGLE(1500.f, y[6], 200.f, false), roadLight[3]);
        if (difficultyMode != 0)
        {
            addAnimal(new CCLOUD(1000.f, y[7], 140.f, true), roadLight[3]);
            addAnimal(new CBIRD(1500.f, y[7] + 5.f, 280.f, true), roadLight[3]);
        }
        if (difficultyMode == 2)
        {
            addAnimal(new CANGLE(600.f, y[6], 250.f, true), roadLight[3]);
            addAnimal(new CCLOUD(2000.f, y[7], 170.f, false), roadLight[3]);
        }
        break;
    }

    default:
        break;
    }

    //----------------------------------
    // ===== ADDED: co che canh bao + da lan - chi bat cho Ancient +
    // Nightmare (mấy map/do kho khac se nang cap sau) =====
    //----------------------------------

    rockManager.Reset();
    rockManager.SetActive(false);

    if (currentMap == 1 && difficultyMode == 2)
    {
        // RollingRockManager mac dinh canvasW/canvasH = 1280x720, khop
        // CANVAS_W/CANVAS_H o day nen khong can set lai

        if (!rockManager.LoadTextures(ROCK_TEXTURE_PATH, SIGN_TEXTURE_PATH))
        {
            std::cout << "[CGAME] Cannot load rolling-rock textures\n";
        }

        // ===== ADDED: am thanh da lan - khong lam gian doan gameplay neu
        // load that bai, chi la im lang luc da lan =====
        if (!rockManager.LoadSound(ROCK_SOUND_PATH))
        {
            std::cout << "[CGAME] Cannot load rolling-rock sound\n";
        }

        rockManager.SetActive(true);
    }

    //----------------------------------
    // ===== ADDED: co che canh bao + thien thach - chi bat cho Hell +
    // Nightmare =====
    //----------------------------------

    meteoriteManager.Reset();
    meteoriteManager.SetActive(false);

    if (currentMap == 2 && difficultyMode == 2)
    {
        // MeteoriteManager mac dinh canvasW/canvasH = 1280x720, khop
        // CANVAS_W/CANVAS_H o day nen khong can set lai

        if (!meteoriteManager.LoadTextures(METEORITE_TEXTURE_PATH, METEORITE_SIGN_TEXTURE_PATH))
        {
            std::cout << "[CGAME] Cannot load meteorite textures\n";
        }

        // ===== ADDED: am thanh cham dat/no cua Meteorite =====
        if (!meteoriteManager.LoadSound(METEORITE_SOUND_PATH))
        {
            std::cout << "[CGAME] Cannot load meteorite sound\n";
        }

        meteoriteManager.SetActive(true);
    }

    //----------------------------------
    // ===== ADDED: co che gio giat - chi bat cho Sky + Nightmare =====
    //----------------------------------

    windGustManager.Reset();
    windGustManager.SetActive(false);

    if (currentMap == 3 && difficultyMode == 2)
    {
        // WindGustManager mac dinh canvasW/canvasH = 1280x720, khop
        // CANVAS_W/CANVAS_H o day nen khong can set lai

        if (!windGustManager.LoadTextures(WIND_TEXTURE_PATH_1, WIND_TEXTURE_PATH_2, WIND_SIGN_TEXTURE_PATH))
        {
            std::cout << "[CGAME] Cannot load wind-gust textures\n";
        }

        // ===== ADDED: am thanh gio thoi =====
        if (!windGustManager.LoadSound(WIND_SOUND_PATH))
        {
            std::cout << "[CGAME] Cannot load wind-gust sound\n";
        }

        windGustManager.SetActive(true);
    }

    //----------------------------------
    // ===== ADDED: co che tau hoa - chi bat cho City + Nightmare =====
    //----------------------------------

    trainManager.Reset();
    trainManager.SetActive(false);

    if (currentMap == 0 && difficultyMode == 2)
    {
        if (!trainManager.LoadTextures(TRAIN_HEAD_TEXTURE_PATH, TRAIN_BODY_TEXTURE_PATH,
            TRAIN_RAIL_TEXTURE_PATH, TRAIN_SIGN_TEXTURE_PATH))
        {
            std::cout << "[CGAME] Cannot load train textures\n";
        }

        // ===== ADDED: am coi bao (Warning) + am tau chay lap (Running) =====
        if (!trainManager.LoadSounds(TRAIN_SIGN_SOUND_PATH, TRAIN_SOUND_PATH))
        {
            std::cout << "[CGAME] Cannot load train sounds\n";
        }

        // City co 4 road (moi road = 2 lane trong CITY_LANE_Y) - tau hoa
        // dung TAM cua tung road (trung binh 2 lane) de ve rail/tau va
        // xac dinh vung "an obstacle"
        const float* cy = CITY_LANE_Y;
        float roadCenters[TrainManager::ROAD_COUNT] =
        {
            (cy[0] + cy[1]) / 2.f,
            (cy[2] + cy[3]) / 2.f,
            (cy[4] + cy[5]) / 2.f,
            (cy[6] + cy[7]) / 2.f
        };
        trainManager.SetRoadCenters(roadCenters);

        trainManager.SetRoadCenters(roadCenters);

        trainManager.SetActive(true);
    }
}

//==================================================
// ===== CHANGED: KHONG con set level bat dau nua. So nguoi choi nhap o
// Custom gio la "level muon vuot qua de Win" (winTargetLevel) - gameplay
// van bat dau tu level 1 nhu Adventure, tang dan +1 tung level, va se
// Win ngay khi vuot qua dung level da nhap (level >= winTargetLevel,
// xem OnLevelComplete()). PHAI goi SAU Init() vi Init() da tinh san
// winTargetLevel mac dinh theo WIN_LEVEL_BY_MODE, ham nay se GHI DE lai
// bang gia tri nguoi choi tu nhap. Score khong doi (van bat dau 0), +100
// moi qua level nhu binh thuong =====
//==================================================

void CGAME::SetStartingLevel(int targetLevel)
{
    if (targetLevel < 1) targetLevel = 1;
    if (targetLevel > 999) targetLevel = 999;

    // ===== CHANGED: day la MOC THANG, khong phai level bat dau - level
    // van giu nguyen = 1 (da duoc Init() gan), chi ghi de winTargetLevel =====
    winTargetLevel = targetLevel;

    // ===== ADDED: Nightmare - Custom thi khong gioi han thoi gian nua
    // (nguoi choi tu chon so level muon vuot qua nen bo qua dong ho dem
    // nguoc). Ham nay chi duoc goi khi ModeSelection::isCustomNightmare()
    // ==true (xem main.cpp), nen tat timeRemaining o day la du, khong
    // anh huong Easy/Hard/Nightmare-Adventure =====
    timeRemaining = -1.f;
}

// ===== ADDED: goi lai SAU MOI LAN Init() (Retry/Restart/PlayAgain) de
// khoi phuc dung trang thai Nightmare-Custom da chon tu dau (level van
// bat dau tu 1, chi khoi phuc lai moc thang), tranh bi Init() tra ve
// moc thang mac dinh cua Adventure. Neu isCustom==false thi khong lam
// gi them (Init() tu no da dung cho Easy/Hard/Nightmare-Adventure roi) =====
void CGAME::ReapplyCustomNightmare(bool isCustom, int targetLevel)
{
    if (isCustom)
        SetStartingLevel(targetLevel);
}

//==================================================
// HandleInput — Bước 2
//==================================================

void CGAME::HandleInput(sf::Event& event)
{
    if (!player || isPaused || isGameOver)
        return;

    if (event.type != sf::Event::KeyPressed)
        return;

    // ===== ADDED: truoc khi thuc su di chuyen, du doan o ke tiep va kiem
    // tra xem co dang la "ho da" (Meteorite vua roi trung, con hieu luc
    // trong 5s) hay khong - neu co thi bo qua input nay (player bi chan) =====
    switch (event.key.code)
    {
    case sf::Keyboard::Up:
    case sf::Keyboard::W:
    {
        float nx, ny;
        player->PeekNextPosition(0, nx, ny);
        if (!meteoriteManager.IsPositionBlocked(nx, ny) &&
            !trainManager.TryBlockEntry(player, player->getY(), ny))
            player->MoveUp();
        break;
    }

    case sf::Keyboard::Down:
    case sf::Keyboard::S:
    {
        float nx, ny;
        player->PeekNextPosition(1, nx, ny);
        if (!meteoriteManager.IsPositionBlocked(nx, ny) &&
            !trainManager.TryBlockEntry(player, player->getY(), ny))
            player->MoveDown();
        break;
    }

    case sf::Keyboard::Left:
    case sf::Keyboard::A:
    {
        float nx, ny;
        player->PeekNextPosition(2, nx, ny);
        if (!meteoriteManager.IsPositionBlocked(nx, ny) &&
            !trainManager.TryBlockEntry(player, player->getY(), ny))
            player->MoveLeft();
        break;
    }

    case sf::Keyboard::Right:
    case sf::Keyboard::D:
    {
        float nx, ny;
        player->PeekNextPosition(3, nx, ny);
        if (!meteoriteManager.IsPositionBlocked(nx, ny) &&
            !trainManager.TryBlockEntry(player, player->getY(), ny))
            player->MoveRight();
        break;
    }

    default:
        break;
    }

    // Va chạm được check mỗi frame trong Update() (không chỉ sau khi
    // player di chuyển) - vì obstacle vẫn có thể tự lao vào player dù
    // player đứng yên

    // ===== ADDED (Bước 5): finish check ngay sau khi player di chuyển,
    // vì đây là hành động của chính player (khác collision, không cần
    // check liên tục mỗi frame) =====
    if (CheckFinish())
        OnLevelComplete();
}

//==================================================
// Update
//==================================================

void CGAME::Update(float dt)
{
    if (isPaused)
        return;

    if (isGameOver)
    {
        // Van cho animation chet cua player choi not (Update tu dung
        // khi het frame - xem CPEOPLE::Update), nhung khong cap nhat
        // vehicles/animals/collision nua
        if (player)
            player->Update(dt);
        return;
    }

    // ===== ADDED: da thang thi dung han gameplay (obstacle dong bang),
    // cho main.cpp thoi gian hien WinMenu =====
    if (isWin)
        return;

    if (player)
        player->Update(dt);

    // ===== ADDED: het gio ma chua Win thi Game Over ngay (Easy khong co
    // gioi han vi timeRemaining = -1, dieu kien duoi luon false) =====
    if (timeRemaining >= 0.f)
    {
        timeRemaining -= dt;
        if (timeRemaining <= 0.f)
        {
            timeRemaining = 0.f;
            OnDeath();
            return;
        }
    }

    // ===== ADDED: co che canh bao + da lan - tu Update() se khong lam gi
    // neu SetActive(false) (khong phai Ancient + Nightmare) =====
    rockManager.Update(dt, player);

    // ===== ADDED: co che canh bao + thien thach - tu Update() se khong
    // lam gi neu SetActive(false) (khong phai Hell + Nightmare) =====
    meteoriteManager.Update(dt, player);

    // ===== ADDED: player dung dung cho Meteorite roi trung luc no vua
    // cham dat -> mat 1 tim, tach rieng voi CheckCollision() (vehicles/
    // animals) vi day la su kien tuc thoi luc cham dat, khong phai
    // overlap lien tuc moi frame =====
    if (meteoriteManager.ConsumePlayerHit())
    {
        playerHP--;

        // ===== ADDED: SFX va cham =====
        if (audio)
            audio->playSound("collide");

        if (playerHP <= 0)
            OnDeath();
        else
            OnHit();
    }

    // ===== ADDED: co che gio giat - tu Update() se khong lam gi neu
    // SetActive(false) (khong phai Sky + Nightmare). Khac voi rockManager/
    // meteoriteManager, gio KHONG khoa di chuyen va KHONG gay damage -
    // chi lien tuc day nhan vat troi ngang qua ApplyWindPush() (xem
    // CPEOPLE::Update) trong suot pha Active =====
    windGustManager.Update(dt, player);

    // ===== ADDED: co che tau hoa - tu Update() se khong lam gi neu
    // SetActive(false) (khong phai City + Nightmare) =====
    trainManager.Update(dt, player);

    // ===== ADDED: player DUNG SAN tren road ma tau chay toi va cham trung
    // (khac voi TryBlockEntry o HandleInput - do la truong hop CO Y DINH
    // di chuyen vao, khong mat HP) -> mat 2 HP (nang hon va cham xe thuong) =====
    if (trainManager.ConsumePlayerHit())
    {
        playerHP -= 2;

        // ===== ADDED: SFX va cham =====
        if (audio)
            audio->playSound("collide");

        if (playerHP <= 0)
            OnDeath();
        else
            OnHit();
    }

    // ===== ADDED (Bước 5): cap nhat den, roi dong bo trang thai
    // dung/chay vao tung xe TRUOC khi goi Update() cua xe =====
    for (auto* l : lights)
        l->Update(dt);

    for (auto* v : vehicles)
    {
        if (TrafficLight* tl = v->GetTrafficLight())
            v->SetStopped(tl->GetCurrentState() == RED);

        v->Update(dt);
    }

    for (auto* a : animals)
    {
        // ===== ADDED: dong bo trang thai dung/chay tu den giao thong vao
        // animal, y het cach lam voi vehicles ben tren =====
        if (TrafficLight* tl = a->GetTrafficLight())
            a->SetStopped(tl->GetCurrentState() == RED);

        a->Update(dt);
    }

    // Va chạm: mất 1 tim, hết tim mới thực sự Game Over
    if (CheckCollision())
    {
        playerHP--;

        // ===== ADDED: SFX va cham =====
        if (audio)
            audio->playSound("collide");

        if (playerHP <= 0)
            OnDeath();
        else
            OnHit();
    }
}

//==================================================
// Draw
//==================================================

// ===== ADDED: nhan con tro AudioManager tu main.cpp =====
void CGAME::SetAudioManager(AudioManager* manager)
{
    audio = manager;

    // ===== ADDED: propagate cho 4 hazard manager de LoadSound()/LoadSounds()
    // (goi trong Init()) co the nap am thanh vao dung AudioManager - can
    // gan TRUOC khi Init() chay lan dau (main.cpp da lam dung thu tu nay) =====
    rockManager.SetAudioManager(audio);
    meteoriteManager.SetAudioManager(audio);
    windGustManager.SetAudioManager(audio);
    trainManager.SetAudioManager(audio);
}

// ===== ADDED: HUD font setup =====
void CGAME::SetFont(const sf::Font& font)
{
    hudScoreText.setFont(font);
    hudScoreText.setCharacterSize(22);
    hudScoreText.setFillColor(sf::Color::White);
    hudScoreText.setOutlineColor(sf::Color::Black);
    hudScoreText.setOutlineThickness(2.f);
    hudScoreText.setPosition(16.f, 14.f);

    hudLevelText.setFont(font);
    hudLevelText.setCharacterSize(22);
    hudLevelText.setFillColor(sf::Color::White);
    hudLevelText.setOutlineColor(sf::Color::Black);
    hudLevelText.setOutlineThickness(2.f);
    hudLevelText.setPosition(16.f, 42.f);

    hudHPText.setFont(font);
    hudHPText.setCharacterSize(22);
    hudHPText.setFillColor(sf::Color(255, 90, 90));
    hudHPText.setOutlineColor(sf::Color::Black);
    hudHPText.setOutlineThickness(2.f);
    hudHPText.setPosition(16.f, 70.f);

    // ===== CHANGED: dong ho dem nguoc chuyen ra GIUA TREN man hinh - dung
    // HUD_TIME_FONT_SIZE (co the chinh o dau file) thay vi 22 co dinh.
    // Khong setPosition() o day nua vi Draw() se tu tinh lai origin/vi tri
    // moi frame de LUON can giua theo be rong that cua chuoi "Time: mm:ss" =====
    hudTimeText.setFont(font);
    hudTimeText.setCharacterSize(HUD_TIME_FONT_SIZE);
    hudTimeText.setFillColor(sf::Color(255, 220, 80));
    hudTimeText.setOutlineColor(sf::Color::Black);
    hudTimeText.setOutlineThickness(2.f);
}

void CGAME::Draw()
{
    mWindow.draw(mapSprite);

    for (auto* l : lights)
        l->Draw(mWindow);

    for (auto* v : vehicles)
    {
        // ===== ADDED: xe nam tren road dang bi TrainManager "chiem"
        // (Warning hoac Running) thi tam thoi bien mat, khong ve nua =====
        sf::FloatRect vBox = v->GetBoundingBox();
        if (trainManager.IsRoadActive(vBox.top + vBox.height / 2.f))
            continue;

        v->Draw(mWindow);
    }

    for (auto* a : animals)
    {
        sf::FloatRect aBox = a->GetBoundingBox();
        if (trainManager.IsRoadActive(aBox.top + aBox.height / 2.f))
            continue;

        a->Draw(mWindow);
    }

    if (player)
        player->Draw(mWindow);

    // ===== ADDED: ve canh bao / da lan LEN TREN player de nguoi choi
    // luon thay ro nguy hiem (khong lam gi neu dang tat) =====
    rockManager.Draw(mWindow);

    // ===== ADDED: ve canh bao / thien thach LEN TREN player, cung logic
    // nhu rockManager (khong lam gi neu dang tat) =====
    meteoriteManager.Draw(mWindow);

    // ===== ADDED: ve canh bao / dai gio LEN TREN player, cung logic nhu
    // rockManager/meteoriteManager (khong lam gi neu dang tat) =====
    windGustManager.Draw(mWindow);

    // ===== ADDED: ve rail/bien bao/tau LEN TREN player, cung logic nhu
    // 3 manager kia (khong lam gi neu dang tat) =====
    trainManager.Draw(mWindow);

    // HUD Score/Level/HP, goc tren-trai (doi xung icon Pause tren-phai)
    std::ostringstream ossScore;
    ossScore << "Score: " << score;
    hudScoreText.setString(ossScore.str());

    std::ostringstream ossLevel;
    ossLevel << "Level: " << level;
    hudLevelText.setString(ossLevel.str());

    std::ostringstream ossHP;
    ossHP << "HP: " << playerHP << "/" << playerMaxHP;
    hudHPText.setString(ossHP.str());

    mWindow.draw(hudScoreText);
    mWindow.draw(hudLevelText);
    mWindow.draw(hudHPText);

    // ===== ADDED: chi ve dong ho neu do kho hien tai co gioi han thoi
    // gian (Easy thi timeRemaining = -1, HasTimeLimit() tra ve false) =====
    if (HasTimeLimit())
    {
        int totalSec = (int)std::ceil(timeRemaining);
        int mm = totalSec / 60;
        int ss = totalSec % 60;

        std::ostringstream ossTime;
        ossTime << mm << ":" << (ss < 10 ? "0" : "") << ss;
        hudTimeText.setString(ossTime.str());

        // ===== CHANGED: can giua ngang theo dung be rong that cua chuoi
        // (vi "Time: 1:29" va "Time: 10:05" khong dai bang nhau) - set lai
        // origin theo local bounds roi dat X = giua canvas, Y = margin tren.
        // Lam moi frame de van dung ngay ca khi so giay/phut doi so chu so =====
        sf::FloatRect tBounds = hudTimeText.getLocalBounds();
        hudTimeText.setOrigin(tBounds.left + tBounds.width / 2.f, tBounds.top);
        hudTimeText.setPosition(CANVAS_W / 2.f, HUD_TIME_MARGIN_TOP);

        mWindow.draw(hudTimeText);
    }
}

//==================================================
// Collision / Finish — Bước 4-5
//==================================================

bool CGAME::CheckCollision()
{
    if (!player || !player->IsAlive())
        return false;

    sf::FloatRect pBox = player->GetBoundingBox();

    for (auto* v : vehicles)
    {
        sf::FloatRect vBox = v->GetBoundingBox();

        // ===== ADDED: xe dang bi TrainManager "an" thi khong tinh va cham =====
        if (trainManager.IsRoadActive(vBox.top + vBox.height / 2.f))
            continue;

        if (pBox.intersects(vBox))
            return true;
    }

    for (auto* a : animals)
    {
        sf::FloatRect aBox = a->GetBoundingBox();

        if (trainManager.IsRoadActive(aBox.top + aBox.height / 2.f))
            continue;

        if (pBox.intersects(aBox))
            return true;
    }

    return false;
}

bool CGAME::CheckFinish()
{
    if (!player || !player->IsAlive())
        return false;

    // Nguoi choi vuot qua hang lane tren cung, vao vung trang tri dinh
    // man hinh -> coi la hoan thanh level. Nguong nay uoc luong tu
    // CANVAS_H, dieu chinh lai neu thay finish som/muon hon hinh ve.
    const float FINISH_Y = 100.f;

    return player->getY() <= FINISH_Y;
}

void CGAME::OnDeath()
{
    if (player)
        player->TriggerDeath();   // đổi sang animation chết

    isGameOver = true;
}

// Mất 1 tim nhưng vẫn còn tim khác - đưa player về vạch xuất phát,
// giữ nguyên level/score/obstacle, KHÔNG kết thúc game
void CGAME::OnHit()
{
    if (player)
        player->Reset(CANVAS_W / 2.f, CANVAS_H - 40.f);
}

void CGAME::OnLevelComplete()
{
    // ===== CHANGED: dung winTargetLevel (da tinh san trong Init()/
    // SetStartingLevel()) thay vi tinh lai nguong tuyet doi tu
    // WIN_LEVEL_BY_MODE o day - dam bao Nightmare-Custom van phai vuot
    // dung so luong level nhu Adventure, khong bi thang ngay lap tuc =====
    score += 100;

    if (level >= winTargetLevel)
    {
        isWin = true;
        return;
    }

    level++;

    // Dua player ve lai vach xuat phat de bat dau lane hien tai lai tu dau
    if (player)
        player->Reset(CANVAS_W / 2.f, CANVAS_H - 40.f);

    // ===== ADDED (Bước 6): checkpoint - luu lai tien do moi lan qua man,
    // de nut "Continue" o MainMenu nap lai duoc dung cho =====
    SaveGame(savePath);

    // TODO (nâng cao, không bắt buộc): tăng speed cua vehicles/animals
    // theo level de tang do kho moi lan qua man
}

//==================================================
// Save/Load — Bước 6
//==================================================

void CGAME::SaveGame(const std::string& path)
{
    std::ofstream out(path);

    if (!out.is_open())
    {
        std::cout << "[CGAME] Cannot open save file for writing: " << path << "\n";
        return;
    }

    // New format: each field on its own line
    // playerName
    // characterIndex
    // mapIndex
    // score
    // difficultyMode
    // level
    // saveTime (DD/MM/YYYY HH:MM:SS)

    // Get current time
    std::time_t t = std::time(nullptr);
    char buf[64] = { 0 };
    std::tm tm;
#if defined(_MSC_VER)
    localtime_s(&tm, &t);
#else
    std::tm* ptm = std::localtime(&t);
    if (ptm) tm = *ptm;
#endif
    if (std::strftime(buf, sizeof(buf), "%d/%m/%Y %H:%M:%S", &tm) == 0)
        buf[0] = '\0';

    out << playerName << "\n"
        << characterIndex << "\n"
        << currentMap << "\n"
        << score << "\n"
        << difficultyMode << "\n"
        << level << "\n"
        << buf << "\n";

    // ===== ADDED (Continue Menu preview + Load Game khoi phuc vi tri):
    // ghi them huong/frame VA toa do (x,y) HIEN TAI cua player (tai
    // chinh thoi diem bam L de save) - de ContinueMenu hien dung preview
    // trang thai cuoi cung, VA de LoadGame() dat lai dung vi tri nhan
    // vat dang dung tren duong thay vi luon spawn lai o vach xuat phat.
    // Nam O CUOI file (sau saveTime) nen file save CU (chua co cac dong
    // nay) van doc duoc binh thuong qua PeekSaveData(), chi thieu phan
    // bonus nay (LoadGame() se fallback ve vi tri spawn mac dinh) =====
    if (player)
    {
        out << player->GetDirection() << "\n"
            << player->GetCurrentFrame() << "\n"
            << player->getX() << "\n"
            << player->getY() << "\n";
    }
}

// ===== ADDED (Quick Save - phim L): sinh duong dan file save moi, luon
// nam trong Save/ va khong trung ten voi file da co =====
std::string CGAME::GenerateAutoSavePath()
{
    namespace fs = std::filesystem;

    std::error_code ec;
    fs::create_directories(SAVE_DIR, ec);   // dam bao thu muc Save/ ton tai

    std::time_t t = std::time(nullptr);
    std::tm tm;
#if defined(_MSC_VER)
    localtime_s(&tm, &t);
#else
    std::tm* ptm = std::localtime(&t);
    if (ptm) tm = *ptm;
#endif

    char buf[32] = { 0 };
    std::strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", &tm);

    std::string path = SAVE_DIR + "/save_" + buf + ".sav";

    // Phong truong hop bam L 2 lan trong cung 1 giay (trung ten file) -
    // them hau to _2, _3... cho den khi tim duoc ten con trong
    int suffix = 2;
    while (fs::exists(path, ec))
    {
        path = SAVE_DIR + "/save_" + std::string(buf) + "_" + std::to_string(suffix) + ".sav";
        ++suffix;
    }

    return path;
}

bool CGAME::LoadGame(const std::string& path)
{
    std::ifstream in(path);

    if (!in.is_open())
    {
        std::cout << "[CGAME] No save file found: " << path << "\n";
        return false;
    }

    std::string line;
    // playerName
    if (!std::getline(in, line)) return false;
    playerName = line;

    // characterIndex
    if (!std::getline(in, line)) return false;
    int savedCharacter = std::stoi(line);

    // mapIndex
    if (!std::getline(in, line)) return false;
    int savedMap = std::stoi(line);

    // score
    if (!std::getline(in, line)) return false;
    int savedScore = std::stoi(line);

    // difficultyMode
    if (!std::getline(in, line)) return false;
    int savedMode = std::stoi(line);

    // level
    if (!std::getline(in, line)) return false;
    int savedLevel = std::stoi(line);

    // saveTime (ignored here)
    std::string savedTime;
    std::getline(in, savedTime);

    // ===== ADDED (Load Game - khoi phuc dung vi tri/trang thai): 4 dong
    // bonus O CUOI file - huong, frame, x, y cua player tai thoi diem
    // save. TUY CHON: file save CU (truoc khi co tinh nang nay) khong co
    // cac dong nay - trong truong hop do coi nhu "khong co du lieu vi
    // tri", giu nguyen hanh vi CU (spawn o vach xuat phat mac dinh cua
    // Init()) thay vi bao loi/that bai ca viec Load =====
    bool hasPositionData = false;
    int savedDirection = 1;
    int savedFrame = 0;
    float savedX = -1.f, savedY = -1.f;

    if (std::getline(in, line))
    {
        try { savedDirection = std::stoi(line); }
        catch (...) { savedDirection = 1; }

        if (std::getline(in, line))
        {
            try { savedFrame = std::stoi(line); }
            catch (...) { savedFrame = 0; }

            if (std::getline(in, line))
            {
                try { savedX = std::stof(line); }
                catch (...) { savedX = -1.f; }

                if (std::getline(in, line))
                {
                    try { savedY = std::stof(line); }
                    catch (...) { savedY = -1.f; }
                    hasPositionData = (savedX >= 0.f && savedY >= 0.f);
                }
            }
        }
    }

    // Apply loaded data
    difficultyMode = savedMode;
    Init(savedMap, savedCharacter);   // spawn player o vach xuat phat mac dinh

    // Neu file save co du lieu vi tri (save moi, tu ban co Quick Save L)
    // thi DAT LAI dung vi tri/huong/frame nhan vat dang dung tren duong,
    // GHI DE len vi tri spawn mac dinh vua duoc Init() thiet lap o tren
    if (hasPositionData && player)
    {
        player->RestoreState(savedX, savedY, savedDirection, savedFrame);
    }

    level = savedLevel;
    score = savedScore;
    savePath = path;

    return true;
}

// ===== CHANGED: them tham so mode - moi map/mode la 1 file save doc lap =====
const std::string& CGAME::GetSavePathForMap(int mapIndex, int mode)
{
    if (mapIndex < 0 || mapIndex > 3)
        mapIndex = 0;
    if (mode < 0 || mode > 2)
        mode = 1;

    return SAVE_PATHS[mapIndex][mode];
}
// New: get save path for slot index
const std::string& CGAME::GetSavePathForSlot(int slot)
{
    if (slot < 0 || slot > 3) slot = 0;
    return SAVE_SLOT_PATHS[slot];
}

bool CGAME::PeekSaveInfo(const std::string& path,
    int& outMap, int& outCharacter, int& outLevel, int& outScore,
    int& outMode)
{
    // Backwards compatible: read new format and fill ints
    std::ifstream in(path);
    if (!in.is_open()) return false;

    std::string line;
    // playerName
    if (!std::getline(in, line)) return false;
    // characterIndex
    if (!std::getline(in, line)) return false;
    outCharacter = std::stoi(line);
    // mapIndex
    if (!std::getline(in, line)) return false;
    outMap = std::stoi(line);
    // score
    if (!std::getline(in, line)) return false;
    outScore = std::stoi(line);
    // difficultyMode
    if (!std::getline(in, line)) return false;
    outMode = std::stoi(line);
    // level
    if (!std::getline(in, line)) return false;
    outLevel = std::stoi(line);

    return true;
}

// ===== ADDED (Screenshot preview) =====
std::string CGAME::GetThumbnailPathFor(const std::string& savePath)
{
    std::string path = savePath;

    const std::string ext = ".sav";
    if (path.size() >= ext.size() &&
        path.compare(path.size() - ext.size(), ext.size(), ext) == 0)
    {
        path.erase(path.size() - ext.size());
    }

    path += ".png";
    return path;
}

bool CGAME::PeekSaveData(const std::string& path, SaveData& out)
{
    // ===== CHANGED (Giai doan 1 - Continue Menu redesign) =====
    // Reset ve mac dinh truoc, sau do LUON dien filePath/fileSizeBytes/
    // lastWriteTimeUnix mien la file ton tai tren dia - ke ca khi noi
    // dung ben trong bi hong/thieu dong. Nho vay ListAllSaves() (ben
    // duoi) van liet ke duoc ca cac save loi (giong dong "c" - Version:
    // Invalid trong anh mau Thrive) thay vi lam chung bien mat khoi
    // danh sach. Hop dong tra ve (bool) cua ham nay GIU NGUYEN nhu cu:
    // false neu khong mo duoc file HOAC parse thieu/loi truong bat
    // buoc - de khong lam vo cac noi dang goi ham nay (ContinueMenu
    // hien tai dang dua vao gia tri tra ve nay).
    out = SaveData();
    out.filePath = path;

    std::error_code ec;
    if (std::filesystem::exists(path, ec) && !ec)
    {
        out.exists = true;

        auto sz = std::filesystem::file_size(path, ec);
        out.fileSizeBytes = ec ? 0 : static_cast<long long>(sz);

        ec.clear();
        auto ftime = std::filesystem::last_write_time(path, ec);
        out.lastWriteTimeUnix = ec ? 0 : FileTimeToUnix(ftime);

        // ===== ADDED (Screenshot preview): kiem tra co file anh .png
        // cung ten hay khong (chup luc bam L). Chi dien thumbnailPath
        // neu file anh THAT SU ton tai tren dia, de ContinueMenu biet
        // khi nao fallback ve preview nhan vat cu =====
        ec.clear();
        std::string thumbPath = GetThumbnailPathFor(path);
        if (std::filesystem::exists(thumbPath, ec) && !ec)
            out.thumbnailPath = thumbPath;
    }

    std::ifstream in(path);
    if (!in.is_open()) return false;

    std::string line;
    if (!std::getline(in, line)) return false;
    out.playerName = line;

    if (!std::getline(in, line)) return false;
    try { out.characterIndex = std::stoi(line); }
    catch (...) { return false; }

    if (!std::getline(in, line)) return false;
    try { out.mapIndex = std::stoi(line); }
    catch (...) { return false; }

    if (!std::getline(in, line)) return false;
    try { out.score = std::stoi(line); }
    catch (...) { return false; }

    if (!std::getline(in, line)) return false;
    try { out.difficultyMode = std::stoi(line); }
    catch (...) { return false; }

    if (!std::getline(in, line)) return false;
    try { out.level = std::stoi(line); }
    catch (...) { return false; }

    if (!std::getline(in, line)) line = "";
    out.saveTime = line;

    // ===== ADDED (Continue Menu preview): 2 dong bonus O CUOI file -
    // huong (0..4) va frame (0..3) cua player tai thoi diem save. TUY
    // CHON: file save CU khong co 2 dong nay van duoc coi la HOP LE
    // (isValid=true), chi don gian giu nguyen gia tri mac dinh 1/0 da
    // dat san trong SaveData - khong duoc lam that bai (return false)
    // ca file chi vi thieu 2 dong THEM VAO SAU nay =====
    if (std::getline(in, line))
    {
        try { out.lastDirection = std::stoi(line); }
        catch (...) { out.lastDirection = 1; }
    }
    if (std::getline(in, line))
    {
        try { out.lastFrame = std::stoi(line); }
        catch (...) { out.lastFrame = 0; }
    }
    if (std::getline(in, line))
    {
        try { out.lastX = std::stof(line); }
        catch (...) { out.lastX = -1.f; }
    }
    if (std::getline(in, line))
    {
        try { out.lastY = std::stof(line); }
        catch (...) { out.lastY = -1.f; }
    }

    out.isValid = true;
    return true;
}

// ===== ADDED (Giai doan 1 - Continue Menu redesign) =====
std::vector<CGAME::SaveData> CGAME::ListAllSaves()
{
    namespace fs = std::filesystem;
    std::vector<SaveData> result;

    std::error_code ec;
    if (!fs::exists(SAVE_DIR, ec) || ec || !fs::is_directory(SAVE_DIR, ec) || ec)
        return result;   // chua co thu muc Save/ -> danh sach rong, khong loi

    for (const auto& entry : fs::directory_iterator(SAVE_DIR, ec))
    {
        if (ec) break;
        if (!entry.is_regular_file()) continue;
        if (entry.path().extension() != ".sav") continue;

        // Chuan hoa dau '\\' -> '/' de dong nhat voi cac duong dan
        // dang dung "Save/xxx.sav" o phan con lai cua code
        std::string path = entry.path().string();
        for (auto& c : path) if (c == '\\') c = '/';

        SaveData sd;
        PeekSaveData(path, sd);   // bo qua gia tri tra ve: du hong hay
        // khong, sd van co filePath/size/mtime
        // huu ich (xem isValid de biet noi
        // dung co doc duoc day du hay khong)
        result.push_back(std::move(sd));
    }

    // File sua doi gan day nhat len dau, giong cach Thrive sap xep
    std::sort(result.begin(), result.end(),
        [](const SaveData& a, const SaveData& b)
        {
            return a.lastWriteTimeUnix > b.lastWriteTimeUnix;
        });

    return result;
}

bool CGAME::DeleteSave(const std::string& path)
{
    std::error_code ec;
    bool removed = std::filesystem::remove(path, ec);
    if (ec)
    {
        std::cout << "[CGAME] Cannot delete save: " << path << " (" << ec.message() << ")\n";
        return false;
    }

    // ===== ADDED (Screenshot preview): xoa luon file anh thumbnail cung
    // ten (neu co) de khong de lai file .png "mo coi" trong thu muc
    // Save/ sau khi save da bi xoa. Best-effort - khong lam that bai ca
    // ham neu khong xoa duoc anh (VD anh khong ton tai) =====
    std::error_code ecThumb;
    std::filesystem::remove(GetThumbnailPathFor(path), ecThumb);

    return removed;
}

long long CGAME::GetSaveFileSize(const std::string& path)
{
    std::error_code ec;
    auto sz = std::filesystem::file_size(path, ec);
    if (ec) return 0;
    return static_cast<long long>(sz);
}

long long CGAME::GetTotalSaveSpaceUsed()
{
    long long total = 0;
    for (const auto& sd : ListAllSaves())
        total += sd.fileSizeBytes;
    return total;
}