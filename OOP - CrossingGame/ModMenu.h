// ModMenu.h
#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <array>

#include "Menu.h"
#include "Button.h"
#include "MenuBackground.h"

class AudioManager;
class ModManager;

//==================================================
// Result
//==================================================
enum class ModMenuResult
{
    None = 0,
    Back
};

// ===== ADDED: 1 nhan vat co the duoc chon lam muc tieu MOD =====
struct ModCharacterInfo
{
    std::string name;
    std::string path;
};

//==================================================
// ModMenu
//
// Man hinh "MOD" (tinh nang KHONG bat buoc, giang vien cho phep them):
// cho nguoi choi tu import 1 file anh tu may de thay the skin CHO BAT
// KY nhan vat nao trong 4 nhan vat (dung < > hoac phim TRAI/PHAI de
// chon nhan vat, giong kieu chuyen doi cua CharacterSelection) - kieu
// nhu 1 "modpack" don gian. Phan xu ly anh/file thuc su nam o
// ModManager (xem ModManager.h/.cpp), ModMenu chi la lop UI.
//==================================================
class ModMenu : public Menu
{
private:

    MenuBackground background;

    ModManager* modManager = nullptr;

    // ===== ADDED: danh sach nhan vat co the MOD + nhan vat dang chon =====
    static constexpr int MOD_CHARACTER_COUNT = 4;
    std::array<ModCharacterInfo, MOD_CHARACTER_COUNT> modCharacters = { {
        { "Chicken", "Character/Chicken_character.png" },
        { "Knight",  "Character/Knight_character.png"  },
        { "Dog",     "Character/Dog_character.png"     },
        { "Luffy",   "Character/Luffy_character.png"   }
    } };
    int selectedCharIndex = 0;

    //--------------------------------------------------
    // Buttons
    //--------------------------------------------------

    Button importButton;
    Button resetButton;
    Button backButton;

    // ===== ADDED: nut < > chuyen nhan vat, giong CharacterSelection =====
    sf::Texture leftArrowTexture;
    Button prevButton;
    Button nextButton;

    //--------------------------------------------------
    // Preview skin hien tai (frame dau cua spritesheet Chicken)
    //--------------------------------------------------

    sf::Texture previewTexture;
    sf::Sprite previewSprite;
    int previewFrameW = 0;
    int previewFrameH = 0;

    sf::RectangleShape previewBox;
    sf::RectangleShape previewBorder;

    //--------------------------------------------------
    // Text
    //--------------------------------------------------

    sf::Font font;
    bool hasFont = false;

    sf::Text titleText;
    sf::Text hintText;
    sf::Text statusText;   // ket qua lan Import/Reset gan nhat
    sf::Text charNameText; // ===== ADDED: ten nhan vat dang duoc chon de MOD

    ModMenuResult result = ModMenuResult::None;

    AudioManager* audio = nullptr;

    float W = 1280.f;
    float H = 720.f;

    void centerText(sf::Text& t, float cx, float y);

    // Doc lai file skin cua nhan vat DANG DUOC CHON (modCharacters[selectedCharIndex])
    // tu dia de cap nhat preview - goi lai sau moi lan Import/Reset/doi
    // nhan vat thanh cong.
    void reloadPreview();

    // ===== ADDED: chuyen nhan vat dang duoc MOD (giong selectPrev/Next
    // cua CharacterSelection) - cap nhat modManager->SetTarget() roi
    // reload lai preview =====
    void selectPrev();
    void selectNext();
    void updateCharNameText();

public:

    ModMenu();

    void setAudioManager(AudioManager* manager);
    void setModManager(ModManager* manager);

    void setWindowSize(float w, float h);
    void setBackgroundTexture(const sf::Texture& tex, float sx, float sy);

    bool loadFont(const std::string& path);

    // ===== ADDED: nap texture mui ten < > (dung chung cho prevButton/
    // nextButton, giong CharacterSelection/MapSelection) =====
    bool loadArrowTexture(const std::string& path);

    void setupButtons(const sf::Texture& buttonTex, float scaleX, float scaleY);
    void setupLayout();

    // Goi moi lan vao man hinh MOD (tu MainMenu) de preview luon dung
    // skin moi nhat va xoa thong bao cu.
    void refresh();

    void processEvent(const sf::Event& event,
        const sf::RenderWindow& window) override;

    void update(float dt) override;

    void draw(sf::RenderWindow& window) const override;

    ModMenuResult getResult() const;
    void clearResult();
};
