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
    About,      // ===== ADDED: man hinh gioi thieu game =====
    ModMenu,    // ===== ADDED: man hinh MOD - import skin tuy chinh =====
    // ===== ADDED: overlay nhap ten save, mo tu phim 'L' (dang Playing)
    // HOAC nut "SAVE GAME" trong Pause Menu - xem SaveNamePrompt.h va
    // main.cpp (bien "saveNamePromptReturnState") =====
    SaveNamePrompt,
    // ===== ADDED: overlay nhap TEN SAVE CAN LOAD, mo tu phim 'T' o
    // Main Menu - tim trong Save/ file nao co playerName trung ten vua
    // go, neu co thi Load luon, neu khong thi bao loi ngay trong dialog
    // (xem main.cpp, khu vuc xu ly phim 'T' o MainMenu) =====
    LoadByNamePrompt,
    Exit
};