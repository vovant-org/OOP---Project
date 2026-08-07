// main.cpp — Crossing Game
// Luồng: MainMenu → CharacterSelection → MapSelection → Game
#include <SFML/Graphics.hpp>
#include <iostream>

#include "AppState.h"        
#include "MenuManager.h"   

#include "MainMenu.h"
#include "CharacterSelection.h"
#include "MapSelection.h"
#include "SettingMenu.h"
#include "AudioManager.h"
#include "CGAME.h"   // ===== CHANGED: đã implement Bước 1 =====
#include "PauseMenu.h"      // ===== ADDED =====
#include "GameOverMenu.h"   // ===== ADDED =====
#include "WinMenu.h"        // ===== ADDED =====
#include "ContinueMenu.h"   // ===== ADDED =====
#include "ModeSelection.h"  // ===== ADDED =====
#include "LeaderboardMenu.h"   // ===== ADDED =====

//==================================================
// Hằng số
//==================================================
const unsigned int WIN_W = 1280;
const unsigned int WIN_H = 720;

const std::string FONT_PATH = "Font/PixelOperator.ttf";
const std::string BG_PATH = "ui/Background/CrossingGame_background.png";
const std::string LOGO_PATH = "ui/Logo/CrossingGame_Logo.png";
const std::string BUTTON_PATH = "ui/Button/button_normal.png";

const std::string SETTING_PANEL_PATH = "ui/Button/GoldenBox.png";
const std::string SETTING_BOX_PATH = "ui/Button/SilverBox.png";
const std::string SETTING_PLUS_PATH = "ui/Icon/Plus.png";
const std::string SETTING_MINUS_PATH = "ui/Icon/Minus.png";
const std::string SETTING_SWITCH_ON_PATH = "ui/Icon/Switch_ON.png";
const std::string SETTING_SWITCH_OFF_PATH = "ui/Icon/Switch_OFF.png";
const std::string SETTING_TIMES_PATH = "ui/Icon/Times.png";
const std::string SETTING_BACK_PATH = "ui/Logo/BACK.png";
const std::string SETTING_TITLE_PATH = "ui/Logo/SETTING.png";
const std::string PAUSE_ICON_PATH = "ui/Icon/Pause.png";   // ===== ADDED =====

// ===== ADDED: khung avatar mau theo do kho, dung cho LeaderboardMenu -
// sua duong dan neu file thuc te nam o thu muc khac =====
const std::string EASY_AVATAR_PATH = "ui/button/EasyAvatar.png";
const std::string HARD_AVATAR_PATH = "ui/button/HardAvatar.png";
const std::string NIGHTMARE_AVATAR_PATH = "ui/button/NightmareAvatar.png";

const std::string BGM_PATH = "Sound/MainMenu_backgroundmusic.mp3";
const std::string SFX_SELECT_PATH = "Sound/SelectSound.mp3";
const std::string SFX_COLLIDE_PATH = "Sound/CollideSound.mp3";   // ===== ADDED =====

//==================================================
// Helper: load chung tài nguyên
//==================================================
static bool loadTexture(sf::Texture& tex, const std::string& path)
{
    if (!tex.loadFromFile(path))
    {
        std::cout << "[ERROR] Cannot load: " << path << "\n";
        return false;
    }
    return true;
}

//==================================================
// Helper: letterbox — giữ đúng tỉ lệ WIN_W x WIN_H (canvas logic)
// dù cửa sổ/màn hình thực tế có kích thước hay tỉ lệ khác (fullscreen, v.v.)
// Nhờ vậy toàn bộ toạ độ UI (vốn tính theo pixel cố định 1280x720)
// không bị vỡ layout khi bật fullscreen.
//==================================================
static void applyLetterboxView(sf::RenderWindow& window, sf::View& view)
{
    sf::Vector2u wsize = window.getSize();
    if (wsize.x == 0 || wsize.y == 0) return;

    float windowRatio = (float)wsize.x / (float)wsize.y;
    float viewRatio = (float)WIN_W / (float)WIN_H;

    float sizeX = 1.f, sizeY = 1.f, posX = 0.f, posY = 0.f;

    if (windowRatio > viewRatio)
    {
        // Cửa sổ rộng hơn tỉ lệ 16:9 → viền đen 2 bên trái/phải
        sizeX = viewRatio / windowRatio;
        posX = (1.f - sizeX) / 2.f;
    }
    else
    {
        // Cửa sổ cao hơn tỉ lệ 16:9 → viền đen trên/dưới
        sizeY = windowRatio / viewRatio;
        posY = (1.f - sizeY) / 2.f;
    }

    view.setViewport(sf::FloatRect(posX, posY, sizeX, sizeY));
    window.setView(view);
}

//==================================================
// main
//==================================================
int main()
{
    //--------------------------------------------------
    // Window
    //--------------------------------------------------

    sf::RenderWindow window(
        sf::VideoMode(WIN_W, WIN_H),
        "Crossing Game",
        sf::Style::Close
    );
    window.setFramerateLimit(60);

    // View logic cố định 1280x720 — mọi toạ độ UI trong game đều tính
    // theo view này, nên khi đổi kích thước cửa sổ / bật fullscreen,
    // ta chỉ cần letterbox view chứ không cần sửa toạ độ UI ở đâu khác.
    bool isFullscreen = false;
    sf::View gameView(sf::FloatRect(0.f, 0.f, (float)WIN_W, (float)WIN_H));
    applyLetterboxView(window, gameView);

    // SettingMenu được thiết kế theo canvas logic 1920x1080 (xem SettingMenu.cpp),
    // khác với canvas 1280x720 của phần còn lại của game. Vì 1920x1080 và
    // 1280x720 cùng tỉ lệ 16:9, ta dùng lại đúng applyLetterboxView() cho view
    // riêng này mà không cần sửa hàm đó.
    sf::View settingsView(sf::FloatRect(0.f, 0.f, 1920.f, 1080.f));

    //--------------------------------------------------
    // Load shared assets
    //--------------------------------------------------
    sf::Texture bgTexture, logoTexture, buttonTexture;
    if (!loadTexture(bgTexture, BG_PATH))     return -1;
    if (!loadTexture(logoTexture, LOGO_PATH))   return -1;
    if (!loadTexture(buttonTexture, BUTTON_PATH)) return -1;

    float bgScaleX = (float)WIN_W / bgTexture.getSize().x;
    float bgScaleY = (float)WIN_H / bgTexture.getSize().y;

    // SettingMenu dùng canvas 1920x1080 (khác 1280x720 của các màn còn
    // lại), nên cần scale riêng cho cùng 1 ảnh background
    float settingBgScaleX = 1920.f / bgTexture.getSize().x;
    float settingBgScaleY = 1080.f / bgTexture.getSize().y;

    float btnW = 240.f, btnH = 100.f;
    float btnScaleX = btnW / buttonTexture.getSize().x;
    float btnScaleY = btnH / buttonTexture.getSize().y;

    sf::Font font;
    if (!font.loadFromFile(FONT_PATH))
    {
        std::cout << "[ERROR] Cannot load font\n";
        return -1;
    }

    sf::Texture settingPanelTexture, settingBoxTexture;
    sf::Texture settingPlusTexture, settingMinusTexture;
    sf::Texture settingSwitchOnTexture, settingSwitchOffTexture;
    sf::Texture settingTimesTexture;
    sf::Texture settingBackTexture;
    sf::Texture settingTitleTexture;

    if (!loadTexture(settingPanelTexture, SETTING_PANEL_PATH))     return -1;
    if (!loadTexture(settingBoxTexture, SETTING_BOX_PATH))         return -1;
    if (!loadTexture(settingPlusTexture, SETTING_PLUS_PATH))       return -1;
    if (!loadTexture(settingMinusTexture, SETTING_MINUS_PATH))     return -1;
    if (!loadTexture(settingSwitchOnTexture, SETTING_SWITCH_ON_PATH))  return -1;
    if (!loadTexture(settingSwitchOffTexture, SETTING_SWITCH_OFF_PATH)) return -1;
    if (!loadTexture(settingTimesTexture, SETTING_TIMES_PATH))     return -1;
    if (!loadTexture(settingBackTexture, SETTING_BACK_PATH))       return -1;
    if (!loadTexture(settingTitleTexture, SETTING_TITLE_PATH))     return -1;

    // ===== ADDED: icon Pause goc tren-phai khi dang Playing =====
    sf::Texture pauseIconTexture;
    if (!loadTexture(pauseIconTexture, PAUSE_ICON_PATH))
        return -1;

    std::cout << "[INFO] Nhan F11 de bat/tat toan man hinh\n";

    //--------------------------------------------------
    // ===== CHANGED: AppState giờ nằm ở AppState.h, không khai báo
    // cục bộ ở đây nữa =====
    //--------------------------------------------------

    int selectedCharIndex = 0;
    int selectedMapIndex = 0;
    int selectedMode = 0;   // ===== ADDED: 0=Easy 1=Hard 2=Nightmare (chua dung toi) =====

    // ===== ADDED: SettingMenu co the duoc mo tu MainMenu HOAC tu
    // PauseMenu -> can biet quay ve dau khi bam Back =====
    AppState settingsReturnState = AppState::MainMenu;

    // ===== ADDED: cho gameover co 1 khoang delay nho de choi het
    // animation chet truoc khi hien GameOverMenu =====
    float gameOverTimer = 0.f;
    const float GAME_OVER_DELAY = 1.2f;

    // ===== ADDED: delay tương tự cho màn thắng =====
    float winTimer = 0.f;
    const float WIN_DELAY = 1.2f;

    //--------------------------------------------------
    // Build MainMenu
    //--------------------------------------------------
    MainMenu mainMenu;
    mainMenu.setBackgroundTexture(bgTexture);
    mainMenu.setBackgroundScale(bgScaleX, bgScaleY);
    mainMenu.setLogoTexture(logoTexture);
    mainMenu.setLogoScale(0.40f, 0.40f);
    mainMenu.setLogoPosition(330.f, -50.f);

    {
        float bx = 515.f, startY = 250.f, spacing = 55.f;
        auto setupBtn = [&](Button& btn, const std::string& text, float y) {
            btn.setTexture(buttonTexture);
            btn.setFont(font);
            btn.setText(text);
            btn.setCharacterSize(28);
            btn.setScale(btnScaleX, btnScaleY);
            btn.setPosition(bx, y);
            btn.setFocused(false);
            };
        setupBtn(mainMenu.getButton(MainMenuButton::Play), "PLAY", startY);
        setupBtn(mainMenu.getButton(MainMenuButton::Continue), "CONTINUE", startY + spacing);
        setupBtn(mainMenu.getButton(MainMenuButton::Settings), "SETTINGS", startY + spacing * 2);
        setupBtn(mainMenu.getButton(MainMenuButton::Leaderboard), "LEADERBOARD", startY + spacing * 3);
        setupBtn(mainMenu.getButton(MainMenuButton::About), "ABOUT", startY + spacing * 4);
        setupBtn(mainMenu.getButton(MainMenuButton::Exit), "EXIT", startY + spacing * 5);
    }

    //--------------------------------------------------
    // Build CharacterSelection
    //--------------------------------------------------
    CharacterSelection charSelect;
    charSelect.setWindowSize((float)WIN_W, (float)WIN_H);
    charSelect.setBackgroundTexture(bgTexture, bgScaleX, bgScaleY);
    charSelect.loadFont(FONT_PATH);
    charSelect.loadCharacterTexture(0, "Character/Chicken_character.png");
    charSelect.loadCharacterTexture(1, "Character/Knight_character.png");
    charSelect.loadCharacterTexture(2, "Character/Dog_character.png");
    charSelect.loadCharacterTexture(3, "Character/Luffy_character.png");

    charSelect.loadUITextures(
        "ui/Icon/LeftArrow.png",
        "ui/Icon/Heart.png",
        "ui/Icon/Lightning.png");

    charSelect.setupButtons(buttonTexture, btnW, btnH, btnScaleX, btnScaleY);
    charSelect.setupLayout();

    //--------------------------------------------------
    // Build MapSelection
    //--------------------------------------------------
    MapSelection mapSelect;
    mapSelect.setWindowSize((float)WIN_W, (float)WIN_H);
    mapSelect.setBackgroundTexture(bgTexture, bgScaleX, bgScaleY);
    mapSelect.loadFont(FONT_PATH);
    mapSelect.loadMapThumbnail(0, "Map/City_map.png");
    mapSelect.loadMapThumbnail(1, "Map/Ancient_map.png");
    mapSelect.loadMapThumbnail(2, "Map/Hell_map.png");
    mapSelect.loadMapThumbnail(3, "Map/Sky_map.png");

    mapSelect.loadArrowTexture(
        "ui/Icon/LeftArrow.png");

    mapSelect.setupButtons(buttonTexture, btnW, btnH, btnScaleX, btnScaleY);
    mapSelect.setupLayout();

    //--------------------------------------------------
    // Build ModeSelection
    //--------------------------------------------------
    ModeSelection modeSelect;
    modeSelect.setWindowSize((float)WIN_W, (float)WIN_H);
    modeSelect.setBackgroundTexture(bgTexture, bgScaleX, bgScaleY);
    modeSelect.loadFont(FONT_PATH);
    modeSelect.loadBoxTexture(SETTING_BOX_PATH);   // tái dùng SilverBox có sẵn
    modeSelect.setupButtons(buttonTexture, btnW, btnH, btnScaleX, btnScaleY);
    modeSelect.setupLayout();

    //--------------------------------------------------
    // Build SettingMenu
    //--------------------------------------------------
    SettingMenu settingMenu;
    settingMenu.setBackgroundTexture(bgTexture, settingBgScaleX, settingBgScaleY);
    settingMenu.setPanelTexture(settingPanelTexture);
    settingMenu.setValueBoxTexture(settingBoxTexture);
    settingMenu.setPlusTexture(settingPlusTexture);
    settingMenu.setMinusTexture(settingMinusTexture);
    settingMenu.setSwitchTextures(settingSwitchOnTexture, settingSwitchOffTexture);
    settingMenu.setTimesTexture(settingTimesTexture);
    settingMenu.setBackButtonTexture(settingBackTexture);
    settingMenu.setTitleTexture(settingTitleTexture);
    settingMenu.setFont(font);

    //--------------------------------------------------
    // ===== ADDED: Gameplay =====
    //--------------------------------------------------
    CGAME game(window);
    game.SetFont(font);   // ===== ADDED: HUD Score/Level =====

    //--------------------------------------------------
    // ===== ADDED: PauseMenu / GameOverMenu =====
    // Dung chung buttonTexture/font/scale voi cac menu khac,
    // canh giua theo canvas 1280x720 (WIN_W x WIN_H).
    //--------------------------------------------------
    PauseMenu pauseMenu;
    pauseMenu.setFont(font);
    pauseMenu.setOverlaySize((float)WIN_W, (float)WIN_H);
    {
        float bx = 515.f, startY = 260.f, spacing = 55.f;
        auto setupPauseBtn = [&](PauseMenuButton b, const std::string& text, float y) {
            Button& btn = pauseMenu.getButton(b);
            btn.setTexture(buttonTexture);
            btn.setFont(font);
            btn.setText(text);
            btn.setCharacterSize(28);
            btn.setScale(btnScaleX, btnScaleY);
            btn.setPosition(bx, y);
            btn.setFocused(false);
            };
        setupPauseBtn(PauseMenuButton::Resume, "RESUME", startY);
        setupPauseBtn(PauseMenuButton::Restart, "RESTART", startY + spacing);
        setupPauseBtn(PauseMenuButton::Settings, "SETTINGS", startY + spacing * 2);
        setupPauseBtn(PauseMenuButton::MainMenu, "MAIN MENU", startY + spacing * 3);
    }

    GameOverMenu gameOverMenu;
    gameOverMenu.setFont(font);
    gameOverMenu.setOverlaySize((float)WIN_W, (float)WIN_H);
    {
        float bx = 515.f, startY = 420.f, spacing = 55.f;
        auto setupOverBtn = [&](GameOverMenuButton b, const std::string& text, float y) {
            Button& btn = gameOverMenu.getButton(b);
            btn.setTexture(buttonTexture);
            btn.setFont(font);
            btn.setText(text);
            btn.setCharacterSize(28);
            btn.setScale(btnScaleX, btnScaleY);
            btn.setPosition(bx, y);
            btn.setFocused(false);
            };
        setupOverBtn(GameOverMenuButton::Retry, "RETRY", startY);
        setupOverBtn(GameOverMenuButton::MainMenu, "MAIN MENU", startY + spacing);
    }

    // ===== ADDED: WinMenu =====
    WinMenu winMenu;
    winMenu.setFont(font);
    winMenu.setOverlaySize((float)WIN_W, (float)WIN_H);
    {
        float bx = 515.f, startY = 420.f, spacing = 55.f;
        auto setupWinBtn = [&](WinMenuButton b, const std::string& text, float y) {
            Button& btn = winMenu.getButton(b);
            btn.setTexture(buttonTexture);
            btn.setFont(font);
            btn.setText(text);
            btn.setCharacterSize(28);
            btn.setScale(btnScaleX, btnScaleY);
            btn.setPosition(bx, y);
            btn.setFocused(false);
            };
        setupWinBtn(WinMenuButton::PlayAgain, "PLAY AGAIN", startY);
        setupWinBtn(WinMenuButton::MainMenu, "MAIN MENU", startY + spacing);
    }

    // ===== ADDED: ContinueMenu - man hinh chon map nao de Continue =====
    ContinueMenu continueMenu;
    continueMenu.setWindowSize((float)WIN_W, (float)WIN_H);
    continueMenu.setBackgroundTexture(bgTexture, bgScaleX, bgScaleY);
    continueMenu.setFont(font);
    continueMenu.setButtonTexture(buttonTexture, btnScaleX, btnScaleY);
    continueMenu.setModeBoxTexture(settingBoxTexture);   // ===== ADDED: SilverBox cho header 3 mode =====

    // ===== ADDED: LeaderboardMenu - bang diem 4 map x 3 do kho, doc lai
    // chinh 12 file save (khong luu file rieng) =====
    LeaderboardMenu leaderboardMenu;
    leaderboardMenu.setWindowSize((float)WIN_W, (float)WIN_H);
    leaderboardMenu.setBackgroundTexture(bgTexture, bgScaleX, bgScaleY);
    leaderboardMenu.setFont(font);
    leaderboardMenu.setButtonTexture(buttonTexture, btnScaleX, btnScaleY);
    leaderboardMenu.setPanelTexture(settingPanelTexture);   // GoldenBox - khung ngoai
    leaderboardMenu.setRowBoxTexture(settingBoxTexture);    // SilverBox - nen tung dong xep hang

    // Khung avatar mau theo do kho (Easy=xanh/Hard=do/Nightmare=tim)
    leaderboardMenu.loadModeFrameTexture(0, EASY_AVATAR_PATH);
    leaderboardMenu.loadModeFrameTexture(1, HARD_AVATAR_PATH);
    leaderboardMenu.loadModeFrameTexture(2, NIGHTMARE_AVATAR_PATH);

    // Sprite sheet nhan vat (dung chung file voi CharacterSelection ben tren)
    leaderboardMenu.loadCharacterTexture(0, "Character/Chicken_character.png");
    leaderboardMenu.loadCharacterTexture(1, "Character/Knight_character.png");
    leaderboardMenu.loadCharacterTexture(2, "Character/Dog_character.png");
    leaderboardMenu.loadCharacterTexture(3, "Character/Luffy_character.png");

    //--------------------------------------------------
    // ===== ADDED: nut icon Pause, goc tren-phai man hinh, chi hien
    // va bam duoc khi dang o state Playing =====
    //--------------------------------------------------
    Button pauseButton;
    {
        const float ICON_SIZE = 48.f;
        float isx = ICON_SIZE / pauseIconTexture.getSize().x;
        float isy = ICON_SIZE / pauseIconTexture.getSize().y;

        pauseButton.setTexture(pauseIconTexture);
        pauseButton.setScale(isx, isy);
        pauseButton.setPosition((float)WIN_W - ICON_SIZE - 16.f, 16.f);
        pauseButton.setFocused(false);
    }

    //--------------------------------------------------
    // Audio
    //--------------------------------------------------
    AudioManager audio;

    if (!audio.loadMusic(BGM_PATH))
        return -1;

    if (!audio.loadSound("select", SFX_SELECT_PATH))
        return -1;

    // ===== ADDED: SFX phat khi player va cham (mat HP), xem CGAME::Update() =====
    if (!audio.loadSound("collide", SFX_COLLIDE_PATH))
        return -1;

    audio.setMusicVolume(settingMenu.getMusicVolume());
    audio.playMusic(true);

    settingMenu.setOnMusicVolumeChanged([&audio](int volume)
        {
            audio.setMusicVolume(volume);
        });

    mainMenu.setAudioManager(&audio);
    charSelect.setAudioManager(&audio);
    mapSelect.setAudioManager(&audio);
    modeSelect.setAudioManager(&audio);     // ===== ADDED =====
    settingMenu.setAudioManager(&audio);    // ===== FIXED: truoc day dong nay bi
    // "lot" vao trong comment cua dong tren
    // (// ===== ADDED ===== settingMenu...),
    // nen KHONG BAO GIO duoc goi - SettingMenu::audio
    // luon la nullptr, khien slider "Sound" chi
    // doi so hien thi ma khong dieu khien duoc
    // AudioManager thuc su (SFX hazard, nut
    // "select",... deu khong nghe theo) =====
    pauseMenu.setAudioManager(&audio);      // ===== ADDED =====
    gameOverMenu.setAudioManager(&audio);   // ===== ADDED =====
    winMenu.setAudioManager(&audio);        // ===== ADDED =====
    continueMenu.setAudioManager(&audio);   // ===== ADDED =====
    leaderboardMenu.setAudioManager(&audio);   // ===== ADDED =====
    game.SetAudioManager(&audio);           // ===== ADDED: nhac nen theo map =====

    //--------------------------------------------------
    // ===== CHANGED: MenuManager thay cho 3 khoi switch(state) =====
    // SettingMenu dùng canvas 1920x1080 (settingsView), các state còn lại
    // dùng canvas 1280x720 (gameView).
    //--------------------------------------------------
    auto applyViewForState = [&](AppState st) {
        if (st == AppState::Settings)
            applyLetterboxView(window, settingsView);
        else
            applyLetterboxView(window, gameView);
        };

    MenuManager menuManager;
    menuManager.setApplyViewFn(applyViewForState);

    menuManager.registerMenu(AppState::MainMenu, &mainMenu);
    menuManager.registerMenu(AppState::CharSelect, &charSelect);
    menuManager.registerMenu(AppState::MapSelect, &mapSelect);
    menuManager.registerMenu(AppState::ModeSelect, &modeSelect);   // ===== ADDED =====
    menuManager.registerMenu(AppState::Settings, &settingMenu);
    menuManager.registerMenu(AppState::Pause, &pauseMenu);       // ===== ADDED =====
    menuManager.registerMenu(AppState::GameOver, &gameOverMenu); // ===== ADDED =====
    menuManager.registerMenu(AppState::Win, &winMenu);           // ===== ADDED =====
    menuManager.registerMenu(AppState::ContinueSelect, &continueMenu); // ===== ADDED =====
    menuManager.registerMenu(AppState::Leaderboard, &leaderboardMenu); // ===== ADDED =====
    // AppState::Playing / Exit: chưa có Menu tương ứng (CGAME chưa xong) -
    // cứ để trống, MenuManager tự bỏ qua processEvent/update/draw cho các
    // state này (xem getCurrentMenu() trả nullptr).

    menuManager.setState(AppState::MainMenu);

    //--------------------------------------------------
    // Game loop
    //--------------------------------------------------
    sf::Clock clock;

    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                break;
            }

            // F11 — bật/tắt toàn màn hình
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::F11)
            {
                isFullscreen = !isFullscreen;

                if (isFullscreen)
                {
                    // ===== CHANGED: dung "windowed fullscreen" (cua so
                    // khong vien, full kich thuoc desktop) thay vi
                    // sf::Style::Fullscreen (exclusive). Ly do: exclusive
                    // fullscreen CHI vao duoc neu VideoMode nam trong
                    // sf::VideoMode::getFullscreenModes() - tren nhieu may
                    // Windows co scaling man hinh khac 100% (thuong gap o
                    // laptop/man hinh do phan giai cao), getDesktopMode()
                    // KHONG nam trong danh sach do, nen SFML se AM THAM bo
                    // qua yeu cau Fullscreen va tao lai cua so binh thuong
                    // - day chinh la ly do F11 "khong co tac dung gi".
                    // Windowed fullscreen khong phu thuoc danh sach video
                    // mode nen luon hoat dong on dinh =====
                    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
                    window.create(desktop, "Crossing Game", sf::Style::None);
                    window.setPosition(sf::Vector2i(0, 0));
                }
                else
                {
                    window.create(sf::VideoMode(WIN_W, WIN_H),
                        "Crossing Game", sf::Style::Close);
                }
                window.setFramerateLimit(60);
                menuManager.reapplyView();    // ===== CHANGED =====
                continue; // window vừa được tạo lại, event cũ không còn hợp lệ
            }

            // Cửa sổ đổi kích thước (kể cả khi chuyển sang fullscreen)
            if (event.type == sf::Event::Resized)
            {
                menuManager.reapplyView();    // ===== CHANGED =====
            }

            // ===== ADDED: ESC khi dang choi -> mo PauseMenu. Dung continue
            // de tranh event ESC nay bi forward tiep xuong PauseMenu ngay
            // trong cung 1 vong lap (PauseMenu cung dang bat ESC = Resume) =====
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Escape &&
                menuManager.getState() == AppState::Playing)
            {
                game.Pause();
                pauseMenu.clearResult();
                menuManager.setState(AppState::Pause);
                continue;
            }

            // ===== ADDED: bấm icon Pause góc trên-phải (chỉ khi đang
            // Playing) -> cùng hành động với phím ESC ở trên =====
            if (menuManager.getState() == AppState::Playing)
            {
                pauseButton.processEvent(event, window);

                if (event.type == sf::Event::MouseButtonReleased &&
                    event.mouseButton.button == sf::Mouse::Left)
                {
                    sf::Vector2f mousePos = window.mapPixelToCoords(
                        { event.mouseButton.x, event.mouseButton.y });

                    if (pauseButton.contains(mousePos))
                    {
                        pauseButton.press();
                        game.Pause();
                        pauseMenu.clearResult();
                        menuManager.setState(AppState::Pause);
                        continue;
                    }
                }
            }

            // ===== CHANGED: 1 dòng duy nhất thay cho switch(state){...} =====
            menuManager.processEvent(event, window);

            // ===== ADDED: CGAME không phải Menu nên xử lý riêng =====
            if (menuManager.getState() == AppState::Playing)
                game.HandleInput(event);
        }

        //----------------------------------------------
        // Update + check transitions
        //----------------------------------------------

        // ===== CHANGED: goi update() cho Menu dang active =====
        menuManager.update(dt);

        // ===== ADDED: CGAME không phải Menu nên update riêng =====
        if (menuManager.getState() == AppState::Playing)
        {
            game.Update(dt);
            pauseButton.update();   // ===== ADDED =====

            // ===== ADDED: cho animation chet choi 1 doan ngan
            // (GAME_OVER_DELAY) roi moi chuyen sang GameOverMenu =====
            if (game.IsGameOver())
            {
                gameOverTimer += dt;

                if (gameOverTimer >= GAME_OVER_DELAY)
                {
                    gameOverTimer = 0.f;
                    gameOverMenu.setStats(game.GetScore(), game.GetLevel());
                    gameOverMenu.clearResult();
                    menuManager.setState(AppState::GameOver);
                }
            }
            else
            {
                gameOverTimer = 0.f;
            }

            // ===== ADDED: tương tự GameOver, nhưng cho màn thắng =====
            if (game.IsWin())
            {
                winTimer += dt;

                if (winTimer >= WIN_DELAY)
                {
                    winTimer = 0.f;
                    winMenu.setStats(game.GetScore(), game.GetLevel());
                    winMenu.clearResult();
                    menuManager.setState(AppState::Win);
                }
            }
            else
            {
                winTimer = 0.f;
            }
        }

        switch (menuManager.getState())
        {
            //========================
        case AppState::MainMenu:
            //========================
            switch (mainMenu.getResult())
            {
            case MainMenuResult::Play:
                mainMenu.clearResult();
                charSelect.clearResult();
                menuManager.setState(AppState::CharSelect);   // ===== CHANGED =====
                break;

            case MainMenuResult::Continue:
                mainMenu.clearResult();

                // ===== CHANGED (lưu riêng từng map): mở màn hình chọn
                // map nào để tiếp tục, thay vì load thẳng 1 file chung =====
                continueMenu.refresh();
                continueMenu.clearResult();
                menuManager.setState(AppState::ContinueSelect);
                break;

            case MainMenuResult::Leaderboard:   // ===== ADDED =====
                mainMenu.clearResult();

                leaderboardMenu.refresh();
                leaderboardMenu.clearResult();
                menuManager.setState(AppState::Leaderboard);
                break;

            case MainMenuResult::Settings:
                mainMenu.clearResult();
                settingMenu.clearResult();
                settingsReturnState = AppState::MainMenu;     // ===== ADDED =====
                menuManager.setState(AppState::Settings);     // ===== CHANGED =====
                break;

            case MainMenuResult::Exit:
                window.close();
                break;

            default: break;
            }
            break;

            //========================
        case AppState::CharSelect:
            //========================
            switch (charSelect.getResult())
            {
            case CharacterSelectionResult::Selected:
                selectedCharIndex = charSelect.getSelectedIndex();
                std::cout << "[INFO] Character selected: "
                    << selectedCharIndex << "\n";
                charSelect.clearResult();
                mapSelect.clearResult();
                menuManager.setState(AppState::MapSelect);    // ===== CHANGED =====
                break;

            case CharacterSelectionResult::Back:
                charSelect.clearResult();
                menuManager.setState(AppState::MainMenu);     // ===== CHANGED =====
                break;

            default: break;
            }
            break;

            //========================
        case AppState::MapSelect:
            //========================
            switch (mapSelect.getResult())
            {
            case MapSelectionResult::Selected:
                selectedMapIndex = mapSelect.getSelectedIndex();
                std::cout << "[INFO] Map selected: "
                    << selectedMapIndex << "\n";
                mapSelect.clearResult();

                // ===== CHANGED: qua ModeSelect truoc, chua vao Playing ngay =====
                modeSelect.clearResult();
                menuManager.setState(AppState::ModeSelect);
                break;

            case MapSelectionResult::Back:
                mapSelect.clearResult();
                menuManager.setState(AppState::CharSelect);   // ===== CHANGED =====
                break;
            default: break;
            }
            break;

            //========================
        case AppState::ModeSelect:   // ===== ADDED =====
            //========================
            switch (modeSelect.getResult())
            {
            case ModeSelectionResult::Selected:
                selectedMode = modeSelect.getSelectedMode();   // 0=Easy,1=Hard,2=Nightmare
                std::cout << "[INFO] Mode selected: " << selectedMode << "\n";
                modeSelect.clearResult();

                // ===== CHANGED: mode gio da anh huong toc do/so luong
                // obstacle + so den giao thong, xem CGAME::Init() =====
                game.SetDifficultyMode(selectedMode);
                game.Init(selectedMapIndex, selectedCharIndex);
                menuManager.setState(AppState::Playing);
                break;

            case ModeSelectionResult::Back:
                modeSelect.clearResult();
                menuManager.setState(AppState::MapSelect);
                break;

            default: break;
            }
            break;

            //========================
        case AppState::Settings:
            //========================
            switch (settingMenu.getResult())
            {
            case SettingMenuResult::Back:
                settingMenu.clearResult();
                // ===== CHANGED: quay lai dung noi da mo Settings
                // (MainMenu hoac Pause) thay vi luon ve MainMenu =====
                menuManager.setState(settingsReturnState);
                break;

            default: break;
            }
            break;

            //========================
        case AppState::Pause:   // ===== ADDED =====
            //========================
            switch (pauseMenu.getResult())
            {
            case PauseMenuResult::Resume:
                pauseMenu.clearResult();
                game.Resume();
                menuManager.setState(AppState::Playing);
                break;

            case PauseMenuResult::Restart:
                pauseMenu.clearResult();
                game.Init(selectedMapIndex, selectedCharIndex);
                menuManager.setState(AppState::Playing);
                break;

            case PauseMenuResult::Settings:
                pauseMenu.clearResult();
                settingMenu.clearResult();
                settingsReturnState = AppState::Pause;
                menuManager.setState(AppState::Settings);
                break;

            case PauseMenuResult::MainMenu:
                pauseMenu.clearResult();
                game.Resume();   // tranh treo isPaused=true khi vao lai game sau nay

                // ===== ADDED: doi lai nhac nen MainMenu (dang phat nhac
                // rieng cua map) =====
                audio.loadMusic(BGM_PATH);
                audio.playMusic(true);

                menuManager.setState(AppState::MainMenu);
                break;

            default: break;
            }
            break;

            //========================
        case AppState::GameOver:   // ===== ADDED =====
            //========================
            switch (gameOverMenu.getResult())
            {
            case GameOverMenuResult::Retry:
                gameOverMenu.clearResult();
                game.Init(selectedMapIndex, selectedCharIndex);
                menuManager.setState(AppState::Playing);
                break;

            case GameOverMenuResult::MainMenu:
                gameOverMenu.clearResult();

                // ===== ADDED: doi lai nhac nen MainMenu =====
                audio.loadMusic(BGM_PATH);
                audio.playMusic(true);

                menuManager.setState(AppState::MainMenu);
                break;

            default: break;
            }
            break;

            //========================
        case AppState::Win:   // ===== ADDED =====
            //========================
            switch (winMenu.getResult())
            {
            case WinMenuResult::PlayAgain:
                winMenu.clearResult();
                game.Init(selectedMapIndex, selectedCharIndex);
                menuManager.setState(AppState::Playing);
                break;

            case WinMenuResult::MainMenu:
                winMenu.clearResult();

                // ===== ADDED: doi lai nhac nen MainMenu =====
                audio.loadMusic(BGM_PATH);
                audio.playMusic(true);

                menuManager.setState(AppState::MainMenu);
                break;

            default: break;
            }
            break;

            //========================
        case AppState::ContinueSelect:   // ===== ADDED =====
            //========================
            switch (continueMenu.getResult())
            {
            case ContinueMenuResult::Selected:
            {
                continueMenu.clearResult();

                int mapIdx = continueMenu.getSelectedMapIndex();
                int modeIdx = continueMenu.getSelectedMode();   // ===== ADDED =====

                if (game.LoadGame(CGAME::GetSavePathForMap(mapIdx, modeIdx)))
                {
                    selectedMapIndex = game.GetCurrentMap();
                    selectedCharIndex = game.GetCharacterIndex();
                    selectedMode = game.GetDifficultyMode();   // ===== ADDED =====
                    menuManager.setState(AppState::Playing);
                }
                else
                {
                    // Không nên xảy ra (continueMenu chỉ hiện map đã
                    // confirm có save qua refresh()), nhưng phòng hờ
                    std::cout << "[WARN] Continue: save bị mất giữa chừng\n";
                    menuManager.setState(AppState::MainMenu);
                }
                break;
            }

            case ContinueMenuResult::Back:
                continueMenu.clearResult();
                menuManager.setState(AppState::MainMenu);
                break;

            default: break;
            }
            break;

            //========================
        case AppState::Leaderboard:   // ===== ADDED =====
            //========================
            switch (leaderboardMenu.getResult())
            {
            case LeaderboardMenuResult::Back:
                leaderboardMenu.clearResult();
                menuManager.setState(AppState::MainMenu);
                break;

            default: break;
            }
            break;

        default: break;
        }

        //----------------------------------------------
        // Draw
        //----------------------------------------------
        window.clear();

        // ===== CHANGED: CGAME không phải Menu nên vẽ riêng =====
        if (menuManager.getState() == AppState::Playing)
        {
            game.Draw();
            pauseButton.draw(window);   // ===== ADDED =====
        }
        else if (menuManager.getState() == AppState::Pause ||
            menuManager.getState() == AppState::GameOver ||
            menuManager.getState() == AppState::Win)
        {
            // ===== ADDED: ve man hinh game dong bang lam nen, roi ve
            // PauseMenu/GameOverMenu/WinMenu (co overlay mo den) len tren =====
            game.Draw();
            menuManager.draw(window);
        }
        else
        {
            menuManager.draw(window);
        }

        window.display();
    }

    return 0;
}
