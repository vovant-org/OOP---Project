// CGAME.cpp
#include "CGAME.h"
#include <iostream>

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

    // Duong dan nhan vat, dung thu tu index giong CharacterSelection
    // (0=Chicken 1=Knight 2=Dog 3=Luffy)
    const std::string CHARACTER_PATHS[4] =
    {
        "Character/Chicken_character.png",
        "Character/Knight_character.png",
        "Character/Dog_character.png",
        "Character/Luffy_character.png"
    };

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
    player(nullptr),
    level(1),
    score(0),
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

    delete player; // phong khi Init duoc goi lai (choi lai / doi map)

    float startX = CANVAS_W / 2.f;
    float startY = CANVAS_H - 40.f; // gan mep duoi man hinh

    player = new CPEOPLE(startX, startY);

    if (!player->loadTexture(CHARACTER_PATHS[characterIndex]))
    {
        std::cout << "[CGAME] Cannot load character: "
            << CHARACTER_PATHS[characterIndex] << "\n";
    }

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

    switch (currentMap)
    {
    case 0: // City — chỉ có xe cộ
    {
        const float* y = CITY_LANE_Y;

        // ===== CHANGED (Bước 5b): map co 4 road (moi road = 2 lane, chia
        // boi vach ke duong), giua cac road la 1 "vach" an toan cho
        // character dung (tong 3 vach). Moi vach co DUY NHAT 1 den giao
        // thong, va den nay dieu khien CA ROAD (ca 2 lane) NGAY DUOI no.
        // -> road0 (2 lane dau, gan vach dich, tren cung) khong co vach
        // phia truoc no nen luon chay tu do (khong bi den chi phoi).
        float medianY[3] =
        {
            120.f,   // vach giua road0 va road1 -> dieu khien road1
            245.f,   // vach giua road1 va road2 -> dieu khien road2
            372.f    // vach giua road2 va road3 -> dieu khien road3
        };

        // ===== ADDED: thoi gian do/xanh RIENG cho tung den (den 0,1,2
        // tuong ung dieu khien road1, road2, road3). Chinh so o day de
        // doi nhip do/xanh cua tung road =====
        float medianRedDur[3] = { 2.f, 3.f, 6.f };
        float medianGreenDur[3] = { 15.f, 10.f, 5.f };

        TrafficLight* roadLight[4] = { nullptr, nullptr, nullptr, nullptr };
        for (int i = 0; i < 3; ++i)
        {
            TrafficLight* tl = new TrafficLight(10.f, medianY[i], medianRedDur[i], medianGreenDur[i]);
            tl->loadTexture(LIGHT_PATHS[0]);
            lights.push_back(tl);
            roadLight[i + 1] = tl; // den thu i dieu khien road (i+1)
        }

        // Road 0 (y[0], y[1]) - khong den, luon chay
        vehicles.push_back(new CMOTOR(2000.f, y[0] + 2.f, 290.f, false));
        vehicles.back()->SetTrafficLight(roadLight[0]);
        vehicles.push_back(new CMOTOR(2000.f, y[0], 370.f, false));
        vehicles.back()->SetTrafficLight(roadLight[0]);
        vehicles.push_back(new CBIKE(1100.f, y[1], 180.f, true));
        vehicles.back()->SetTrafficLight(roadLight[0]);
        vehicles.push_back(new CTRUCK(1000.f, y[1], 280.f, true));
        vehicles.back()->SetTrafficLight(roadLight[0]);

        // Road 1 (y[2], y[3]) - dieu khien boi den o vach 1
        vehicles.push_back(new CMOTOR(300.f, y[2], 320.f, true));
        vehicles.back()->SetTrafficLight(roadLight[1]);
        vehicles.push_back(new CTRUCK(1000.f, y[2], 180.f, true));
        vehicles.back()->SetTrafficLight(roadLight[1]);
        vehicles.push_back(new CMOTOR(300.f, y[3], 350.f, false));
        vehicles.back()->SetTrafficLight(roadLight[1]);
        vehicles.push_back(new CTRUCK(1000.f, y[3], 240.f, false));
        vehicles.back()->SetTrafficLight(roadLight[1]);

        // Road 2 (y[4], y[5]) - dieu khien boi den o vach 2
        vehicles.push_back(new CBIKE(150.f, y[4], 200.f, false));
        vehicles.back()->SetTrafficLight(roadLight[2]);
        vehicles.push_back(new CMOTOR(800.f, y[4], 390.f, false));
        vehicles.back()->SetTrafficLight(roadLight[2]);
        vehicles.push_back(new CBIKE(150.f, y[5], 150.f, true));
        vehicles.back()->SetTrafficLight(roadLight[2]);
        vehicles.push_back(new CMOTOR(800.f, y[5], 300.f, true));
        vehicles.back()->SetTrafficLight(roadLight[2]);

        // Road 3 (y[6], y[7]) - dieu khien boi den o vach 3
        vehicles.push_back(new CTRUCK(400.f, y[6], 190.f, true));
        vehicles.back()->SetTrafficLight(roadLight[3]);
        vehicles.push_back(new CTRUCK(400.f, y[6], 240.f, true));
        vehicles.back()->SetTrafficLight(roadLight[3]);
        vehicles.push_back(new CTRUCK(400.f, y[7], 275.f, false));
        vehicles.back()->SetTrafficLight(roadLight[3]);
        vehicles.push_back(new CBIKE(1100.f, y[7] + 28.f, 210.f, false));
        vehicles.back()->SetTrafficLight(roadLight[3]);
        break;
    }

    case 1: // Ancient — chỉ có thú vật
    {
        const float* y = ANCIENT_LANE_Y;

        // ===== ADDED: map Ancient co 4 road (khac City/Hell/Sky, moi
        // road o day chi co 1 "lane" duy nhat - duong mon dat), giua cac
        // road la 1 dai co/bui ran an toan (tong 3 dai). Moi dai co 1 den
        // dieu khien CA ROAD NGAY DUOI no. Road0 (tren cung, gan vach
        // dich) khong co dai phia truoc nen luon chay tu do.
        float medianY[3] =
        {
            140.f,   // dai giua road0 va road1 -> dieu khien road1
            265.f,   // dai giua road1 va road2 -> dieu khien road2
            395.f    // dai giua road2 va road3 -> dieu khien road3
        };

        // Thoi gian do/xanh rieng cho tung den - chinh o day de doi nhip
        float medianRedDur[3] = { 2.f, 4.f, 5.f };
        float medianGreenDur[3] = { 14.f, 9.f, 6.f };

        TrafficLight* roadLight[4] = { nullptr, nullptr, nullptr, nullptr };
        for (int i = 0; i < 3; ++i)
        {
            TrafficLight* tl = new TrafficLight(40.f, medianY[i], medianRedDur[i], medianGreenDur[i]);
            tl->loadTexture(LIGHT_PATHS[1]);
            lights.push_back(tl);
            roadLight[i + 1] = tl; // den thu i dieu khien road (i+1)
        }

        // Road 0 (y[0]) - khong den, luon chay
        animals.push_back(new CCROCODILE(3100.f, y[0], 100.f, true));
        animals.back()->SetTrafficLight(roadLight[0]);
        animals.push_back(new CDINOSAUR(2700.f, y[0], 280.f, true));
        animals.back()->SetTrafficLight(roadLight[0]);
        animals.push_back(new CDINOSAUR(2000.f, y[0] - 20.f, 300.f, false));
        animals.back()->SetTrafficLight(roadLight[0]);

        // Road 1 (y[1]) - dieu khien boi den o dai 1
        animals.push_back(new COLDBIRD(300.f, y[1], 350.f, false));
        animals.back()->SetTrafficLight(roadLight[1]);
        animals.push_back(new CBEAST(1000.f, y[1], 130.f, false));
        animals.back()->SetTrafficLight(roadLight[1]);
        animals.push_back(new CCROCODILE(1000.f, y[1] - 10.f, 80.f, true));
        animals.back()->SetTrafficLight(roadLight[1]);

        // Road 2 (y[2]) - dieu khien boi den o dai 2
        animals.push_back(new CCROCODILE(150.f, y[2], 85.f, true));
        animals.back()->SetTrafficLight(roadLight[2]);
        animals.push_back(new CBEAST(800.f, y[2], 180.f, true));
        animals.back()->SetTrafficLight(roadLight[2]);
        animals.push_back(new CBEAST(800.f, y[2], 150.f, false));
        animals.back()->SetTrafficLight(roadLight[2]);

        // Road 3 (y[3]) - dieu khien boi den o dai 3
        animals.push_back(new CDINOSAUR(400.f, y[3], 285.f, false));
        animals.back()->SetTrafficLight(roadLight[3]);
        animals.push_back(new COLDBIRD(1100.f, y[3], 150.f, false));
        animals.back()->SetTrafficLight(roadLight[3]);
        animals.push_back(new COLDBIRD(1100.f, y[3], 300.f, true));
        animals.back()->SetTrafficLight(roadLight[3]);

        break;
    }

    case 2: // Hell — cả xe cộ lẫn thú vật (ít lane hơn, platform to hơn)
    {
        const float* y = HELL_LANE_Y;

        // ===== CHANGED (Bước 5b): giong City - 4 road (2 lane/road), 3 vach
        // an toan giua cac road, moi vach 1 den dieu khien CA ROAD (2 lane)
        // NGAY DUOI no. Road0 (tren cung, gan vach dich) khong co den.
        float medianY[3] =
        {
            80.f,
            222.f,
            365.f// vach giua road2 va road3 -> dieu khien road3
        };

        // ===== ADDED: thoi gian do/xanh RIENG cho tung den (den 0,1,2
        // tuong ung dieu khien road1, road2, road3). Chinh so o day de
        // doi nhip do/xanh cua tung road =====
        float medianRedDur[3] = { 2.f, 3.f, 6.f };
        float medianGreenDur[3] = { 15.f, 9.f, 5.f };

        TrafficLight* roadLight[4] = { nullptr, nullptr, nullptr, nullptr };
        for (int i = 0; i < 3; ++i)
        {
            TrafficLight* tl = new TrafficLight(550.f, medianY[i], medianRedDur[i], medianGreenDur[i]);
            tl->loadTexture(LIGHT_PATHS[2]);
            lights.push_back(tl);
            roadLight[i + 1] = tl; // den thu i dieu khien road (i+1)
        }

        // Road 0 (y[0], y[1]) - khong den, luon chay
        vehicles.push_back(new CHELLMOTOR(100.f, y[0], 290.f, false));
        vehicles.back()->SetTrafficLight(roadLight[0]);
        vehicles.push_back(new CHELLMOTOR(100.f, y[0], 430.f, false));
        vehicles.back()->SetTrafficLight(roadLight[0]);
        vehicles.push_back(new CHELLMOTOR(100.f, y[1], 380.f, true));
        vehicles.back()->SetTrafficLight(roadLight[0]);
        vehicles.push_back(new CHELLMOTOR(100.f, y[1], 210.f, true));
        vehicles.back()->SetTrafficLight(roadLight[0]);

        // Road 1 (y[2], y[3]) - dieu khien boi den o vach 1
        vehicles.push_back(new CHELLMOTOR(100.f, y[2], 320.f, false));
        vehicles.back()->SetTrafficLight(roadLight[1]);
        animals.push_back(new CPIGLIN(500.f, y[2], 120.f, false));
        animals.back()->SetTrafficLight(roadLight[1]);
        vehicles.push_back(new CTRAIN(100.f, y[3] - 15.f, 390.f, true));
        vehicles.back()->SetTrafficLight(roadLight[1]);
        animals.push_back(new CPIGLIN(500.f, y[3], 180.f, true));
        animals.back()->SetTrafficLight(roadLight[1]);

        // Road 2 (y[4], y[5]) - dieu khien boi den o vach 2
        animals.push_back(new CPIGLIN(500.f, y[4], 240.f, false));
        animals.back()->SetTrafficLight(roadLight[2]);
        animals.push_back(new CPIGLIN(500.f, y[4], 120.f, false));
        animals.back()->SetTrafficLight(roadLight[2]);
        vehicles.push_back(new CTRAIN(300.f, y[5], 310.f, true));
        vehicles.back()->SetTrafficLight(roadLight[2]);
        animals.push_back(new CBRUTE(900.f, y[5], 100.f, true));
        animals.back()->SetTrafficLight(roadLight[2]);

        // Road 3 (y[6], y[7]) - dieu khien boi den o vach 3
        vehicles.push_back(new CHELLMOTOR(150.f, y[6], 300.f, false));
        vehicles.back()->SetTrafficLight(roadLight[3]);
        animals.push_back(new CPIGLIN(150.f, y[6], 140.f, false));
        animals.back()->SetTrafficLight(roadLight[3]);
        vehicles.push_back(new CHELLMOTOR(150.f, y[7], 170.f, true));
        vehicles.back()->SetTrafficLight(roadLight[3]);
        animals.push_back(new CBRUTE(700.f, y[7], 75.f, true));
        animals.back()->SetTrafficLight(roadLight[3]);
        break;
    }

    case 3: // Sky — chỉ có thú vật (chim/mây/thiên thần)
    {
        const float* y = SKY_LANE_Y;

        // ===== ADDED: giong City - map Sky co 4 road (moi road 2 lane,
        // chia boi vach ke), giua cac road la 1 dai may an toan (tong 3
        // dai). Moi dai 1 den dieu khien CA ROAD (2 lane) NGAY DUOI no.
        // Road0 (tren cung, gan vach dich) khong co den.
        float medianY[3] =
        {
            120.f,   // dai giua road0 va road1 -> dieu khien road1
            255.f,   // dai giua road1 va road2 -> dieu khien road2
            380.f    // dai giua road2 va road3 -> dieu khien road3
        };

        // Thoi gian do/xanh rieng cho tung den - chinh o day de doi nhip
        float medianRedDur[3] = { 3.f, 4.f, 5.f };
        float medianGreenDur[3] = { 5.f, 4.f, 3.f };

        TrafficLight* roadLight[4] = { nullptr, nullptr, nullptr, nullptr };
        for (int i = 0; i < 3; ++i)
        {
            TrafficLight* tl = new TrafficLight(50.f, medianY[i], medianRedDur[i], medianGreenDur[i]);
            tl->loadTexture(LIGHT_PATHS[3]);
            lights.push_back(tl);
            roadLight[i + 1] = tl; // den thu i dieu khien road (i+1)
        }

        // Road 0 (y[0], y[1]) - khong den, luon bay tu do
        animals.push_back(new CBIRD(700.f, y[0], 380.f, false));
        animals.back()->SetTrafficLight(roadLight[0]);
        animals.push_back(new CANGLE(700.f, y[0], 210.f, false));
        animals.back()->SetTrafficLight(roadLight[0]);
        animals.push_back(new CCLOUD(1000.f, y[1], 150.f, true));
        animals.back()->SetTrafficLight(roadLight[0]);
        animals.push_back(new CBIRD(700.f, y[1], 350.f, true));
        animals.back()->SetTrafficLight(roadLight[0]);

        // Road 1 (y[2], y[3]) - dieu khien boi den o dai 1
        animals.push_back(new CBIRD(300.f, y[2], 150.f, false));
        animals.back()->SetTrafficLight(roadLight[1]);
        animals.push_back(new CANGLE(300.f, y[2], 230.f, false));
        animals.back()->SetTrafficLight(roadLight[1]);
        animals.push_back(new CANGLE(300.f, y[3], 160.f, true));
        animals.back()->SetTrafficLight(roadLight[1]);
        animals.push_back(new CCLOUD(1600.f, y[3], 95.f, true));
        animals.back()->SetTrafficLight(roadLight[1]);

        // Road 2 (y[4], y[5]) - dieu khien boi den o dai 2
        animals.push_back(new CBIRD(150.f, y[4], 400.f, false));
        animals.back()->SetTrafficLight(roadLight[2]);
        animals.push_back(new CANGLE(800.f, y[4], 200.f, false));
        animals.back()->SetTrafficLight(roadLight[2]);
        animals.push_back(new CBIRD(150.f, y[5], 260.f, true));
        animals.back()->SetTrafficLight(roadLight[2]);
        animals.push_back(new CANGLE(800.f, y[5], 120.f, true));
        animals.back()->SetTrafficLight(roadLight[2]);

        // Road 3 (y[6], y[7]) - dieu khien boi den o dai 3
        animals.push_back(new CBIRD(1500.f, y[6], 380.f, false));
        animals.back()->SetTrafficLight(roadLight[3]);
        animals.push_back(new CANGLE(1500.f, y[6], 200.f, false));
        animals.back()->SetTrafficLight(roadLight[3]);
        animals.push_back(new CCLOUD(1000.f, y[7], 140.f, true));
        animals.back()->SetTrafficLight(roadLight[3]);
        animals.push_back(new CBIRD(1500.f, y[7] + 5.f, 280.f, true));
        animals.back()->SetTrafficLight(roadLight[3]);
        break;
    }

    default:
        break;
    }

    // TODO (Bước 5): spawn traffic lights theo currentMap
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

    // ===== ADDED (Bước 4) =====
    if (CheckCollision())
        OnDeath();
}

//==================================================
// Draw
//==================================================

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

    // TODO (main.cpp / Bước sau): sau khi animation chết chạy xong,
    // hiển thị GameOverMenu hoặc quay về MainMenu, hiện score...
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

    // TODO (nâng cao, không bắt buộc): tăng speed cua vehicles/animals
    // theo level de tang do kho moi lan qua man
}

//==================================================
// Save/Load — Bước 6
//==================================================

void CGAME::SaveGame(const std::string& path)
{
    // TODO (Bước 6): ghi level/score/currentMap ra file tại path
    (void)path;
}

void CGAME::LoadGame(const std::string& path)
{
    // TODO (Bước 6): đọc level/score/currentMap từ file tại path
    (void)path;
}