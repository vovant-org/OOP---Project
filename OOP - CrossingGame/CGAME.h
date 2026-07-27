// CGAME.h
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "CPEOPLE.h"
#include "CVEHICLE.h"
#include "CANIMAL.h"
#include "TrafficLight.h"

class CGAME {
private:
    sf::RenderWindow& mWindow;

    // Map
    sf::Texture mapTexture;
    sf::Sprite  mapSprite;
    int currentMap;   // 0=City 1=Ancient 2=Hell 3=Sky

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

    // Save/Load
    std::string savePath;

public:
    CGAME(sf::RenderWindow& window);
    ~CGAME();

    void Init(int mapIndex, int characterIndex);
    void Update(float dt);
    void Draw();
    void HandleInput(sf::Event& event);

    bool CheckCollision();
    bool CheckFinish();
    void OnDeath();
    void OnLevelComplete();

    void SaveGame(const std::string& path);
    void LoadGame(const std::string& path);

    void Pause() { isPaused = true; }
    void Resume() { isPaused = false; }
    bool IsGameOver() const { return isGameOver; }
};
