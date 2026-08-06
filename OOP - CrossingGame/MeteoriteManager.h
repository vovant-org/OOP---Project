// MeteoriteManager.h
// Co che hazard rieng cho Hell map + Nightmare mode:
//   Idle (cho cooldown 5s) -> Warning (2s, MeteoriteSign nhap nhay tai
//   2-4 vi tri ngau nhien) -> Active (tung Meteorite roi tu tren cung map
//   xuong dung vi tri sign vua bien mat, cham dat thi gay damage 1 lan
//   neu player dang dung do, roi bien thanh "ho da" chan duong 5s) -> Idle
#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class CPEOPLE;
class AudioManager;   // ===== ADDED: chi can forward-declare, khong include AudioManager.h o day

class MeteoriteManager
{
private:

    enum class Phase { Idle, Warning, Active };

    // Trang thai rieng cua tung Meteorite dang hoat dong (co the co
    // nhieu Meteorite cung luc, moi cai o 1 giai doan khac nhau)
    enum class MetState { Falling, Impact, Landed };

    struct MeteoriteInstance
    {
        float targetX, targetY;   // vi tri dich = vi tri MeteoriteSign vua bien mat
        float y;                  // vi tri Y hien tai (Falling); == targetY khi da roi trung
        int   frame;              // 0..2 luc Falling, 3 luc Impact, 4 luc Landed
        float frameTimer;
        MetState state;
        float stateTimer;         // dem thoi gian cho Impact/Landed
        bool  hasDamagedPlayer;   // chi kiem tra damage 1 lan duy nhat luc vua cham dat
    };

    sf::Texture meteoriteTexture;   // spritesheet doc, 5 frame
    sf::Texture signTexture;        // 1 anh tinh (mui ten canh bao)

    // ===== CHANGED: khong tu giu sf::SoundBuffer/sf::Sound rieng nua -
    // phat qua AudioManager (chung 1 keo Sound volume trong SettingMenu)
    // de am luong SFX tang/giam theo dung slider "Sound" =====
    AudioManager* audio = nullptr;

    int meteoriteFrameWidth = 0, meteoriteFrameHeight = 0;
    static constexpr int METEORITE_FRAME_COUNT = 5;

    int signFrameWidth = 0, signFrameHeight = 0;

    bool active = false;   // chi bat khi dung Hell + Nightmare

    Phase phase = Phase::Idle;
    float phaseTimer = 0.f;
    float cooldownTimer = 0.f;

    std::vector<sf::Vector2f> warningSpots;        // vi tri (x,y) tung MeteoriteSign dang canh bao
    std::vector<MeteoriteInstance> meteorites;      // Meteorite dang Falling/Impact/Landed

    // ===== co bao cho CGAME biet player vua bi Meteorite roi trung -
    // CGAME doc qua ConsumePlayerHit() moi frame roi tu reset ve false =====
    bool pendingHit = false;

    float canvasW = 1280.f;
    float canvasH = 720.f;

    void StartWarning();
    void StartFalling();
    void PickSpawnSpots();

public:

    MeteoriteManager();

    bool LoadTextures(const std::string& meteoritePath, const std::string& signPath);

    // ===== ADDED: gan AudioManager dung chung cua game - PHAI goi TRUOC
    // LoadSound() (LoadSound se nap am thanh vao chinh AudioManager nay) =====
    void SetAudioManager(AudioManager* manager);

    // ===== CHANGED: gio nap am thanh vao AudioManager (qua SetAudioManager)
    // thay vi tu giu buffer rieng - can goi SetAudioManager() truoc. Neu
    // chua co AudioManager thi bao loi va tra ve false, Update() van chay
    // binh thuong (chi la im lang luc cham dat) =====
    bool LoadSound(const std::string& impactSoundPath);

    void SetActive(bool isActive);
    void Reset();   // goi khi CGAME::Init() mot map moi

    void Update(float dt, CPEOPLE* player);
    void Draw(sf::RenderWindow& window) const;

    // CGAME goi ngay sau Update() de biet co can tru mau player khong.
    // Tra ve true toi da 1 lan cho moi lan Meteorite cham dat trung player.
    bool ConsumePlayerHit();

    // CGAME goi TRUOC khi thuc su di chuyen player (xem CPEOPLE::PeekNextPosition)
    // de biet o (x,y) sap toi co dang la "ho da" (Landed, con hieu luc 5s) hay khong
    bool IsPositionBlocked(float x, float y) const;
};
