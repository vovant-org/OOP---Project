// WindGustManager.h
// Co che hazard rieng cho Sky map + Nightmare mode:
//   Idle (cho cooldown 8s) -> Warning (1.5s, WindSign nhap nhay o canh
//   man hinh, MOI luong gio chi xuat hien tai dung khoang Y cua road ma
//   no se thoi) -> Active (~1.8s, 2 frame gio cuon qua dung road do +
//   day nhan vat troi ngang lien tuc NEU nhan vat dang dung trong road
//   do, KHONG khoa di chuyen) -> Idle
//
// Moi lan spawn se co 1-3 luong gio doc lap (khong nhat thiet cung huong),
// moi luong gan voi dung 1 trong 4 road cua Sky map va TU CHON huong
// trai/phai rieng, khong bi anh huong boi cac luong khac.
#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class CPEOPLE;
class AudioManager;   // ===== ADDED: forward-declare, khong include AudioManager.h o day

class WindGustManager
{
private:

    enum class Phase { Idle, Warning, Active };

    // 1 luong gio doc lap: gan voi 1 road cu the (0..3, xem ROAD_TOP/
    // ROAD_BOTTOM trong .cpp) + huong rieng cua no
    struct GustInstance
    {
        int  roadIndex;
        bool blowsRight;
    };

    sf::Texture windTexture1;
    sf::Texture windTexture2;
    sf::Texture signTexture;

    // ===== CHANGED: khong tu giu sf::SoundBuffer/sf::Sound rieng nua -
    // phat qua AudioManager (playControlledSound/stopControlledSound, vi
    // can cat am dung luc ACTIVE_DURATION ket thuc) =====
    AudioManager* audio = nullptr;

    int windWidth = 0, windHeight = 0;
    int signFrameWidth = 0, signFrameHeight = 0;

    bool active = false;   // chi bat khi dung Sky + Nightmare

    Phase phase = Phase::Idle;
    float phaseTimer = 0.f;
    float cooldownTimer = 0.f;

    float windFrameTimer = 0.f;
    int   windFrame = 0;          // 0 hoac 1, xen ke frame1/frame2

    // ===== CHANGED: thay vi 1 huong gio duy nhat cho toan man hinh, gio
    // la 1 danh sach 1-3 luong doc lap, moi luong 1 road + 1 huong rieng =====
    std::vector<GustInstance> gusts;

    float canvasW = 1280.f;
    float canvasH = 720.f;

    void StartWarning();
    void StartActive();
    void PickGusts();   // chon 1-3 road ngau nhien (khong trung), moi road 1 huong ngau nhien rieng

public:

    WindGustManager();

    bool LoadTextures(const std::string& windPath1, const std::string& windPath2,
        const std::string& signPath);

    // ===== ADDED: gan AudioManager dung chung cua game - PHAI goi TRUOC
    // LoadSound() =====
    void SetAudioManager(AudioManager* manager);

    // ===== CHANGED: gio nap am thanh vao AudioManager thay vi tu giu
    // buffer rieng - can goi SetAudioManager() truoc =====
    bool LoadSound(const std::string& gustSoundPath);

    void SetActive(bool isActive);
    void Reset();   // goi khi CGAME::Init() mot map moi

    void Update(float dt, CPEOPLE* player);
    void Draw(sf::RenderWindow& window) const;
};