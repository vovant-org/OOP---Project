// CPEOPLE.h
#pragma once
#include <SFML/Graphics.hpp>

class CPEOPLE {
private:
    float x, y;
    bool  isAlive;

    sf::Texture texture;
    sf::Sprite  sprite;

    int currentFrame;
    int direction;   // 0=UP 1=DOWN 2=LEFT 3=RIGHT 4=DIE
    int frameWidth, frameHeight;
    float frameTime, elapsedTime;

    // Cooldown giua 2 lan di chuyen - the hien do "nhanh/cham" khac nhau
    // giua cac nhan vat (xem SetMoveCooldown)
    float moveCooldown;
    float moveCooldownTimer;

    // ===== ADDED: trang thai "choang" khi bi hon da (RollingRockManager)
    // va trung, HOAC bi day lui theo chieu doc (TrainManager) - khong the
    // di chuyen va bi bat/day theo bounceVelocity(X,Y), giu nguyen
    // sprite/animation hien tai (khong doi anh) =====
    bool  isStunned;
    float stunTimer;
    float bounceVelocity;    // truc X (RollingRockManager - trai/phai)
    float bounceVelocityY;   // truc Y (TrainManager - day lui len/xuong)

    // ===== ADDED: bi gio day dat (WindGustManager, Sky/Nightmare) - KHONG
    // khoa di chuyen nhu stun, chi cong them 1 luong troi ngang moi frame
    // (van bam phim di chuyen binh thuong duoc, nhung kho giu dung vi tri) =====
    float windPushVelocity;

public:
    CPEOPLE(float startX, float startY);
    bool loadTexture(const std::string& path);

    void MoveUp();
    void MoveDown();
    void MoveLeft();
    void MoveRight();

    // Thoi gian (giay) phai cho giua 2 lan di chuyen - nhan vat speed
    // cao thi truyen so nho (di lien tuc duoc), speed thap truyen so lon
    void SetMoveCooldown(float seconds) { moveCooldown = seconds; }

    void Update(float dt);
    void Draw(sf::RenderWindow& window);

    sf::FloatRect GetBoundingBox() const;
    bool IsAlive() const { return isAlive; }
    void SetDead() { isAlive = false; }
    void Reset(float startX, float startY);

    void TriggerDeath();

    // ===== ADDED: kich hoat trang thai choang 1.5 giay - khong the di
    // chuyen, bi bat ra ngau nhien sang trai (bounceRight=false) hoac
    // phai (bounceRight=true). Goi tu RollingRockManager khi va cham. =====
    void TriggerStun(bool bounceRight);
    bool IsStunned() const { return isStunned; }

    // ===== ADDED: kich hoat trang thai choang 1 giay theo TRUC DOC - dung
    // rieng cho TrainManager khi player co tinh di chuyen VAO 1 road dang
    // co tau chay (Running): pushDown=true day xuong duoi (khi player dang
    // di len), pushDown=false day len tren (khi player dang di xuong).
    // Khac TriggerStun (bat trai/phai), ham nay bat len/xuong. =====
    void TriggerPushback(bool pushDown);

    // ===== ADDED: goi MOI FRAME tu WindGustManager khi dang trong pha
    // Active de day nhan vat troi ngang (px/s). Goi voi 0.f khi het gio
    // hoac khong trong pha Active =====
    void ApplyWindPush(float velocityXPerSec) { windPushVelocity = velocityXPerSec; }

    // ===== ADDED: du doan vi tri (x,y) neu di chuyen 1 buoc theo huong dir
    // (0=UP,1=DOWN,2=LEFT,3=RIGHT), KHONG thuc su di chuyen - dung de CGAME
    // kiem tra o ke tiep co dang bi MeteoriteManager chan (ho da) hay khong
    // TRUOC KHI goi Move...() that su =====
    void PeekNextPosition(int dir, float& outX, float& outY) const;

    float getX() const { return x; }
    float getY() const { return y; }

    // ===== ADDED (Continue Menu preview): expose huong/frame hien tai
    // de CGAME::SaveGame() ghi lai vao file .sav, phuc vu ContinueMenu
    // hien dung frame nhan vat tai thoi diem save cuoi cung (thay vi
    // luon hien 1 frame mac dinh co dinh) =====
    int GetDirection() const { return direction; }
    int GetCurrentFrame() const { return currentFrame; }
};