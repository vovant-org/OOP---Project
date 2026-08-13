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

// ===== CHANGED: moi map GIO co 3 save rieng theo difficultyMode
// (0=Easy 1=Hard 2=Nightmare) - ContinueMenu hien thi 3 cot, moi cot
// la 1 mode, ben duoi cot la cac map DA CO save trong mode do =====
class ContinueMenu : public Menu
{
private:
    static const int SLOT_COUNT = 4;

    MenuBackground background;

    sf::Text titleText;
    sf::Text hintText;   // hien khi KHONG map/mode nao co save

    const sf::Font* font = nullptr;

    const sf::Texture* buttonTexture = nullptr;
    float btnScaleX = 1.f, btnScaleY = 1.f;
    float btnRenderW = 240.f, btnRenderH = 100.f;   // kich thuoc thuc te sau scale, dung de can giua

    const sf::Texture* modeBoxTexture = nullptr;    // SilverBox - lam nen cho 3 header mode
    const sf::Texture* slotBoxTexture = nullptr;    // Save slot background

    // Slots (vertical list)
    std::vector<Button> slotButtons; // size = SLOT_COUNT
    // cached save data for each slot
    // forward declare CGAME::SaveData by opaque pointer to avoid include cycle
    // we'll include CGAME in cpp
    // store minimal info in local struct
    struct SlotInfo
    {
        bool exists = false;
        int characterIndex = 0;
        std::string playerName;
        int mapIndex = 0;
        int score = 0;
        int difficultyMode = 1;
        int level = 1;
        std::string saveTime;
    };
    std::array<SlotInfo, SLOT_COUNT> slotInfo;
    std::array<sf::Texture, 4> charTextures;
    bool charTexturesLoaded = false;

    Button backButton;

    // Vi tri dang chon: index cua slot (0..SLOT_COUNT-1) hoac onBack=true
    int selectedIndex = 0;
    bool onBack = false;

    ContinueMenuResult result = ContinueMenuResult::None;
    int selectedSlotIndex = -1;

    AudioManager* audio = nullptr;

    float W = 1280.f;
    float H = 720.f;

    void updateFocus();
    void moveHorizontal(int dir);   // -1 = trai, +1 = phai (doi cot)
    void moveVertical(int dir);     // -1 = len,  +1 = xuong (doi slot / Back)
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
    void setSlotTexture(const sf::Texture& tex);

    // Quet lai toan bo 12 file save (4 map x 3 mode, thong qua
    // CGAME::PeekSaveInfo), dung goi moi lan chuan bi hien menu nay
    // (VD: ngay truoc khi setState)
    void refresh();
    int getSelectedSlotIndex() const { return selectedSlotIndex; }

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
