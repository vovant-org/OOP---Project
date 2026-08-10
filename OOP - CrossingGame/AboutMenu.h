// AboutMenu.h
#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "Menu.h"
#include "Button.h"
#include "MenuBackground.h"

class AudioManager;

//==================================================
// Result
//==================================================

enum class AboutMenuResult
{
    None = 0,
    Back
};

//==================================================
// AboutMenu
//
// ===== REDESIGNED: khung ngoai GoldenBox (9-slice), ben trong la 1 DANH
// SACH 3 "card" xep chong (Mo ta game / Dieu khien / Credits), moi card
// nen SilverBox (3-slice ngang, giong ContinueMenu/LeaderboardMenu).
// Danh sach cuon duoc bang phim Up/Down (hoac chuot lan) khi tong chieu
// cao 3 card vuot qua khung hien thi - dung chung ky thuat sf::View
// clip+scroll da dung trong LeaderboardMenu. Chi co nut BACK la thanh
// phan tuong tac (con lai la text trang tri).
//==================================================

class AboutMenu : public Menu
{
private:

    static const int SECTION_COUNT = 3;

    MenuBackground background;

    sf::Text titleText;   // "ABOUT"

    // 1 card = 1 ThinSliverBox nen + header (ten muc, luon hien) + body
    // (noi dung nhieu dong, chi hien khi card dang MO RONG) + 1 nut mui
    // ten (DownArrow) o ben phai de dong/mo card.
    // ===== REDESIGNED: card mac dinh THU GON (chi thay header + arrow),
    // bam vao arrow se MO RONG card ra du cao de chua het body, arrow
    // xoay nguoc lai (huong len); bam lan nua se THU GON ve ban dau =====
    struct SectionCard
    {
        sf::Text header;
        sf::Text body;
        float y = 0.f;        // vi tri LOCAL (dinh danh sach = 0) cua card
        float height = 0.f;   // chieu cao HIEN TAI cua card (thu gon/mo rong)

        float collapsedHeight = 0.f;   // chieu cao khi THU GON (chi header)
        float expandedHeight = 0.f;   // chieu cao khi MO RONG (header + body)

        bool expanded = false;   // trang thai hien tai cua card

        sf::Vector2f arrowCenter;   // tam icon mui ten, toa do LOCAL (giong sc.y)

        // ===== Mui ten KHONG dung class Button that: Button::processEvent
        // tu doi chieu chuot bang window.mapPixelToCoords(...) theo VIEW
        // HIEN TAI dang gan tren window, nhung view do chi duoc doi thanh
        // "view cuon" (listView) BEN TRONG AboutMenu::draw() (nhan
        // sf::RenderWindow& khong const) - con AboutMenu::processEvent()
        // nhan sf::RenderWindow CONST nen KHONG the goi window.setView(),
        // tuc la khong the tam thoi chuyen sang toa do cuc bo cua danh
        // sach de Button do dung. Vi vay o day tu tinh hover/click bang
        // tay (xem tryToggleCardAt + block hover ben duoi), nhung VAN
        // dung dung bo mau hover/pressed cua Button (xem ARROW_*_COLOR)
        // de nhin dong bo voi cac nut khac =====
        bool arrowHovered = false;      // rê chuot qua mui ten? (cap nhat moi lan processEvent)
        sf::Clock arrowPressClock;      // dem thoi gian tu lan bam gan nhat, de "nhap nhay" mau
    };
    SectionCard sections[SECTION_COUNT];

    const sf::Font* font = nullptr;
    const sf::Texture* panelTexture = nullptr;   // GoldenBox - khung ngoai (9-slice)
    const sf::Texture* sectionBoxTexture = nullptr;   // ThinSliverBox - nen tung card (3-slice ngang)
    const sf::Texture* arrowTexture = nullptr;   // DownArrow - nut thu gon/mo rong tung card

    Button backButton;

    float scrollOffset = 0.f;
    float maxScroll = 0.f;

    AboutMenuResult result = AboutMenuResult::None;

    AudioManager* audio = nullptr;

    float W = 1280.f;
    float H = 720.f;

    sf::FloatRect panelRect;
    sf::FloatRect listRect;   // vung cuon duoc, BEN TRONG panel (da tru padding)

    void setupLayout();
    void clampScroll();
    bool tryToggleCardAt(const sf::Vector2f& localPoint);   // localPoint: toa do trong listRect (da tru scroll)

    static void centerText(sf::Text& t, float cx, float cy);   // can giua CA 2 truc

public:

    AboutMenu();

    void setAudioManager(AudioManager* manager);

    void setWindowSize(float w, float h);
    void setBackgroundTexture(const sf::Texture& tex, float sx, float sy);
    void setFont(const sf::Font& f);
    void setPanelTexture(const sf::Texture& tex);         // GoldenBox
    void setSectionBoxTexture(const sf::Texture& tex);    // ThinSliverBox
    void setArrowTexture(const sf::Texture& tex);         // DownArrow - nut thu gon/mo rong
    void setButtonTexture(const sf::Texture& tex, float scaleX, float scaleY);

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

    AboutMenuResult getResult() const;
    void clearResult();
};
