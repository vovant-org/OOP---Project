// ContinueMenu.h
#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <array>
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

// ===== CHANGED (Giai doan 2 - Continue Menu redesign): danh sach save
// GIO la DONG (khong con gioi han 4 slot co dinh save1..save4.sav nua),
// lay tu CGAME::ListAllSaves() (quet toan bo thu muc Save/). Vi vay
// danh sach co the dai hon vung hien thi, nen them co che CUON (scroll)
// bang mot sf::View rieng chi ap dung cho vung liet ke, con title/hint/
// Back button van co dinh tren man hinh =====
class ContinueMenu : public Menu
{
private:
    MenuBackground background;

    sf::Text titleText;
    sf::Text hintText;   // hien khi KHONG co file save nao trong Save/

    const sf::Font* font = nullptr;

    const sf::Texture* buttonTexture = nullptr;
    float btnScaleX = 1.f, btnScaleY = 1.f;
    float btnRenderW = 240.f, btnRenderH = 100.f;   // kich thuoc thuc te sau scale, dung de can giua

    const sf::Texture* modeBoxTexture = nullptr;    // SilverBox - danh cho toolbar/header o giai doan sau
    const sf::Texture* slotBoxTexture = nullptr;    // Save slot background

    // Slots (danh sach doc, so luong = so file .sav tim thay trong Save/)
    std::vector<Button> slotButtons;

    struct SlotInfo
    {
        bool exists = false;    // luon true (ListAllSaves() chi tra file THAT SU ton tai tren dia)
        // ===== ADDED (Giai doan 1/2): tu CGAME::SaveData - danh dau file
        // co parse duoc DU du lieu hay khong. exists=true nhung
        // isValid=false nghia la file bi hong/sai dinh dang (VD file
        // .sav kieu cu) - UI se hien "INVALID SAVE" thay vi crash hoac
        // an di, giong cach Thrive hien "Version: Invalid" =====
        bool isValid = false;

        std::string filePath;              // duong dan THAT, dung de Load/Delete dung file
        long long fileSizeBytes = 0;       // du cho thanh trang thai "Space used" (Giai doan 5)
        long long lastWriteTimeUnix = 0;   // du cho hien thi thoi gian sua doi (Giai doan 3)

        int characterIndex = 0;
        std::string playerName;
        int mapIndex = 0;
        int score = 0;
        int difficultyMode = 1;
        int level = 1;
        std::string saveTime;

        // ===== ADDED (preview dung trang thai cuoi cung): huong/frame
        // cua nhan vat tai thoi diem save, lay tu CGAME::SaveData =====
        int lastDirection = 1;
        int lastFrame = 0;
    };
    // ===== CHANGED: std::array<SlotInfo,4> co dinh -> std::vector danh
    // sach dong, kich thuoc = slotButtons.size() =====
    std::vector<SlotInfo> slotInfo;

    std::array<sf::Texture, 4> charTextures;
    bool charTexturesLoaded = false;

    Button backButton;

    // Vi tri dang chon: index trong slotButtons/slotInfo, hoac onBack=true
    int selectedIndex = 0;
    bool onBack = false;

    ContinueMenuResult result = ContinueMenuResult::None;
    // ===== CHANGED: doi ten tu selectedSlotIndex -> selectedRowIndex de
    // ro rang day la index trong danh sach DONG (khong con la 1 trong 4
    // slot co dinh nua) =====
    int selectedRowIndex = -1;

    AudioManager* audio = nullptr;

    float W = 1280.f;
    float H = 720.f;

    // ===== ADDED (Giai doan 2): trang thai cuon =====
    // So pixel noi dung da cuon xuong tinh tu dau danh sach (0 = dau danh sach)
    float scrollOffset = 0.f;

    // helper to expose currently selected save path
    std::string selectedSavePath;

    void updateFocus();
    void moveVertical(int dir);     // -1 = len, +1 = xuong (doi hang / Back)
    void activateSelected();

    // ===== ADDED (Giai doan 2: layout + scroll) =====
    // Vung hien thi danh sach save theo toa do thiet ke cua menu nay
    // (0..W ngang, 0..H doc) - nam duoi title, tren nut Back
    sf::FloatRect listAreaDesign() const;

    // Tao 1 sf::View "long" (nested) ben trong view HIEN TAI cua window
    // (co the da bi letterbox/fullscreen), chi bao phu dung vung
    // listAreaDesign() tren man hinh, va "nhin" vao toa do NOI DUNG
    // (local, bat dau tu 0) da bi dich theo scrollOffset. Dung ca trong
    // draw() (de ve) lan trong processEvent()/xu ly click (de map toa
    // do chuot cho dung, vi processEvent() nhan window dang const nen
    // KHONG the goi window.setView() o do - chi dung ham nay de TINH
    // toan, khong set that len window).
    sf::View computeListView(const sf::RenderWindow& window) const;

    float contentHeight() const;        // tong chieu cao noi dung (theo so hang)
    float maxScrollOffset() const;      // gioi han cuon toi da (0 neu noi dung vua khung nhin)
    void clampScroll();                 // ep scrollOffset ve [0, maxScrollOffset()]
    void ensureRowVisible(int index);   // tu dong cuon de hang dang chon (index) nam trong khung nhin

    void drawScrollbar(sf::RenderWindow& window) const;

    // ===== ADDED (Redesign: navy/cyan compact save list): tra ve hinh
    // chu nhat cua hang save thu i va cua nut DELETE rieng cua hang do,
    // theo toa do NOI DUNG cuc bo (giong he toa do dung trong draw() va
    // trong computeListView()). Dung chung 1 nguon tinh toan duy nhat
    // cho ca ve (draw) lan bat click (processEvent) de tranh lech vung
    // click so voi vung ve nhu truoc day (slotButtons dung kich thuoc
    // texture button goc, khong khop slotW/SLOT_H thuc te) =====
    sf::FloatRect slotRect(std::size_t index) const;
    sf::FloatRect deleteButtonRect(std::size_t index) const;

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
    void setSlotTexture(const sf::Texture& tex);

    // ===== CHANGED (Giai doan 2): quet lai TOAN BO thu muc Save/ qua
    // CGAME::ListAllSaves() (khong con gioi han 4 slot co dinh), dung
    // goi moi lan chuan bi hien menu nay (VD: ngay truoc khi setState) =====
    void refresh();

    // ===== CHANGED (Giai doan 2): thay the getSelectedSlotIndex() (gia
    // dinh 4 slot co dinh save1..save4.sav, khong con dung voi danh sach
    // dong) bang getSelectedSavePath() tra ve duong dan THAT cua file
    // duoc chon. Tra ve chuoi rong neu chua chon gi. main.cpp can doi
    // sang goi game.LoadGame(continueMenu.getSelectedSavePath()) thay vi
    // game.LoadGame(CGAME::GetSavePathForSlot(slot)) =====
    std::string getSelectedSavePath() const;

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