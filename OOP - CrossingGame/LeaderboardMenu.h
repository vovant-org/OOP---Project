// LeaderboardMenu.h
#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

#include "Menu.h"
#include "Button.h"
#include "MenuBackground.h"

class AudioManager;

//==================================================
// Result
//==================================================

enum class LeaderboardMenuResult
{
    None = 0,
    Back
};

//==================================================
// LeaderboardMenu
//
// ===== REDESIGNED: khong con la luoi 4x3 tinh, ma la 1 DANH SACH xep
// hang (rank) doc lai tu 12 file save (4 map x 3 mode, qua
// CGAME::GetSavePathForMap + CGAME::PeekSaveInfo), gop chung khong phan
// biet map/mode, sap xep theo:
//   1) score cao hon -> tren
//   2) level cao hon -> tren (neu score bang nhau)
//   3) "da ton tai truoc" -> tren (neu ca score lan level bang nhau) -
//      dung MODIFICATION TIME cua file save lam moc xap xi (file save
//      KHONG luu timestamp rieng, xem ghi chu chi tiet trong .cpp)
//
// Khung ngoai dung GoldenBox (9-slice ca 4 canh), moi dong xep hang dung
// SilverBox (3-slice ngang, giong ContinueMenu). Moi dong gom: avatar
// (dau nhan vat, khung mau theo do kho) - ten nhan vat - diem - level -
// mode. Danh sach cuon duoc (chuot lan/phim Up-Down) khi noi dung vuot
// qua chieu cao khung.
//==================================================

class LeaderboardMenu : public Menu
{
private:

    static const int MODE_COUNT = 3;
    static const int MAP_COUNT = 4;
    static const int CHAR_COUNT = 4;

    MenuBackground background;

    sf::Text titleText;
    sf::Text emptyHintText;   // hien khi khong co save nao

    const sf::Font* font = nullptr;

    const sf::Texture* panelTexture = nullptr;    // GoldenBox - khung ngoai (9-slice)
    const sf::Texture* rowBoxTexture = nullptr;   // SilverBox - nen tung dong (3-slice ngang)

    sf::Texture characterTextures[CHAR_COUNT];    // 0=Chicken 1=Knight 2=Dog 3=Luffy (khop main.cpp)
    sf::Texture modeFrameTextures[MODE_COUNT];    // 0=Easy 1=Hard 2=Nightmare

    const sf::Texture* buttonTexture = nullptr;
    float btnScaleX = 1.f, btnScaleY = 1.f;
    float btnRenderW = 240.f, btnRenderH = 100.f;

    Button backButton;

    // 1 dong da xep hang (sau khi doc + sort)
    struct Entry
    {
        int map = 0;
        int mode = 0;
        int character = 0;
        int level = 1;
        int score = 0;
        long long savedTimeTicks = 0;   // xem ghi chu o dau .cpp
    };
    std::vector<Entry> entries;

    // Cac sf::Sprite/sf::Text da dung san cho tung dong, vi tri tinh theo
    // toa do LOCAL cua danh sach (0,0 la dinh danh sach, KHONG phai toa
    // do man hinh) - dung chung voi ky thuat cuon bang sf::View
    struct RowVisual
    {
        sf::Sprite avatarFrame;
        sf::Sprite avatarHead;
        sf::Text rankText;
        sf::Text nameText;
        sf::Text scoreText;
        sf::Text levelText;
        sf::Text modeText;
        float y = 0.f;
    };
    std::vector<RowVisual> rows;

    float scrollOffset = 0.f;
    float maxScroll = 0.f;

    LeaderboardMenuResult result = LeaderboardMenuResult::None;

    AudioManager* audio = nullptr;

    float W = 1280.f;
    float H = 720.f;

    // Cache layout - tinh trong setupLayout(), dung lai o rebuildRows()/draw()
    sf::FloatRect panelRect;
    sf::FloatRect listRect;   // vung cuon duoc, BEN TRONG panel (da tru padding)

    void setupLayout();
    void rebuildRows();   // dung entries[] + layout hien tai de tao lai rows[]
    void clampScroll();

    static void centerText(sf::Text& t, float cx, float cy);       // can giua CA 2 truc
    static void leftAlignText(sf::Text& t, float x, float cy);     // can trai, giua theo truc doc

public:

    LeaderboardMenu();

    void setAudioManager(AudioManager* manager);

    void setWindowSize(float w, float h);
    void setBackgroundTexture(const sf::Texture& tex, float sx, float sy);
    void setFont(const sf::Font& f);
    void setPanelTexture(const sf::Texture& tex);     // GoldenBox
    void setRowBoxTexture(const sf::Texture& tex);    // SilverBox
    void setButtonTexture(const sf::Texture& tex, float scaleX, float scaleY);

    // index: 0=Chicken 1=Knight 2=Dog 3=Luffy (PHAI khop thu tu main.cpp
    // dang goi, cung la thu tu CGAME::PeekSaveInfo tra ve o outCharacter)
    void loadCharacterTexture(int index, const std::string& path);

    // modeIndex: 0=Easy 1=Hard 2=Nightmare
    void loadModeFrameTexture(int modeIndex, const std::string& path);

    // Quet lai 12 file save (4 map x 3 mode), gop + sap xep, dung goi
    // truoc khi hien menu nay (vd ngay sau khi bam LEADERBOARD tu MainMenu)
    void refresh();

    //----------------------------------
    // Menu
    //----------------------------------

    void processEvent(const sf::Event& event,
        const sf::RenderWindow& window) override;

    void update(float dt) override;

    void draw(sf::RenderWindow& window) const override;

    //----------------------------------
    // Result
    //----------------------------------

    LeaderboardMenuResult getResult() const;
    void clearResult();
};
