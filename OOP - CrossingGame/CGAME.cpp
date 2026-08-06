// CGAME.cpp
#include "CGAME.h"
#include <iostream>
#include <fstream>   // ===== ADDED (Bước 6) =====
#include <sstream>   // ===== ADDED: build chuoi HUD =====
#include <cmath>     // ===== ADDED: std::ceil cho dong ho HUD =====

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
    timeRemaining(-1.f),
    savePath("save.dat")
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
    // ===== CHANGED: so level can hoan thanh gio khac nhau theo do kho
    // (Easy 3, Hard 5, Nightmare 6) - xem WIN_LEVEL_BY_MODE =====
    int winAfterLevel = WIN_LEVEL_BY_MODE[difficultyMode];

    score += 100;

    if (level >= winAfterLevel)
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

    // Format don gian: moi gia tri 1 dong, theo thu tu co dinh
    out << currentMap << "\n"
        << characterIndex << "\n"
        << level << "\n"
        << score << "\n"
        << difficultyMode << "\n";   // ===== ADDED =====
}

bool CGAME::LoadGame(const std::string& path)
{
    std::ifstream in(path);

    if (!in.is_open())
    {
        std::cout << "[CGAME] No save file found: " << path << "\n";
        return false;
    }

    int savedMap = 0, savedCharacter = 0, savedLevel = 1, savedScore = 0, savedMode = 1;

    if (!(in >> savedMap >> savedCharacter >> savedLevel >> savedScore >> savedMode))
    {
        std::cout << "[CGAME] Save file is corrupted: " << path << "\n";
        return false;
    }

    // Dung lai Init() de load dung map/nhan vat/spawn obstacle nhu binh
    // thuong, roi ghi de level/score bang gia tri da luu
    difficultyMode = savedMode;   // ===== ADDED: khoi phuc dung do kho da chon =====
    Init(savedMap, savedCharacter);

    level = savedLevel;
    score = savedScore;

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

bool CGAME::PeekSaveInfo(const std::string& path,
    int& outMap, int& outCharacter, int& outLevel, int& outScore,
    int& outMode)
{
    std::ifstream in(path);

    if (!in.is_open())
        return false;

    if (!(in >> outMap >> outCharacter >> outLevel >> outScore >> outMode))
        return false;

    return true;
}
