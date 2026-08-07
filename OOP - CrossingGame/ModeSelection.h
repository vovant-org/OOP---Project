// ModeSelection.h
// Màn chọn độ khó — hiện ra sau khi chọn map, trước khi vào gameplay
#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include <string>

#include "Menu.h"
#include "Button.h"
#include "MenuBackground.h"

class AudioManager;

//==================================================
// Kết quả
//==================================================

enum class ModeSelectionResult
{
    None = 0,
    Selected,
    Back
};

//==================================================
// Trang thai con CHI danh cho Nightmare - sau khi chon Nightmare o luoi
// chinh, hien them buoc chon "Adventure" (choi du lieu co san) hoac
// "Custom" (tu nhap Level muon vuot qua de THANG, 1..999, van bat dau
// choi tu level 1 nhu Adventure, moi qua 1 level +100 diem)
//==================================================

enum class NightmareSubStep
{
    Grid = 0,     // dang o luoi chinh (Easy/Hard/Nightmare)
    ChooseType,   // da chon Nightmare, dang chon Adventure/Custom
    EnterLevel    // da chon Custom, dang nhap Level
};

//==================================================
// Dữ liệu từng mode
//==================================================

struct ModeInfo
{
    std::string name;
    sf::Color   color;   // màu tint cho SilverBox + chữ
};

class ModeSelection : public Menu
{
private:

    MenuBackground background;

    static constexpr int MODE_COUNT = 3;

    std::array<ModeInfo, MODE_COUNT> modeInfos =
    { {
        { "EASY",      sf::Color(154, 205, 50)  },   // xanh la chuoi
        { "HARD",      sf::Color(220, 20, 60)   },   // do
        { "NIGHTMARE", sf::Color(148, 0, 211)   }    // tim tuoi dam
    } };

    //--------------------------------------------------
    // Box (dung chung 1 texture SilverBox, tint mau rieng)
    //--------------------------------------------------

    sf::Texture boxTexture;
    std::array<sf::Sprite, MODE_COUNT> boxSprites;
    std::array<sf::Text, MODE_COUNT>   nameTexts;
    std::array<sf::RectangleShape, MODE_COUNT> highlightBorders;

    int selectedIndex = 0;

    //--------------------------------------------------
    // ===== ADDED: luong con rieng cho Nightmare (Adventure/Custom) =====
    //--------------------------------------------------

    NightmareSubStep nightmareStep = NightmareSubStep::Grid;

    // 0 = Adventure, 1 = Custom - dang duoc highlight trong buoc ChooseType
    int nightmareTypeIndex = 0;

    bool customNightmare = false;    // true neu nguoi choi chon Custom
    int  customStartLevel = 1;       // ===== CHANGED: gio la LEVEL MUON VUOT QUA DE WIN
                                      // (da validate 1..999) - level choi van bat dau tu 1
    std::string customLevelInput;    // buffer dang go, chi chua chu so, toi da 3 ky tu

    Button adventureButton;
    Button customButton;
    Button confirmButton;    // dung o buoc EnterLevel, thay cho playButton

    sf::Text nightmareChoiceTitle;   // tieu de buoc ChooseType
    sf::Text customPromptText;       // "Nhap Level (1 - 999):"
    sf::Text customInputText;        // hien so nguoi choi dang go (co con tro nhap)
    sf::Text customErrorText;        // thong bao loi khi nhap sai, rong neu khong loi

    void selectNightmareTypePrev();
    void selectNightmareTypeNext();
    void updateNightmareTypeHighlight();
    void goBackStep();               // Escape/BACK: lui 1 buoc tuy nightmareStep
    void tryConfirmCustomLevel();    // validate + xac nhan buoc EnterLevel
    void applyStepLayout();          // dat lai vi tri nut/text theo nightmareStep hien tai
    void updateCustomInputDisplay(); // ve lai customInputText tu customLevelInput

    //--------------------------------------------------
    // Buttons
    //--------------------------------------------------

    Button playButton;
    Button backButton;

    //--------------------------------------------------
    // Text
    //--------------------------------------------------

    sf::Font font;
    sf::Text titleText;
    sf::Text hintText;

    //--------------------------------------------------
    // Window
    //--------------------------------------------------

    float W = 1280.f;
    float H = 720.f;

    ModeSelectionResult result = ModeSelectionResult::None;

    AudioManager* audio = nullptr;

    //--------------------------------------------------
    // Helpers
    //--------------------------------------------------

    void updateHighlight();
    void selectPrev();
    void selectNext();
    void centerText(sf::Text& t, float cx, float y);

public:

    ModeSelection();

    void setAudioManager(AudioManager* manager);

    void setWindowSize(float w, float h);
    void setBackgroundTexture(const sf::Texture& tex, float sx, float sy);
    bool loadFont(const std::string& path);

    // Tai texture SilverBox (co the tai chinh anh dang dung o SettingMenu)
    bool loadBoxTexture(const std::string& path);

    void setupButtons(const sf::Texture& buttonTex,
        float btnW, float btnH,
        float scaleX, float scaleY);

    void setupLayout();

    ModeSelectionResult getResult() const;
    void clearResult();

    // 0 = Easy, 1 = Hard, 2 = Nightmare
    int getSelectedMode() const;

    // ===== ADDED: chi co y nghia khi getSelectedMode()==2 (Nightmare) =====
    // true  -> nguoi choi chon "Custom": VAN bat dau choi tu level 1 nhu
    //          Adventure, nhung se THANG ngay khi vuot qua getCustomStartLevel()
    //          (CGAME::SetStartingLevel() dung gia tri nay lam moc thang)
    // false -> nguoi choi chon "Adventure": choi du lieu co san nhu binh thuong
    bool isCustomNightmare() const;
    int  getCustomStartLevel() const;   // ten ham giu nguyen de tuong thich,
                                         // nhung gia tri tra ve la MOC THANG

    // Goi ham nay MOI LAN mo lai man ModeSelection (vd sau khi quay lai
    // tu MapSelection) de dam bao luon bat dau lai tu luoi chinh, khong
    // con giu trang thai ChooseType/EnterLevel cua lan truoc
    void resetNightmareFlow();

    void processEvent(const sf::Event& event,
        const sf::RenderWindow& window) override;

    void update(float dt) override;

    void draw(sf::RenderWindow& window) const override;
};
