// main.cpp — Crossing Game
// Luồng: MainMenu → CharacterSelection → MapSelection → Game
#include <SFML/Graphics.hpp>
#include <iostream>

#include "AppState.h"        // ===== ADDED =====
#include "MenuManager.h"     // ===== ADDED =====

#include "MainMenu.h"
#include "CharacterSelection.h"
#include "MapSelection.h"
#include "SettingMenu.h"
#include "AudioManager.h"
// #include "CGAME.h"   ← bật khi implement xong

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

const std::string BGM_PATH = "Sound/Hellmap_backgroundmusic.mp3";
const std::string SFX_SELECT_PATH = "Sound/SelectSound.mp3";

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

    std::cout << "[INFO] Nhan F11 de bat/tat toan man hinh\n";

    //--------------------------------------------------
    // ===== CHANGED: AppState giờ nằm ở AppState.h, không khai báo
    // cục bộ ở đây nữa =====
    //--------------------------------------------------

    int selectedCharIndex = 0;
    int selectedMapIndex = 0;

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
    // Audio
    //--------------------------------------------------
    AudioManager audio;

    if (!audio.loadMusic(BGM_PATH))
        return -1;

    if (!audio.loadSound("select", SFX_SELECT_PATH))
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
    settingMenu.setAudioManager(&audio);

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
    menuManager.registerMenu(AppState::Settings, &settingMenu);
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
                    window.create(sf::VideoMode::getDesktopMode(),
                        "Crossing Game", sf::Style::Fullscreen);
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

            // ===== CHANGED: 1 dòng duy nhất thay cho switch(state){...} =====
            menuManager.processEvent(event, window);
        }

        //----------------------------------------------
        // Update + check transitions
        //----------------------------------------------

        // ===== CHANGED: goi update() cho Menu dang active =====
        menuManager.update();

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
                std::cout << "[INFO] Continue — chưa implement\n";
                mainMenu.clearResult();
                break;

            case MainMenuResult::Settings:
                mainMenu.clearResult();
                settingMenu.clearResult();
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

                // TODO: khởi tạo CGAME với selectedCharIndex + selectedMapIndex
                // CGAME game(window, selectedCharIndex, selectedMapIndex);
                // game.run();
                // Sau khi game kết thúc → về MainMenu
                menuManager.setState(AppState::MainMenu);     // ===== CHANGED =====
                break;

            case MapSelectionResult::Back:
                mapSelect.clearResult();
                menuManager.setState(AppState::CharSelect);   // ===== CHANGED =====
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
                menuManager.setState(AppState::MainMenu);     // ===== CHANGED =====
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

        // ===== CHANGED: 1 dòng duy nhất thay cho switch(state){...} =====
        menuManager.draw(window);

        window.display();
    }

    return 0;
}