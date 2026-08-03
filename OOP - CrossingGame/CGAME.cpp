// CGAME.cpp
#include "CGAME.h"
#include <iostream>
#include <fstream>   // ===== ADDED (Bước 6) =====
#include <sstream>   // ===== ADDED: build chuoi HUD =====

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

    // Duong dan map, dung thu tu index giong MapSelection
    // (0=City 1=Ancient 2=Hell 3=Sky)
    const std::string MAP_PATHS[4] =
    {
        "Map/City_map.png",
        "Map/Ancient_map.png",
        "Map/Hell_map.png",
        "Map/Sky_map.png"
    };

    // ===== ADDED (Bước 6, sửa lại): moi map co file save rieng, khong
    // con dung chung "save.dat" nua =====
    const std::string SAVE_PATHS[4] =
    {
        "Save/city.sav",
        "Save/ancient.sav",
        "Save/hell.sav",
        "Save/sky.sav"
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

    // ===== CHANGED (fix lane sai + đè vào hàng spawn): toa do Y gio la
    // TAM obstacle (vi origin da chuan hoa ve giua sprite), va lane cuoi
    // cung duoc keo len xa hon hang spawn cua nguoi choi (y = CANVAS_H-40 = 680)
    // de chua du khong gian cho sprite cao (train, brute...) =====
    const float CITY_LANE_Y[] = { 125.f, 180.f, 250.f, 300.f, 370.f, 420.f, 490.f, 540.f };
    const float ANCIENT_LANE_Y[] = { 160.f, 280.f, 420.f, 550.f };
    const float HELL_LANE_Y[] = { 105.f, 137.f, 220.f, 270.f, 360.f, 400.f, 510.f, 550.f };
    const float SKY_LANE_Y[] = { 125.f, 180.f, 255.f, 310.f, 375.f, 430.f, 515.f, 570.f };

    // ===== ADDED (Bước 5): texture den giao thong rieng theo tung map =====
    const std::string LIGHT_PATHS[4] =
    {
        "Obstacles/City_light.png",
        "Obstacles/Ancient_light.png",
        "Obstacles/Hell_light.png",
        "Obstacles/Sky_light.png"
    };
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

    // ===== ADDED (Bước 6, sửa lại): moi map luu vao file rieng =====
    savePath = SAVE_PATHS[currentMap];

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
    if (difficultyMode == 0)       speedDelta = -70.f;  // Easy
    else if (difficultyMode == 2)  speedDelta = 40.f;   // Nightmare

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
        addVehicle(new CMOTOR(2000.f, y[0] + 2.f, 290.f, false), roadLight[0]);
        addVehicle(new CMOTOR(2000.f, y[0], 370.f, false), roadLight[0]);
        if (difficultyMode != 0)
        {
            addVehicle(new CBIKE(1100.f, y[1], 180.f, true), roadLight[0]);
            addVehicle(new CTRUCK(1000.f, y[1], 280.f, true), roadLight[0]);
        }
        if (difficultyMode == 2)
        {
            addVehicle(new CBIKE(600.f, y[0], 260.f, true), roadLight[0]);
            addVehicle(new CTRUCK(1600.f, y[1], 220.f, false), roadLight[0]);
        }

        // Road 1 (y[2], y[3]) - dieu khien boi den o vach 1
        addVehicle(new CMOTOR(300.f, y[2], 320.f, true), roadLight[1]);
        addVehicle(new CTRUCK(1000.f, y[2], 180.f, true), roadLight[1]);
        if (difficultyMode != 0)
        {
            addVehicle(new CMOTOR(300.f, y[3], 350.f, false), roadLight[1]);
            addVehicle(new CTRUCK(1000.f, y[3], 240.f, false), roadLight[1]);
        }
        if (difficultyMode == 2)
        {
            addVehicle(new CMOTOR(700.f, y[2], 300.f, false), roadLight[1]);
            addVehicle(new CBIKE(1500.f, y[3], 200.f, true), roadLight[1]);
        }

        // Road 2 (y[4], y[5]) - dieu khien boi den o vach 2
        addVehicle(new CBIKE(150.f, y[4], 200.f, false), roadLight[2]);
        addVehicle(new CMOTOR(800.f, y[4], 390.f, false), roadLight[2]);
        if (difficultyMode != 0)
        {
            addVehicle(new CBIKE(150.f, y[5], 150.f, true), roadLight[2]);
            addVehicle(new CMOTOR(800.f, y[5], 300.f, true), roadLight[2]);
        }
        if (difficultyMode == 2)
        {
            addVehicle(new CTRUCK(1400.f, y[4], 260.f, true), roadLight[2]);
            addVehicle(new CBIKE(500.f, y[5], 220.f, false), roadLight[2]);
        }

        // Road 3 (y[6], y[7]) - dieu khien boi den o vach 3
        addVehicle(new CTRUCK(400.f, y[6], 190.f, true), roadLight[3]);
        addVehicle(new CTRUCK(400.f, y[6], 240.f, true), roadLight[3]);
        if (difficultyMode != 0)
        {
            addVehicle(new CTRUCK(400.f, y[7], 275.f, false), roadLight[3]);
            addVehicle(new CBIKE(1100.f, y[7] + 28.f, 210.f, false), roadLight[3]);
        }
        if (difficultyMode == 2)
        {
            addVehicle(new CMOTOR(900.f, y[6], 330.f, false), roadLight[3]);
            addVehicle(new CTRUCK(1700.f, y[7], 200.f, true), roadLight[3]);
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
            addAnimal(new COLDBIRD(1500.f, y[0], 320.f, true), roadLight[0]);
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
            addAnimal(new CDINOSAUR(1800.f, y[1], 260.f, false), roadLight[1]);
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
            addVehicle(new CTRAIN(1400.f, y[1], 300.f, false), roadLight[0]);
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

    switch (event.key.code)
    {
    case sf::Keyboard::Up:
    case sf::Keyboard::W:
        player->MoveUp();
        break;

    case sf::Keyboard::Down:
    case sf::Keyboard::S:
        player->MoveDown();
        break;

    case sf::Keyboard::Left:
    case sf::Keyboard::A:
        player->MoveLeft();
        break;

    case sf::Keyboard::Right:
    case sf::Keyboard::D:
        player->MoveRight();
        break;

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
}

void CGAME::Draw()
{
    mWindow.draw(mapSprite);

    for (auto* l : lights)
        l->Draw(mWindow);

    for (auto* v : vehicles)
        v->Draw(mWindow);

    for (auto* a : animals)
        a->Draw(mWindow);

    if (player)
        player->Draw(mWindow);

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
        if (pBox.intersects(v->GetBoundingBox()))
            return true;
    }

    for (auto* a : animals)
    {
        if (pBox.intersects(a->GetBoundingBox()))
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
    // ===== ADDED: qua du 3 man (level hien tai la man thu may nguoi choi
    // vua hoan thanh) -> thang, dung lai khong tang level/reset player nua =====
    const int WIN_AFTER_LEVEL = 3;

    score += 100;

    if (level >= WIN_AFTER_LEVEL)
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

// ===== ADDED (Bước 6, sửa lại) =====
const std::string& CGAME::GetSavePathForMap(int mapIndex)
{
    if (mapIndex < 0 || mapIndex > 3)
        mapIndex = 0;

    return SAVE_PATHS[mapIndex];
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