#pragma once

//==================================================
// Các trạng thái của game
//==================================================

enum class GameState
{
    MainMenu = 0,

    Settings,

    Leaderboard,

    About,

    Playing,

    Pause,

    GameOver,

    Win,

    Exit
};
