// SaveData.h
// Cấu trúc dữ liệu cho mỗi Save Slot
#pragma once
#include <string>

struct SaveData
{
    bool        exists = false;

    int         characterIndex = 0;
    std::string playerName;

    int         mapIndex = 0;
    int         score = 0;
    int         difficultyMode = 1;  // 0=Easy 1=Hard 2=Nightmare
    int         level = 1;

    std::string saveTime;   // "DD/MM/YYYY HH:MM:SS"
};

// Tên file 4 slot
inline const char* SLOT_PATHS[4] =
{
    "save1.sav",
    "save2.sav",
    "save3.sav",
    "save4.sav"
};
