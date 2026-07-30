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
    const float CITY_LANE_Y[] = { 180.f, 300.f, 420.f, 540.f };
    const float ANCIENT_LANE_Y[] = { 180.f, 300.f, 420.f, 540.f };
    const float HELL_LANE_Y[] = { 220.f, 400.f, 550.f };
    const float SKY_LANE_Y[] = { 180.f, 300.f, 420.f, 540.f };
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

    //----------------------------------
    // ===== ADDED (Bước 3): spawn vehicles/animals theo currentMap =====
    //----------------------------------

    for (auto* v : vehicles)
        delete v;
    vehicles.clear();

    for (auto* a : animals)
        delete a;
    animals.clear();

    switch (currentMap)
    {
    case 0: // City — chỉ có xe cộ
    {
        const float* y = CITY_LANE_Y;
        vehicles.push_back(new CBIKE(1100.f, y[0], 180.f, true));
        vehicles.push_back(new CTRUCK(1000.f, y[0], 280.f, true));

        vehicles.push_back(new CMOTOR(300.f, y[1], 350.f, false));
        vehicles.push_back(new CTRUCK(1000.f, y[1], 240.f, false));

        vehicles.push_back(new CBIKE(150.f, y[2], 160.f, true));
        vehicles.push_back(new CMOTOR(800.f, y[2], 300.f, true));

        vehicles.push_back(new CTRUCK(400.f, y[3], 275.f, false));
        vehicles.push_back(new CBIKE(1100.f, y[3] + 28.f, 210.f, false));
        break;
    }

    case 1: // Ancient — chỉ có thú vật
    {
        const float* y = ANCIENT_LANE_Y;
        animals.push_back(new CCROCODILE(100.f, y[0], 80.f, true));
        animals.push_back(new CDINOSAUR(700.f, y[0], 60.f, true));

        animals.push_back(new COLDBIRD(300.f, y[1], 150.f, false));
        animals.push_back(new CBEAST(1000.f, y[1], 90.f, false));

        animals.push_back(new CCROCODILE(150.f, y[2], 85.f, true));
        animals.push_back(new CBEAST(800.f, y[2], 95.f, true));

        animals.push_back(new CDINOSAUR(400.f, y[3], 65.f, false));
        animals.push_back(new COLDBIRD(1100.f, y[3], 160.f, false));
        break;
    }

    case 2: // Hell — cả xe cộ lẫn thú vật (ít lane hơn, platform to hơn)
    {
        const float* y = HELL_LANE_Y;
        vehicles.push_back(new CHELLMOTOR(100.f, y[0], 180.f, true));
        animals.push_back(new CPIGLIN(500.f, y[0], 80.f, true));

        vehicles.push_back(new CTRAIN(300.f, y[1], 250.f, false));
        animals.push_back(new CBRUTE(900.f, y[1], 70.f, false));

        vehicles.push_back(new CHELLMOTOR(150.f, y[2], 170.f, true));
        animals.push_back(new CBRUTE(700.f, y[2], 75.f, true));
        break;
    }

    case 3: // Sky — chỉ có thú vật (chim/mây/thiên thần)
    {
        const float* y = SKY_LANE_Y;
        animals.push_back(new CCLOUD(1000.f, y[0], 100.f, true));
        animals.push_back(new CBIRD(700.f, y[0], 180.f, true));

        animals.push_back(new CANGLE(300.f, y[1], 150.f, false));
        animals.push_back(new CCLOUD(1600.f, y[1], 95.f, false));

        animals.push_back(new CBIRD(150.f, y[2], 160.f, true));
        animals.push_back(new CANGLE(800.f, y[2], 130.f, true));

        animals.push_back(new CCLOUD(1000.f, y[3], 100.f, false));
        animals.push_back(new CBIRD(1500.f, y[3], 160.f, false));
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

    // TODO (Bước 4): sau khi move xong, gọi CheckCollision() ngay tại đây
    // (crossing game kiểu Frogger thường check va chạm ngay sau mỗi bước
    // đi, không đợi tới Update())
}

//==================================================
// Update
//==================================================

void CGAME::Update(float dt)
{
    if (isPaused || isGameOver)
        return;

    if (player)
        player->Update(dt);

    // ===== ADDED (Bước 3) =====
    for (auto* v : vehicles)
        v->Update(dt);

    for (auto* a : animals)
        a->Update(dt);

    // TODO (Bước 4): CheckCollision()
    // TODO (Bước 5): update lights, CheckFinish()
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
    // TODO (Bước 4): so GetBoundingBox() cua player voi tung vehicle/animal
    return false;
}

bool CGAME::CheckFinish()
{
    // TODO (Bước 5): player toi hang tren cung -> true
    return false;
}

void CGAME::OnDeath()
{
    // TODO (Bước 4): player->SetDead(), isGameOver = true, phát âm thanh...
}

void CGAME::OnLevelComplete()
{
    // TODO (Bước 5): level++, tăng score, reset player, tăng độ khó...
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