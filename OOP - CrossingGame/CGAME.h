// CGAME.h
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "CPEOPLE.h"
#include "CVEHICLE.h"
#include "CANIMAL.h"
#include "TrafficLight.h"
#include "RollingRockManager.h"   // ===== ADDED: co che canh bao + da lan (Ancient/Nightmare) =====
#include "MeteoriteManager.h"            // ===== ADDED: co che canh bao + thien thach (Hell/Nightmare) =====
#include "WindGustManager.h"             // ===== ADDED: co che gio giat (Sky/Nightmare) =====
#include "TrainManager.h"                // ===== ADDED: co che tau hoa (City/Nightmare) =====

class AudioManager;   // forward declare, chi can con tro

class CGAME {
private:
    sf::RenderWindow& mWindow;

    // Map
    sf::Texture mapTexture;
    sf::Sprite  mapSprite;
    int currentMap;      // 0=City 1=Ancient 2=Hell 3=Sky
    int characterIndex;  // luu de Save/Load / Continue biet dung nhan vat nao
    int difficultyMode;  // 0=Easy 1=Hard 2=Nightmare, dung khi Init() spawn obstacle

    // Entities
    CPEOPLE* player;
    std::vector<CVEHICLE*> vehicles;
    std::vector<CANIMAL*>  animals;
    std::vector<TrafficLight*> lights;

    // ===== ADDED: co che canh bao + da lan, chi kich hoat khi
    // currentMap==1 (Ancient) va difficultyMode==2 (Nightmare) - xem
    // CGAME::Init() =====
    RollingRockManager rockManager;

    // ===== ADDED: co che canh bao + thien thach, chi kich hoat khi
    // currentMap==2 (Hell) va difficultyMode==2 (Nightmare) - xem
    // CGAME::Init() =====
    MeteoriteManager meteoriteManager;

    // ===== ADDED: co che gio giat, chi kich hoat khi currentMap==3 (Sky)
    // va difficultyMode==2 (Nightmare) - xem CGAME::Init() =====
    WindGustManager windGustManager;

    // ===== ADDED: co che tau hoa, chi kich hoat khi currentMap==0 (City)
    // va difficultyMode==2 (Nightmare) - xem CGAME::Init() =====
    TrainManager trainManager;

    // Game state
    int  level;
    int  score;
    int  playerHP;
    int  playerMaxHP;
    bool isGameOver;
    bool isPaused;
    bool isWin;

    // ===== ADDED: level tuyet doi can dat duoc de Win. Adventure (bat dau
    // tu level 1) = WIN_LEVEL_BY_MODE[difficultyMode]. Nightmare - Custom:
    // nguoi choi tu nhap gia tri nay qua SetStartingLevel() - level VAN
    // BAT DAU TU 1 nhu Adventure, chi khac o cho moc thang la so tuy chinh
    // thay vi WIN_LEVEL_BY_MODE co dinh - xem CGAME::SetStartingLevel() va
    // CGAME::OnLevelComplete() =====
    int winTargetLevel;

    // ===== ADDED: gioi han thoi gian theo do kho (Easy = khong gioi han).
    // Het gio ma chua Win thi Game Over ngay - xem CGAME::Update() =====
    float timeRemaining;

    // Save/Load
    std::string savePath;
    std::string playerName; // optional player name stored in save

    // HUD (Score/Level/HP trong luc Playing)
    sf::Text hudScoreText;
    sf::Text hudLevelText;
    sf::Text hudHPText;
    sf::Text hudTimeText;   // ===== ADDED: chi ve neu HasTimeLimit() =====

    // Doi nhac nen theo map khi Init()
    AudioManager* audio = nullptr;

public:
    CGAME(sf::RenderWindow& window);
    ~CGAME();

    void Init(int mapIndex, int characterIndex);
    void Update(float dt);
    void Draw();
    void HandleInput(sf::Event& event);

    // Goi TRUOC Init() de chon do kho cho lan choi nay. Neu khong goi,
    // giu nguyen gia tri cu (mac dinh Hard=1) - nen Retry/Restart/PlayAgain
    // tu dong giu nguyen do kho da chon
    void SetDifficultyMode(int mode) { difficultyMode = mode; }
    int GetDifficultyMode() const { return difficultyMode; }

    // ===== CHANGED: goi SAU Init() de ghi de MOC LEVEL CAN VUOT QUA DE WIN
    // (khong con la level bat dau nua - level luon bat dau tu 1 nhu
    // Adventure, tang dan +1 tung level). Dung cho Nightmare - Custom
    // (xem ModeSelection::getCustomStartLevel()). Phai goi SAU Init() vi
    // Init() da tinh san moc thang mac dinh theo WIN_LEVEL_BY_MODE, ham
    // nay se ghi de lai. Score KHONG bi anh huong - van bat dau tu 0 va
    // +100 moi lan qua level nhu binh thuong (xem OnLevelComplete()).
    // Cung tat luon gioi han thoi gian (timeRemaining = -1) vi ham nay
    // chi duoc goi rieng cho Nightmare - Custom =====
    void SetStartingLevel(int targetLevel);

    // ===== ADDED: goi lai SAU MOI LAN Init() (Retry/Restart/PlayAgain)
    // de "nho" lai nguoi choi dang o Nightmare-Custom hay khong, tranh
    // bi tuot ve Adventure (mat moc thang da nhap) sau khi choi lai.
    // main.cpp nen goi ham nay ngay sau moi lan goi Init() thay vi chi
    // goi SetStartingLevel() 1 lan duy nhat =====
    void ReapplyCustomNightmare(bool isCustom, int targetLevel);

    // Nap font cho HUD (goi 1 lan sau khi tao CGAME)
    void SetFont(const sf::Font& font);

    // De Init() tu doi sang nhac nen rieng cua tung map
    void SetAudioManager(AudioManager* manager);

    bool CheckCollision();
    bool CheckFinish();
    void OnDeath();
    void OnHit();   // mat 1 tim (chua het HP) - reset vi tri, chua Game Over
    void OnLevelComplete();

    void SaveGame(const std::string& path);
    bool LoadGame(const std::string& path);

    // New API: save slots (0..3)
    static const std::string& GetSavePathForSlot(int slot);
    // Peek full save data
    struct SaveData
    {
        bool exists = false;
        int characterIndex = 0;
        std::string playerName;
        int mapIndex = 0;
        int score = 0;
        int difficultyMode = 1;
        int level = 1;
        std::string saveTime;
    };
    static bool PeekSaveData(const std::string& path, SaveData& out);

    // Static vi khong can 1 instance CGAME song - dung de ContinueMenu
    // liet ke cac save co san ma khong phai load han.
    // ===== CHANGED: moi map GIO co 3 file save rieng theo difficultyMode
    // (0=Easy 1=Hard 2=Nightmare), thay vi 1 file save chung cho ca map =====
    static const std::string& GetSavePathForMap(int mapIndex, int mode);
    static bool PeekSaveInfo(const std::string& path,
        int& outMap, int& outCharacter, int& outLevel, int& outScore,
        int& outMode);

    void Pause() { isPaused = true; }
    void Resume() { isPaused = false; }
    bool IsGameOver() const { return isGameOver; }
    bool IsPaused() const { return isPaused; }
    bool IsWin() const { return isWin; }

    int GetScore() const { return score; }
    int GetLevel() const { return level; }
    int GetPlayerHP() const { return playerHP; }
    int GetPlayerMaxHP() const { return playerMaxHP; }

    // ===== ADDED: HasTimeLimit()==false khi dang Easy (khong gioi han
    // thoi gian) - main.cpp/HUD dua vao day de biet co hien dong ho khong =====
    bool HasTimeLimit() const { return timeRemaining >= 0.f; }
    float GetTimeRemaining() const { return timeRemaining; }

    // De main.cpp dong bo lai lua chon map/nhan vat sau khi Continue
    // (LoadGame), va biet duong dan file save
    int GetCurrentMap() const { return currentMap; }
    int GetCharacterIndex() const { return characterIndex; }
    const std::string& GetSavePath() const { return savePath; }
};
