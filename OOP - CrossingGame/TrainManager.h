// TrainManager.h
// Co che hazard rieng cho City map + Nightmare mode:
//   Idle (cho cooldown) -> Warning (2s, TrainSign nhap nhay + Rail hien
//   dan tu 1 canh road) -> Running (8s, tau chay ngang qua road) -> Idle
//
// Trong ca Warning lan Running cua 1 road: obstacles (vehicles/animals)
// tren road do bien mat (CGAME an Draw + bo qua va cham), spawn lai binh
// thuong ngay sau khi Running ket thuc.
//
// Va cham voi player chia 2 truong hop:
//   - Player DUNG SAN tren road, tau chay toi va cham trung -> mat 2 HP
//     (xem ConsumePlayerHit(), CGAME tru HP giong Meteorite).
//   - Player CO Y DINH di chuyen TU NGOAI VAO road dang Running -> khong
//     mat HP, chi bi day lui + choang 1 giay (xem TryBlockEntry(), goi tu
//     CGAME::HandleInput truoc khi Move...() that su).
#pragma once

#include <SFML/Graphics.hpp>
#include <string>

class CPEOPLE;
class AudioManager;   // ===== ADDED: forward-declare, khong include AudioManager.h o day

class TrainManager
{
public:
    // City map co 4 road (moi road = 2 lane vehicle), xem CITY_LANE_Y
    // trong CGAME.cpp - CGAME tinh toa do Y TAM cua tung road roi truyen
    // vao qua SetRoadCenters()
    static constexpr int ROAD_COUNT = 4;

    TrainManager();

    // headPath/bodyPath: 2 frame cua ModernTrain (dau tau + toa than,
    // dau tau dung lai o dau VA cuoi doan tau, lat ngang o dau cuoi).
    // railPath: duong ray hien dan trong luc canh bao.
    // signPath: bien bao TrainSign, nhap nhay trong luc canh bao.
    bool LoadTextures(const std::string& headPath, const std::string& bodyPath,
        const std::string& railPath, const std::string& signPath);

    // ===== ADDED: gan AudioManager dung chung cua game - PHAI goi TRUOC
    // LoadSounds() =====
    void SetAudioManager(AudioManager* manager);

    // ===== CHANGED: gio nap 2 am thanh vao AudioManager - ca signSoundPath
    // (coi bao luc Warning) va trainSoundPath (tieng tau chay luc Running)
    // deu phat qua playControlledSound() (co the stop rieng theo ten) -
    // signSound dung stop dung luc StartRunning() (TrainSign bien mat),
    // trainSound loop suot Running va stop khi tau chay xong/Reset(). Can
    // goi SetAudioManager() truoc =====
    bool LoadSounds(const std::string& signSoundPath, const std::string& trainSoundPath);

    void SetActive(bool isActive);
    bool IsActive() const { return active; }

    // Toa do Y TAM cua 4 road tren City map, theo thu tu road0..road3
    // (goi 1 lan trong CGAME::Init, TRUOC SetActive(true))
    void SetRoadCenters(const float centers[ROAD_COUNT]);

    // ===== ADDED: chinh chieu cao hien thi cua Rail (px), doc lap voi
    // ROAD_BAND_HEIGHT (vung va cham/an obstacle van giu nguyen, chi anh
    // hien thi cua duong ray to/nho theo gia tri nay). Mac dinh =
    // ROAD_BAND_HEIGHT neu khong goi. Rail luon can giua theo centerY
    // cua road. Co the goi bat ky luc nao (truoc hoac trong luc choi).
    void SetRailHeight(float height) { if (height > 0.f) railHeight = height; }
    float GetRailHeight() const { return railHeight; }

    // ===== ADDED: dich vi tri Rail theo chieu doc (px), tinh tu tam
    // (centerY) cua road - so duong (+) dich XUONG, so am (-) dich LEN.
    // Mac dinh = 0 (Rail can dung giua road). Doc lap voi SetRailHeight(),
    // co the goi bat ky luc nao (truoc hoac trong luc choi).
    void SetRailYOffset(float offset) { railYOffset = offset; }
    float GetRailYOffset() const { return railYOffset; }

    // ===== ADDED: he so nhan toc do ModernTrain (mac dinh = 1.0). Vi du
    // 1.5f = nhanh hon 50%, 0.5f = cham hon 50%. Doi ngay ca khi tau
    // dang chay (Phase::Running) - ap dung tu lan StartRunning() ke tiep;
    // neu muon thay doi tuc thi giua luc dang chay, xem
    // ApplySpeedMultiplierNow() ben duoi. Gia tri <= 0 se bi bo qua.
    void SetTrainSpeedMultiplier(float multiplier);
    float GetTrainSpeedMultiplier() const { return trainSpeedMultiplier; }

    // ===== ADDED: neu dang o Phase::Running, ap dung ngay he so toc do
    // MOI NHAT (da SetTrainSpeedMultiplier truoc do) vao trainSpeed hien
    // tai va tinh lai thoi gian con lai cho vua khop quang duong con lai
    // - dung khi muon tang/giam toc do tau tuc thi (vd nguoi choi bam
    // phim +/- trong luc tau dang chay). Khong lam gi neu dang Idle/Warning.
    void ApplySpeedMultiplierNow();

    // Dua ve trang thai ban dau (Idle, cooldown day du, khong road nao
    // dang hoat dong) - goi khi CGAME::Init() lai (doi map/choi lai)
    void Reset();

    void Update(float dt, CPEOPLE* player);
    void Draw(sf::RenderWindow& window) const;

    // true neu toa do y dang thuoc 1 road dang co su kien (Warning HOAC
    // Running) - CGAME dung de AN (khong Draw + khong tinh va cham)
    // vehicles/animals dang nam tren road do
    bool IsRoadActive(float y) const;

    // true neu toa do y dang thuoc road dang o pha Running (tau that su
    // dang chay, khac voi Warning chi la canh bao)
    bool IsRoadRunning(float y) const;

    // Goi tu CGAME::HandleInput TRUOC KHI Move...() that su: neu player
    // dang o currentY (ngoai road) va co dinh di chuyen toi nextY (roi
    // vao 1 road dang Running), ham se tu dong TriggerPushback() de day
    // player lui lai + choang 1 giay, roi tra ve true (CGAME nen BO QUA
    // Move...() lan nay). Neu khong roi vao truong hop do, tra ve false
    // va CGAME cu Move...() binh thuong.
    bool TryBlockEntry(CPEOPLE* player, float currentY, float nextY) const;

    // Tra ve true DUY NHAT 1 LAN moi khi tau (dang Running) thuc su cham
    // (overlap) voi player dang dung san tren road - CGAME nen tru 2 HP
    // (thay vi 1 nhu va cham xe thuong) khi nhan true. Tu dong reset ve
    // false ngay sau khi doc, giong ConsumePlayerHit() cua MeteoriteManager.
    bool ConsumePlayerHit();

private:
    enum class Phase { Idle, Warning, Running };

    struct RoadInfo
    {
        float centerY = 0.f;
    };

    void PickRoadAndSide();
    void StartWarning();
    void StartRunning();

    bool active = false;

    Phase phase = Phase::Idle;
    float phaseTimer = 0.f;
    float cooldownTimer = 0.f;

    RoadInfo roads[ROAD_COUNT];

    int  activeRoad = -1;   // road dang dien ra su kien, -1 = khong co
    bool fromLeft = true;   // huong xuat hien canh bao / huong tau di toi

    float trainX = 0.f;        // mep TRAI cua toan bo doan tau (world space)
    float trainSpeed = 0.f;    // px/s, co dau (am = chay sang trai)
    float trainLength = 0.f;   // tong chieu dai doan tau da scale (px)

    // ===== ADDED: thoi luong THUC TE cua pha Running cho luot chay hien
    // tai (giay) - tinh lai moi lan StartRunning()/ApplySpeedMultiplierNow()
    // dua tren trainSpeed thuc te, thay vi dung thang hang so RUNNING_DURATION,
    // de tau luon vua di het man hinh khi phaseTimer cham moc nay (bat ke
    // trainSpeedMultiplier dang la bao nhieu)
    float runningDuration = 0.f;

    bool trainHasHitPlayer = false;   // tranh tinh va cham lap lai trong 1 luot chay
    bool pendingPlayerHit = false;    // co du cho ConsumePlayerHit()

    // ===== ADDED: chieu cao hien thi cua Rail (px). MUON TU CHINH: sua
    // truc tiep con so 110.f o day. Gia tri nay PHAI KHOP voi
    // ROAD_BAND_HEIGHT khai bao trong TrainManager.cpp (khong the tham
    // chieu truc tiep vi ROAD_BAND_HEIGHT nam trong namespace an danh
    // cua file .cpp) - neu doi ROAD_BAND_HEIGHT ben do thi nen doi luon
    // so nay cho dong bo. Ngoai ra co the doi luc runtime qua SetRailHeight().
    float railHeight = 330.f;

    // ===== ADDED: do lech vi tri Rail theo chieu doc so voi centerY cua
    // road. MUON TU CHINH: sua truc tiep con so nay, hoac goi SetRailYOffset()
    // luc runtime. Giong railHeight/trainSpeedMultiplier, KHONG bi Reset().
    float railYOffset = 10.f;

    // ===== ADDED: he so nhan toc do ModernTrain. MUON TU CHINH: sua
    // truc tiep con so 1.f o day (vd 1.5f = nhanh hon 50%, 0.6f = cham
    // hon 40%). Ngoai ra co the doi luc runtime qua SetTrainSpeedMultiplier().
    float trainSpeedMultiplier = 1.8f;

    sf::Texture headTexture;
    sf::Texture bodyTexture;
    sf::Texture railTexture;
    sf::Texture signTexture;

    // ===== CHANGED: khong tu giu sf::SoundBuffer/sf::Sound rieng nua -
    // phat qua AudioManager (chung 1 keo Sound volume trong SettingMenu) =====
    AudioManager* audio = nullptr;

    int headTexW = 0, headTexH = 0;
    int bodyTexW = 0, bodyTexH = 0;
    int railTexW = 0, railTexH = 0;
    int signTexW = 0, signTexH = 0;

    // ===== ADDED: hop gioi han (bounding box) cua vung PIXEL KHONG TRONG
    // SUOT thuc su ben trong tung texture dau tau/than tau. 2 file
    // ModernTrain_frame1/2.png co le trong (canvas rong hon hinh ve that
    // su), neu dung nguyen headTexW/bodyTexW de xep hang thi cac toa se
    // bi hut mot khoang trong -> nhin nhu "tach xa nhau". Tinh 1 lan luc
    // LoadTextures() va dung contentRect nay (thay vi ca texture) de vua
    // scale vua ve, giup cac toa dinh sat nhau.
    sf::IntRect headContentRect;
    sf::IntRect bodyContentRect;

    // Tinh bounding box cua cac pixel co alpha > 0 trong 1 sf::Texture.
    // Neu texture rong/toan trong suot, tra ve nguyen ca texture (fallback).
    static sf::IntRect ComputeOpaqueBounds(const sf::Texture& tex);

    float canvasW = 1280.f;
    float canvasH = 720.f;
};