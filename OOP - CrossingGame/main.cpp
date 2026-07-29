// main.cpp — Crossing Game
// Luồng: MainMenu → CharacterSelection → MapSelection → Game
#include <SFML/Graphics.hpp>
#include <iostream>

#include "MainMenu.h"
#include "CharacterSelection.h"
#include "MapSelection.h"
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

    //--------------------------------------------------
    // Load shared assets
    //--------------------------------------------------
    sf::Texture bgTexture, logoTexture, buttonTexture;
    if (!loadTexture(bgTexture, BG_PATH))     return -1;
    if (!loadTexture(logoTexture, LOGO_PATH))   return -1;
    if (!loadTexture(buttonTexture, BUTTON_PATH)) return -1;

    float bgScaleX = (float)WIN_W / bgTexture.getSize().x;
    float bgScaleY = (float)WIN_H / bgTexture.getSize().y;

    float btnW = 240.f, btnH = 100.f;
    float btnScaleX = btnW / buttonTexture.getSize().x;
    float btnScaleY = btnH / buttonTexture.getSize().y;

    sf::Font font;
    if (!font.loadFromFile(FONT_PATH))
    {
        std::cout << "[ERROR] Cannot load font\n";
        return -1;
    }

    std::cout << "[INFO] Nhan F11 de bat/tat toan man hinh\n";

    //--------------------------------------------------
    // State machine
    //--------------------------------------------------
    enum class AppState { MainMenu, CharSelect, MapSelect, Playing, Exit };
    AppState state = AppState::MainMenu;

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
                applyLetterboxView(window, gameView);
                continue; // window vừa được tạo lại, event cũ không còn hợp lệ
            }

            // Cửa sổ đổi kích thước (kể cả khi chuyển sang fullscreen)
            if (event.type == sf::Event::Resized)
            {
                applyLetterboxView(window, gameView);
            }

            switch (state)
            {
            case AppState::MainMenu:
                mainMenu.processEvent(event, window);
                break;

            case AppState::CharSelect:
                charSelect.processEvent(event, window);
                break;

            case AppState::MapSelect:
                mapSelect.processEvent(event, window);
                break;

            default: break;
            }
        }

        //----------------------------------------------
        // Update + check transitions
        //----------------------------------------------
        switch (state)
        {
            //========================
        case AppState::MainMenu:
            //========================
            mainMenu.update();

            switch (mainMenu.getResult())
            {
            case MainMenuResult::Play:
                mainMenu.clearResult();
                charSelect.clearResult();
                state = AppState::CharSelect;
                break;

            case MainMenuResult::Continue:
                std::cout << "[INFO] Continue — chưa implement\n";
                mainMenu.clearResult();
                break;

            case MainMenuResult::Settings:
                std::cout << "[INFO] Settings — chưa implement\n";
                mainMenu.clearResult();
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
            charSelect.update(dt);

            switch (charSelect.getResult())
            {
            case CharacterSelectionResult::Selected:
                selectedCharIndex = charSelect.getSelectedIndex();
                std::cout << "[INFO] Character selected: "
                    << selectedCharIndex << "\n";
                charSelect.clearResult();
                mapSelect.clearResult();
                state = AppState::MapSelect;
                break;

            case CharacterSelectionResult::Back:
                charSelect.clearResult();
                state = AppState::MainMenu;
                break;

            default: break;
            }
            break;

            //========================
        case AppState::MapSelect:
            //========================
            mapSelect.update();

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
                state = AppState::MainMenu;
                break;

            case MapSelectionResult::Back:
                mapSelect.clearResult();
                state = AppState::CharSelect;
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

        switch (state)
        {
        case AppState::MainMenu:  mainMenu.draw(window);   break;
        case AppState::CharSelect: charSelect.draw(window); break;
        case AppState::MapSelect:  mapSelect.draw(window);  break;
        default: break;
        }
        window.display();
    }

    return 0;
}