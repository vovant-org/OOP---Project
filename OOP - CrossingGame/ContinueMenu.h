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

class ContinueMenu : public Menu
{
private:

    MenuBackground background;

    sf::Text titleText;
    sf::Text hintText;   // hien khi chua co save nao

    const sf::Font* font = nullptr;
    const sf::Texture* buttonTexture = nullptr;
    float btnScaleX = 1.f, btnScaleY = 1.f;

    // 1 nut cho moi map DANG CO save (danh sach nay duoc dung lai moi
    // khi refresh() - so luong thay doi tuy theo save nao ton tai)
    std::vector<Button> saveButtons;
    std::vector<int> saveButtonMapIndex;   // map index tuong ung tung nut

    Button backButton;

    // selectedIndex: 0..saveButtons.size()-1 la cac nut save,
    // == saveButtons.size() nghia la dang chon Back
    int selectedIndex = 0;

    ContinueMenuResult result = ContinueMenuResult::None;
    int selectedMapIndex = -1;

    AudioManager* audio = nullptr;

    float W = 1280.f;
    float H = 720.f;

    void updateFocus();
    void moveSelectionUp();
    void moveSelectionDown();
    void activateSelected();

    static void centerText(sf::Text& t, float cx, float y);

public:

    ContinueMenu();

    void setAudioManager(AudioManager* manager);

    void setWindowSize(float w, float h);
    void setBackgroundTexture(const sf::Texture& tex, float sx, float sy);
    void setFont(const sf::Font& f);
    void setButtonTexture(const sf::Texture& tex, float scaleX, float scaleY);

    // Quet lai 4 file save (thong qua CGAME::PeekSaveInfo), dung goi
    // moi lan chuan bi hien menu nay (VD: ngay truoc khi setState)
    void refresh();

    int getSelectedMapIndex() const { return selectedMapIndex; }

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
