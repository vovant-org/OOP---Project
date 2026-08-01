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
    Settings,
    Playing,
    Pause,      // ===== ADDED: pause overlay khi dang Playing =====
    GameOver,   // ===== ADDED: overlay khi CGAME::IsGameOver() =====
    Exit
};