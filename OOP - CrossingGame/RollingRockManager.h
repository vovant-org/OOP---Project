// RollingRockManager.h
// Co che hazard rieng cho Ancient map + Nightmare mode:
//   Idle (cho cooldown) -> Warning (2s, mui ten nhap nhay tuan tu) ->
//   Rolling (2-4 hon da lan tu tren xuong duoi map) -> Idle (cooldown 10s)
#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class CPEOPLE;
class AudioManager;   // ===== ADDED: forward-declare, khong include AudioManager.h o day

class RollingRockManager
{
private:

    enum class Phase { Idle, Warning, Rolling };

    struct Rock
    {
        float x, y;
        float speed;
        int   frame;
        float frameTimer;
        bool  hasHitPlayer;   // tranh choang lap lai boi cung 1 hon da
    };

    sf::Texture rockTexture;
    sf::Texture signTexture;

    // ===== CHANGED: khong tu giu sf::SoundBuffer/sf::Sound rieng nua -
    // phat qua AudioManager (chung 1 keo Sound volume trong SettingMenu) =====
    AudioManager* audio = nullptr;

    int rockFrameWidth = 0, rockFrameHeight = 0;
    static constexpr int ROCK_FRAME_COUNT = 5;

    int signFrameWidth = 0, signFrameHeight = 0;

    bool active = false;   // chi bat khi dung Ancient + Nightmare

    Phase phase = Phase::Idle;
    float phaseTimer = 0.f;
    float cooldownTimer = 0.f;

    std::vector<float> warningColumns;   // x cua tung cot canh bao/lan da
    std::vector<Rock>  rocks;

    float canvasW = 1280.f;
    float canvasH = 720.f;

    void StartWarning();
    void StartRolling();
    void PickColumns();

public:

    RollingRockManager();

    bool LoadTextures(const std::string& rockPath, const std::string& signPath);

    // ===== ADDED: gan AudioManager dung chung cua game - PHAI goi TRUOC
    // LoadSound() =====
    void SetAudioManager(AudioManager* manager);

    // ===== CHANGED: gio nap am thanh vao AudioManager thay vi tu giu
    // buffer rieng - can goi SetAudioManager() truoc =====
    bool LoadSound(const std::string& rollSoundPath);

    void SetActive(bool isActive);
    void Reset();   // goi khi CGAME::Init() mot map moi

    void Update(float dt, CPEOPLE* player);
    void Draw(sf::RenderWindow& window) const;
};