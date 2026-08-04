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

    // Game state
    int  level;
    int  score;
    int  playerHP;
    int  playerMaxHP;
    bool isGameOver;
    bool isPaused;
    bool isWin;

    // Save/Load
    std::string savePath;

    // HUD (Score/Level/HP trong luc Playing)
    sf::Text hudScoreText;
    sf::Text hudLevelText;
    sf::Text hudHPText;

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

    // Static vi khong can 1 instance CGAME song - dung de ContinueMenu
    // liet ke cac save co san ma khong phai load han
    static const std::string& GetSavePathForMap(int mapIndex);
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

    // De main.cpp dong bo lai lua chon map/nhan vat sau khi Continue
    // (LoadGame), va biet duong dan file save
    int GetCurrentMap() const { return currentMap; }
    int GetCharacterIndex() const { return characterIndex; }
    const std::string& GetSavePath() const { return savePath; }
};