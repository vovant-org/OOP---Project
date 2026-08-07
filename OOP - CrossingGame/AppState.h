// AppState.h
#pragma once

//==================================================
// AppState
//
// Duoc tach ra file rieng (truoc day khai bao cuc bo
// trong main()) de MenuManager va cac file khac co the
// dung chung, khong phu thuoc vao main.cpp.
//==================================================

enum class AppState
{
    MainMenu = 0,
    CharSelect,
    MapSelect,
    ModeSelect,   // ===== ADDED: chọn EASY/HARD/NIGHTMARE trước khi chơi =====
    Settings,
    Playing,
    ContinueSelect,   // ===== ADDED: chọn map nào để Continue =====
    Leaderboard,      // ===== ADDED: bang diem cao 4 map x 3 do kho =====
    Pause,      // ===== ADDED: pause overlay khi dang Playing =====
    GameOver,   // ===== ADDED: overlay khi CGAME::IsGameOver() =====
    Win,        // ===== ADDED: overlay khi CGAME::IsWin() =====
    Exit
};