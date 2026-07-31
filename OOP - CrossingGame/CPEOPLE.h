// CPEOPLE.h
#pragma once
#include <SFML/Graphics.hpp>

class CPEOPLE {
private:
    float x, y;
    float speed;
    bool  isAlive;

    sf::Texture texture;
    sf::Sprite  sprite;

    int currentFrame;
    int direction;   // 0=UP 1=DOWN 2=LEFT 3=RIGHT 4=DIE
    int frameWidth, frameHeight;
    float frameTime, elapsedTime;

public:
    CPEOPLE(float startX, float startY);
    bool loadTexture(const std::string& path);

    void MoveUp();
    void MoveDown();
    void MoveLeft();
    void MoveRight();

    void Update(float dt);
    void Draw(sf::RenderWindow& window);

    sf::FloatRect GetBoundingBox() const;
    bool IsAlive() const { return isAlive; }
    void SetDead() { isAlive = false; }
    void Reset(float startX, float startY);

    // ===== ADDED (Bước 4): chuyển sang tư thế/animation chết khi va chạm =====
    void TriggerDeath();

    float getX() const { return x; }
    float getY() const { return y; }
};