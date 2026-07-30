// MenuManager.h
#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <functional>

#include "Menu.h"
#include "AppState.h"

//==================================================
// MenuManager
//
// Gom viec goi processEvent/update/draw cho dung Menu
// dang active theo AppState hien tai, thay cho 3 khoi
// switch(state) lap lai trong main.cpp.
//
// Logic CHUYEN state (dua tren Result rieng cua tung
// Menu, vi moi Menu co 1 enum Result khac nhau) van nam
// o main.cpp - MenuManager khong biet gi ve Result.
//
// Con tro Menu* dang ky vao day phai song suot vong doi
// MenuManager (thuong la bien local trong main(), MenuManager
// khong so huu / khong delete chung).
//==================================================

class MenuManager
{
public:

    // Callback ap dung view (letterbox) rieng cho tung state -
    // main.cpp truyen vao lambda applyViewForState da co san.
    using ApplyViewFn = std::function<void(AppState)>;

    void setApplyViewFn(ApplyViewFn fn);

    //----------------------------------
    // Dang ky Menu cho 1 state
    //----------------------------------

    void registerMenu(AppState state, Menu* menu);

    //----------------------------------
    // State
    //----------------------------------

    // Chuyen sang state moi + tu dong goi ApplyViewFn tuong ung
    void setState(AppState newState);

    AppState getState() const;

    // Goi lai ApplyViewFn cho state hien tai (dung khi F11 / Resized,
    // khong doi state, chi can ap dung lai view)
    void reapplyView() const;

    Menu* getCurrentMenu() const;

    //----------------------------------
    // Forward toi Menu dang active
    // (khong lam gi neu state hien tai chua duoc registerMenu -
    //  vi du Playing / Exit)
    //----------------------------------

    void processEvent(const sf::Event& event, const sf::RenderWindow& window);
    void update(float dt);
    void draw(sf::RenderWindow& window) const;

private:

    std::unordered_map<AppState, Menu*> menus;

    AppState currentState = AppState::MainMenu;

    ApplyViewFn applyViewFn;
};
