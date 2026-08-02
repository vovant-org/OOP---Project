// CGAME.h
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "CPEOPLE.h"
#include "CVEHICLE.h"
#include "CANIMAL.h"
#include "TrafficLight.h"

class AudioManager;   // ===== ADDED: forward declare, chi can con tro =====

class CGAME {
private:
    sf::RenderWindow& mWindow;

    // Map
    sf::Texture mapTexture;
    sf::Sprite  mapSprite;
    int currentMap;   // 0=City 1=Ancient 2=Hell 3=Sky
    int characterIndex; // ===== ADDED (Bước 6): luu de Save/Load / Continue biet dung nhan vat nao =====

    // Entities
    CPEOPLE* player;
    std::vector<CVEHICLE*> vehicles;
    std::vector<CANIMAL*>  animals;
    std::vector<TrafficLight*> lights;

    // Game state
    int  level;
    int  score;
    bool isGameOver;
    bool isPaused;
    bool isWin;   // ===== ADDED =====

    // Save/Load
    std::string savePath;

    // ===== ADDED: HUD hien Score/Level trong luc Playing =====
    sf::Text hudScoreText;
    sf::Text hudLevelText;

    // ===== ADDED: doi nhac nen theo map khi Init() =====
    AudioManager* audio = nullptr;

public:
    CGAME(sf::RenderWindow& window);
    ~CGAME();

    void Init(int mapIndex, int characterIndex);
    void Update(float dt);
    void Draw();
    void HandleInput(sf::Event& event);

    // ===== ADDED: nap font cho HUD (goi 1 lan sau khi tao CGAME) =====
    void SetFont(const sf::Font& font);

    // ===== ADDED: de Init() tu doi sang nhac nen rieng cua tung map =====
    void SetAudioManager(AudioManager* manager);

    bool CheckCollision();
    bool CheckFinish();
    void OnDeath();
    void OnLevelComplete();

    void SaveGame(const std::string& path);
    bool LoadGame(const std::string& path);   // ===== CHANGED: tra ve true/false =====

    // ===== ADDED (Bước 6, sửa lại): static vì không cần 1 instance CGAME
    // sống - dùng để ContinueMenu liệt kê các save có sẵn mà không phải
    // load hẳn (không đụng vào state của bất kỳ CGAME nào đang chạy) =====
    static const std::string& GetSavePathForMap(int mapIndex);
    static bool PeekSaveInfo(const std::string& path,
        int& outMap, int& outCharacter, int& outLevel, int& outScore);

    void Pause() { isPaused = true; }
    void Resume() { isPaused = false; }
    bool IsGameOver() const { return isGameOver; }
    bool IsPaused() const { return isPaused; }   // ===== ADDED =====
    bool IsWin() const { return isWin; }         // ===== ADDED =====

    // ===== ADDED: de GameOverMenu/HUD hien thi diem/level =====
    int GetScore() const { return score; }
    int GetLevel() const { return level; }

    // ===== ADDED (Bước 6): de main.cpp dong bo lai lua chon map/nhan vat
    // sau khi Continue (LoadGame), va biet duong dan file save =====
    int GetCurrentMap() const { return currentMap; }
    int GetCharacterIndex() const { return characterIndex; }
    const std::string& GetSavePath() const { return savePath; }
};