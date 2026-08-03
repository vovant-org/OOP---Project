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
    // va trung - khong the di chuyen va bi day (bounce) ngau nhien sang
    // trai/phai, giu nguyen sprite/animation hien tai (khong doi anh) =====
    bool  isStunned;
    float stunTimer;
    float bounceVelocity;

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

    // ===== ADDED: kich hoat trang thai choang 1 giay - khong the di
    // chuyen, bi bat ra ngau nhien sang trai (bounceRight=false) hoac
    // phai (bounceRight=true). Goi tu RollingRockManager khi va cham. =====
    void TriggerStun(bool bounceRight);
    bool IsStunned() const { return isStunned; }

    float getX() const { return x; }
    float getY() const { return y; }
};