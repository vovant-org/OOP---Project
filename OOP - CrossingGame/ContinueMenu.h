// ContinueMenu.h
#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

#include "Menu.h"
#include "Button.h"
#include "MenuBackground.h"

class AudioManager;

//==================================================
// Result
//==================================================

enum class ContinueMenuResult
{
    None = 0,
    Selected,
    Back
};

// ===== CHANGED: moi map GIO co 3 save rieng theo difficultyMode
// (0=Easy 1=Hard 2=Nightmare) - ContinueMenu hien thi 3 cot, moi cot
// la 1 mode, ben duoi cot la cac map DA CO save trong mode do =====
class ContinueMenu : public Menu
{
private:

    static const int MODE_COUNT = 3;

    MenuBackground background;

    sf::Text titleText;
    sf::Text hintText;   // hien khi KHONG map/mode nao co save

    const sf::Font* font = nullptr;

    const sf::Texture* buttonTexture = nullptr;
    float btnScaleX = 1.f, btnScaleY = 1.f;
    float btnRenderW = 240.f, btnRenderH = 100.f;   // kich thuoc thuc te sau scale, dung de can giua

    const sf::Texture* modeBoxTexture = nullptr;    // SilverBox - lam nen cho 3 header mode

    // Moi mode (cot) co 1 danh sach nut save rieng - so luong thay doi
    // tuy theo mode do co bao nhieu map da luu
    std::vector<Button> saveButtons[MODE_COUNT];
    std::vector<int> saveButtonMapIndex[MODE_COUNT];   // map index tuong ung tung nut, cung cot

    sf::Text modeLabelText[MODE_COUNT];
    sf::Text modeEmptyHint[MODE_COUNT];   // "Chua co save" khi cot rong

    Button backButton;

    // Vi tri dang chon: cot (mode) + hang (index trong saveButtons[col]),
    // hoac onBack = true neu dang chon nut Back
    int selectedCol = 0;
    int selectedRow = 0;
    bool onBack = false;

    ContinueMenuResult result = ContinueMenuResult::None;
    int selectedMapIndex = -1;
    int selectedModeIndex = -1;   // ===== ADDED: mode cua save vua chon =====

    AudioManager* audio = nullptr;

    float W = 1280.f;
    float H = 720.f;

    void updateFocus();
    void moveHorizontal(int dir);   // -1 = trai, +1 = phai (doi cot)
    void moveVertical(int dir);     // -1 = len,  +1 = xuong (doi hang / Back)
    void activateSelected();

    int totalButtons() const;
    float columnCenterX(int mode) const;
    sf::FloatRect headerRect(int mode) const;

    static void centerText(sf::Text& t, float cx, float y);
    static void centerTextFull(sf::Text& t, float cx, float cy);

public:

    ContinueMenu();

    void setAudioManager(AudioManager* manager);

    void setWindowSize(float w, float h);
    void setBackgroundTexture(const sf::Texture& tex, float sx, float sy);
    void setFont(const sf::Font& f);
    void setButtonTexture(const sf::Texture& tex, float scaleX, float scaleY);
    void setModeBoxTexture(const sf::Texture& tex);   // ===== ADDED: SilverBox cho header =====

    // Quet lai toan bo 12 file save (4 map x 3 mode, thong qua
    // CGAME::PeekSaveInfo), dung goi moi lan chuan bi hien menu nay
    // (VD: ngay truoc khi setState)
    void refresh();

    int getSelectedMapIndex() const { return selectedMapIndex; }
    int getSelectedMode() const { return selectedModeIndex; }   // ===== ADDED =====

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

    ContinueMenuResult getResult() const;
    void clearResult();
};
