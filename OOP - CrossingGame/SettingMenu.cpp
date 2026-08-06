#include "SettingMenu.h"
#include "AudioManager.h"
#include <string>

void SettingMenu::setAudioManager(AudioManager* manager)
{
    audio = manager;
}

//==================================================
// Layout Constants (internal to this file)
//
// Computed from the real texture sizes:
//   GoldenBox.png   1254 x 1254
//   SliverBox.png   1537 x 1023
//   Plus.png        1254 x 1254
//   Minus.png       1254 x 1254
//   Switch_ON.png   1254 x 1254
//   Switch_OFF.png  1254 x 1254
//   Times.png       1254 x 1254
//==================================================

namespace
{
    constexpr float WINDOW_WIDTH = 1920.f;
    constexpr float WINDOW_HEIGHT = 1080.f;

    constexpr float CENTER_X = WINDOW_WIDTH / 2.f;

    //----------------------------------
    // Panel (GoldenBox.png, 1254 x 1254)
    //----------------------------------

    constexpr float PANEL_TEX_SIZE = 1254.f;

    // ===== CHANGED: mở rộng panel theo chiều ngang, giữ nguyên chiều cao =====
    constexpr float PANEL_WIDTH = 950.f;
    constexpr float PANEL_HEIGHT = 950.f;

    constexpr float PANEL_SCALE_X = PANEL_WIDTH / PANEL_TEX_SIZE;
    constexpr float PANEL_SCALE_Y = PANEL_HEIGHT / PANEL_TEX_SIZE;

    constexpr float PANEL_X = CENTER_X - PANEL_WIDTH / 2.f;
    constexpr float PANEL_Y = 110.f;

    //----------------------------------
    // Content area (lề trái/phải bên trong panel để bố trí label/value)
    //----------------------------------

    constexpr float CONTENT_MARGIN = 90.f;
    constexpr float CONTENT_LEFT = PANEL_X + CONTENT_MARGIN;
    constexpr float CONTENT_RIGHT = PANEL_X + PANEL_WIDTH - CONTENT_MARGIN;

    //----------------------------------
    // Title (SETTING.png, 1827 x 861 — chữ "SETTING" vẽ sẵn trong ảnh)
    //----------------------------------

    constexpr float TITLE_TEX_WIDTH = 1827.f;
    constexpr float TITLE_TEX_HEIGHT = 861.f;

    constexpr float TITLE_Y = 225.f;

    constexpr float TITLE_WIDTH = 330.f;
    constexpr float TITLE_HEIGHT =
        TITLE_WIDTH * (TITLE_TEX_HEIGHT / TITLE_TEX_WIDTH);

    constexpr float TITLE_SCALE = TITLE_WIDTH / TITLE_TEX_WIDTH;

    constexpr float TITLE_X = CENTER_X - TITLE_WIDTH / 2.f;
    constexpr float TITLE_TOP_Y = TITLE_Y - TITLE_HEIGHT / 2.f;

    //----------------------------------
    // Rows
    //----------------------------------

    constexpr unsigned int LABEL_CHAR_SIZE = 35;

    constexpr float MUSIC_ROW_Y = 310.f;
    constexpr float SOUND_ROW_Y = 420.f;
    constexpr float FULLSCREEN_ROW_Y = 530.f;
    constexpr float FPS_ROW_Y = 640.f;
    constexpr float RESOLUTION_ROW_Y = 750.f;

    //----------------------------------
    // Plus / Minus Icons (1254 x 1254)
    //----------------------------------

    constexpr float ICON_TEX_SIZE = 1254.f;

    constexpr float ICON_SIZE = 50.f;
    constexpr float ICON_SCALE = ICON_SIZE / ICON_TEX_SIZE;

    constexpr float ICON_GAP = -2.f;

    //----------------------------------
    // Value Boxes (SliverBox.png, 1537 x 1023)
    // Căn theo mép phải nội dung: [minus][box][plus] áp sát CONTENT_RIGHT
    //----------------------------------

    constexpr float BOX_TEX_WIDTH = 1537.f;
    constexpr float BOX_TEX_HEIGHT = 1023.f;

    constexpr float BOX_WIDTH = 180.f;
    constexpr float BOX_HEIGHT = 160.f;

    constexpr float BOX_SCALE_X = BOX_WIDTH / BOX_TEX_WIDTH;
    constexpr float BOX_SCALE_Y = BOX_HEIGHT / BOX_TEX_HEIGHT;

    constexpr float BOX_X = CONTENT_RIGHT - ICON_SIZE - ICON_GAP - BOX_WIDTH;

    // ===== ADDED: lệch icon xuống để trùng trục ngang (vertical center) với box =====
    constexpr float ICON_Y_OFFSET = (BOX_HEIGHT - ICON_SIZE) / 2.f;

    constexpr float RES_BOX_WIDTH = 255.f;
    constexpr float RES_BOX_HEIGHT = 160.f;

    constexpr float RES_BOX_SCALE_X = RES_BOX_WIDTH / BOX_TEX_WIDTH;
    constexpr float RES_BOX_SCALE_Y = RES_BOX_HEIGHT / BOX_TEX_HEIGHT;

    constexpr float RES_BOX_X =
        CONTENT_RIGHT - ICON_SIZE - ICON_GAP - RES_BOX_WIDTH;

    //----------------------------------
    // Label Boxes (SilverBox.png làm nền cho label — CHỈ TRANG TRÍ,
    // không nằm trong danh sách isMouseOver nên không tương tác được)
    //
    // ===== FIX: trước đây label box bị kéo giãn không đều (non-uniform
    // scale) từ 96px gốc lên 320px, khiến 2 đầu bo góc của SilverBox.png
    // bị méo dẹt thành hình "thanh dài". Giờ tách làm 3 mảnh — trái/phải
    // giữ nguyên tỉ lệ gốc (scale đều), chỉ phần giữa bị kéo giãn — nên
    // 2 đầu box không còn bị méo (kỹ thuật 3-slice / 9-slice).
    //----------------------------------

    constexpr float LABEL_BOX_WIDTH = 310.f;
    constexpr float LABEL_BOX_HEIGHT = BOX_HEIGHT;

    constexpr float LABEL_BOX_X = CONTENT_LEFT;

    // Tỉ lệ phần bo góc 2 đầu so với chiều rộng texture gốc (chỉnh nếu
    // 2 đầu box vẫn còn méo — tăng lên nếu góc bo to, giảm nếu góc bo nhỏ)
    constexpr float LABEL_BOX_CAP_FRACTION = 0.22f;

    constexpr float LABEL_BOX_CAP_TEX_WIDTH =
        BOX_TEX_WIDTH * LABEL_BOX_CAP_FRACTION;

    // Scale đều theo chiều cao — áp dụng cho 2 đầu bo góc để giữ đúng tỉ lệ
    constexpr float LABEL_BOX_UNIFORM_SCALE = LABEL_BOX_HEIGHT / BOX_TEX_HEIGHT;

    constexpr float LABEL_BOX_CAP_SCREEN_WIDTH =
        LABEL_BOX_CAP_TEX_WIDTH * LABEL_BOX_UNIFORM_SCALE;

    constexpr float LABEL_BOX_MID_TEX_WIDTH =
        BOX_TEX_WIDTH - 2.f * LABEL_BOX_CAP_TEX_WIDTH;

    constexpr float LABEL_BOX_MID_SCREEN_WIDTH =
        LABEL_BOX_WIDTH - 2.f * LABEL_BOX_CAP_SCREEN_WIDTH;

    constexpr float LABEL_BOX_MID_SCALE_X =
        LABEL_BOX_MID_SCREEN_WIDTH / LABEL_BOX_MID_TEX_WIDTH;

    // Chỉnh nếu chữ label bị lệch tâm so với box do texture có padding ẩn
    constexpr float LABEL_TEXT_Y_OFFSET = -30.f;

    // ===== ADDED: bù lệch ngang do font có padding không đều 2 bên =====
    constexpr float LABEL_TEXT_X_OFFSET = -30.f; // âm = dịch trái, dương = dịch phải

    //----------------------------------
    // Value Text
    //----------------------------------

    constexpr unsigned int VALUE_CHAR_SIZE = 40;

    //----------------------------------
    // Switch (1254 x 1254)
    //----------------------------------

    constexpr float SWITCH_TEX_SIZE = 1254.f;

    constexpr float SWITCH_SIZE = 130.f;
    constexpr float SWITCH_SCALE = SWITCH_SIZE / SWITCH_TEX_SIZE;

    //----------------------------------
    // Times Icon (1254 x 1254)
    //----------------------------------

    constexpr float TIMES_TEX_SIZE = 1254.f;

    constexpr float TIMES_SIZE = 40.f;
    constexpr float TIMES_SCALE = TIMES_SIZE / TIMES_TEX_SIZE;

    //----------------------------------
    // Back Button (BACK.png, 1736 x 906 — chữ "BACK" đã vẽ sẵn trong ảnh)
    //----------------------------------

    constexpr float BACK_TEX_WIDTH = 1736.f;
    constexpr float BACK_TEX_HEIGHT = 860.f;

    constexpr float BACK_BUTTON_WIDTH = 130.f;   // ===== CHANGED: nhỏ lại (200 -> 170) =====
    constexpr float BACK_BUTTON_HEIGHT =
        BACK_BUTTON_WIDTH * (BACK_TEX_HEIGHT / BACK_TEX_WIDTH);

    constexpr float BACK_BUTTON_SCALE =
        BACK_BUTTON_WIDTH / BACK_TEX_WIDTH;

    constexpr float BACK_BUTTON_X = CENTER_X - BACK_BUTTON_WIDTH / 2.f;
    constexpr float BACK_BUTTON_Y = 930.f;

    //----------------------------------
    // FPS Steps
    //----------------------------------

    constexpr int FPS_STEPS[] = { 30, 60, 120, 240 };
    constexpr int FPS_STEP_COUNT = 4;

    //----------------------------------
    // Helper: center a text on a point
    //----------------------------------

    void centerTextOnPoint(sf::Text& text, float centerX, float centerY)
    {
        sf::FloatRect bounds = text.getLocalBounds();

        text.setOrigin(
            bounds.left + bounds.width / 2.f,
            bounds.top + bounds.height / 2.f);

        text.setPosition(centerX, centerY);
    }
}

//==================================================
// Constructor
//==================================================

SettingMenu::SettingMenu()
{
    initializeObjects();
}

//==================================================
// Texture Setters
//==================================================

void SettingMenu::setPanelTexture(const sf::Texture& texture)
{
    panelSprite.setTexture(texture, true);
    panelSprite.setScale(PANEL_SCALE_X, PANEL_SCALE_Y);
}

// ===== ADDED: background giống màn Select Character =====
void SettingMenu::setBackgroundTexture(const sf::Texture& texture,
    float scaleX, float scaleY)
{
    backgroundSprite.setTexture(texture, true);
    backgroundSprite.setPosition(0.f, 0.f);
    backgroundSprite.setScale(scaleX, scaleY);
}

void SettingMenu::setValueBoxTexture(const sf::Texture& texture)
{
    musicBox.setTexture(texture, true);
    musicBox.setScale(BOX_SCALE_X, BOX_SCALE_Y);

    soundBox.setTexture(texture, true);
    soundBox.setScale(BOX_SCALE_X, BOX_SCALE_Y);

    fpsBox.setTexture(texture, true);
    fpsBox.setScale(BOX_SCALE_X, BOX_SCALE_Y);

    resolutionBox.setTexture(texture, true);
    resolutionBox.setScale(RES_BOX_SCALE_X, RES_BOX_SCALE_Y);

    // ===== ADDED: label box dùng chung texture SilverBox, chỉ trang trí =====
    // ===== CHANGED: dùng helper 3-slice để 2 đầu bo góc không bị méo =====
    setLabelBoxTexture(musicLabelBox, texture);
    setLabelBoxTexture(soundLabelBox, texture);
    setLabelBoxTexture(fullscreenLabelBox, texture);
    setLabelBoxTexture(fpsLabelBox, texture);
    setLabelBoxTexture(resolutionLabelBox, texture);
}

void SettingMenu::setPlusTexture(const sf::Texture& texture)
{
    musicPlusSprite.setTexture(texture, true);
    musicPlusSprite.setScale(ICON_SCALE, ICON_SCALE);

    soundPlusSprite.setTexture(texture, true);
    soundPlusSprite.setScale(ICON_SCALE, ICON_SCALE);

    fpsPlusSprite.setTexture(texture, true);
    fpsPlusSprite.setScale(ICON_SCALE, ICON_SCALE);

    resolutionPlusSprite.setTexture(texture, true);
    resolutionPlusSprite.setScale(ICON_SCALE, ICON_SCALE);
}

void SettingMenu::setMinusTexture(const sf::Texture& texture)
{
    musicMinusSprite.setTexture(texture, true);
    musicMinusSprite.setScale(ICON_SCALE, ICON_SCALE);

    soundMinusSprite.setTexture(texture, true);
    soundMinusSprite.setScale(ICON_SCALE, ICON_SCALE);

    fpsMinusSprite.setTexture(texture, true);
    fpsMinusSprite.setScale(ICON_SCALE, ICON_SCALE);

    resolutionMinusSprite.setTexture(texture, true);
    resolutionMinusSprite.setScale(ICON_SCALE, ICON_SCALE);
}

void SettingMenu::setSwitchTextures(const sf::Texture& onTexture,
    const sf::Texture& offTexture)
{
    switchOnTexture = &onTexture;
    switchOffTexture = &offTexture;

    fullscreenSprite.setTexture(
        fullscreen ? *switchOnTexture : *switchOffTexture, true);

    fullscreenSprite.setScale(SWITCH_SCALE, SWITCH_SCALE);
}

void SettingMenu::setTimesTexture(const sf::Texture& texture)
{
    resolutionTimesSprite.setTexture(texture, true);
    resolutionTimesSprite.setScale(TIMES_SCALE, TIMES_SCALE);
}

// ===== ADDED =====
void SettingMenu::setBackButtonTexture(const sf::Texture& texture)
{
    backButton.setTexture(texture);
    backButton.setScale(BACK_BUTTON_SCALE, BACK_BUTTON_SCALE);
}

// ===== ADDED: ảnh chữ "SETTING" thay cho sf::Text =====
void SettingMenu::setTitleTexture(const sf::Texture& texture)
{
    title.setTexture(texture, true);
    title.setPosition(TITLE_X, TITLE_TOP_Y);
    title.setScale(TITLE_SCALE, TITLE_SCALE);
}

//==================================================
// Font
//==================================================

void SettingMenu::setFont(const sf::Font& newFont)
{
    font = newFont;

    musicLabel.setFont(font);
    soundLabel.setFont(font);
    fullscreenLabel.setFont(font);
    fpsLabel.setFont(font);
    resolutionLabel.setFont(font);

    musicValueText.setFont(font);
    soundValueText.setFont(font);
    fpsValueText.setFont(font);
    resolutionWidthText.setFont(font);
    resolutionHeightText.setFont(font);

    backButton.setFont(font);

    updateTexts();
}

//==================================================
// Helper Functions
//==================================================

// ===== ADDED: cấu hình texture 3-slice cho label box (không méo) =====
void SettingMenu::setLabelBoxTexture(LabelBoxSprite& box,
    const sf::Texture& texture)
{
    box.left.setTexture(texture, true);
    box.mid.setTexture(texture, true);
    box.right.setTexture(texture, true);

    box.left.setTextureRect(sf::IntRect(
        0, 0,
        (int)LABEL_BOX_CAP_TEX_WIDTH, (int)BOX_TEX_HEIGHT));

    box.mid.setTextureRect(sf::IntRect(
        (int)LABEL_BOX_CAP_TEX_WIDTH, 0,
        (int)LABEL_BOX_MID_TEX_WIDTH, (int)BOX_TEX_HEIGHT));

    box.right.setTextureRect(sf::IntRect(
        (int)(BOX_TEX_WIDTH - LABEL_BOX_CAP_TEX_WIDTH), 0,
        (int)LABEL_BOX_CAP_TEX_WIDTH, (int)BOX_TEX_HEIGHT));

    // 2 đầu bo góc: scale đều (không méo)
    box.left.setScale(LABEL_BOX_UNIFORM_SCALE, LABEL_BOX_UNIFORM_SCALE);
    box.right.setScale(LABEL_BOX_UNIFORM_SCALE, LABEL_BOX_UNIFORM_SCALE);

    // Phần giữa: chỉ phần này bị kéo giãn theo chiều ngang
    box.mid.setScale(LABEL_BOX_MID_SCALE_X, LABEL_BOX_UNIFORM_SCALE);
}

// ===== ADDED: đặt vị trí 3 mảnh liền kề nhau =====
void SettingMenu::setLabelBoxPosition(LabelBoxSprite& box,
    float x, float y)
{
    box.left.setPosition(x, y);
    box.mid.setPosition(x + LABEL_BOX_CAP_SCREEN_WIDTH, y);
    box.right.setPosition(
        x + LABEL_BOX_CAP_SCREEN_WIDTH + LABEL_BOX_MID_SCREEN_WIDTH, y);
}

// ===== ADDED: vẽ cả 3 mảnh =====
void SettingMenu::drawLabelBox(sf::RenderWindow& window,
    const LabelBoxSprite& box)
{
    window.draw(box.left);
    window.draw(box.mid);
    window.draw(box.right);
}

void SettingMenu::initializeObjects()
{
    //----------------------------------
    // Panel
    //----------------------------------

    panelSprite.setPosition(PANEL_X, PANEL_Y);
    panelSprite.setScale(PANEL_SCALE_X, PANEL_SCALE_Y);

    //----------------------------------
    // Title
    //----------------------------------

    // Title giờ là sf::Sprite (ảnh SETTING.png đã vẽ sẵn chữ),
    // vị trí/scale được thiết lập trong setTitleTexture().

    //----------------------------------
    // Labels
    //----------------------------------

    musicLabel.setString("MUSIC");
    musicLabel.setCharacterSize(LABEL_CHAR_SIZE);

    soundLabel.setString("SOUND");
    soundLabel.setCharacterSize(LABEL_CHAR_SIZE);

    fullscreenLabel.setString("F.S");
    fullscreenLabel.setCharacterSize(LABEL_CHAR_SIZE);

    fpsLabel.setString("FPS");
    fpsLabel.setCharacterSize(LABEL_CHAR_SIZE);

    resolutionLabel.setString("RES");
    resolutionLabel.setCharacterSize(LABEL_CHAR_SIZE);

    //----------------------------------
    // Label Boxes (SilverBox trang trí đứng sau label — KHÔNG tương tác,
    // không được thêm vào isMouseOver nên click không ảnh hưởng)
    //----------------------------------

    //----------------------------------
    // Label Boxes (SilverBox trang trí đứng sau label — KHÔNG tương tác,
    // không được thêm vào isMouseOver nên click không ảnh hưởng)
    // ===== CHANGED: dùng helper 3-slice, 2 đầu không còn bị méo =====
    //----------------------------------

    setLabelBoxPosition(musicLabelBox, LABEL_BOX_X, MUSIC_ROW_Y);
    setLabelBoxPosition(soundLabelBox, LABEL_BOX_X, SOUND_ROW_Y);
    setLabelBoxPosition(fullscreenLabelBox, LABEL_BOX_X, FULLSCREEN_ROW_Y);
    setLabelBoxPosition(fpsLabelBox, LABEL_BOX_X, FPS_ROW_Y);
    setLabelBoxPosition(resolutionLabelBox, LABEL_BOX_X, RESOLUTION_ROW_Y);

    // Label text được căn giữa bên trong label box tương ứng.
    // Vì nội dung label không đổi sau khi khởi tạo, không cần
    // tính lại trong updateTexts().
    centerTextOnPoint(musicLabel,
        LABEL_BOX_X + LABEL_BOX_WIDTH / 2.f + LABEL_TEXT_X_OFFSET,
        MUSIC_ROW_Y + LABEL_BOX_HEIGHT / 2.f + LABEL_TEXT_Y_OFFSET);

    centerTextOnPoint(soundLabel,
        LABEL_BOX_X + LABEL_BOX_WIDTH / 2.f + LABEL_TEXT_X_OFFSET,
        SOUND_ROW_Y + LABEL_BOX_HEIGHT / 2.f + LABEL_TEXT_Y_OFFSET);

    centerTextOnPoint(fullscreenLabel,
        LABEL_BOX_X + LABEL_BOX_WIDTH / 2.f + LABEL_TEXT_X_OFFSET,
        FULLSCREEN_ROW_Y + LABEL_BOX_HEIGHT / 2.f + LABEL_TEXT_Y_OFFSET);

    centerTextOnPoint(fpsLabel,
        LABEL_BOX_X + LABEL_BOX_WIDTH / 2.f + LABEL_TEXT_X_OFFSET,
        FPS_ROW_Y + LABEL_BOX_HEIGHT / 2.f + LABEL_TEXT_Y_OFFSET);

    centerTextOnPoint(resolutionLabel,
        LABEL_BOX_X + LABEL_BOX_WIDTH / 2.f + LABEL_TEXT_X_OFFSET,
        RESOLUTION_ROW_Y + LABEL_BOX_HEIGHT / 2.f + LABEL_TEXT_Y_OFFSET);

    //----------------------------------
    // Value Boxes
    //----------------------------------

    musicBox.setPosition(BOX_X, MUSIC_ROW_Y);
    musicBox.setScale(BOX_SCALE_X, BOX_SCALE_Y);

    soundBox.setPosition(BOX_X, SOUND_ROW_Y);
    soundBox.setScale(BOX_SCALE_X, BOX_SCALE_Y);

    fpsBox.setPosition(BOX_X, FPS_ROW_Y);
    fpsBox.setScale(BOX_SCALE_X, BOX_SCALE_Y);

    resolutionBox.setPosition(RES_BOX_X, RESOLUTION_ROW_Y);
    resolutionBox.setScale(RES_BOX_SCALE_X, RES_BOX_SCALE_Y);

    //----------------------------------
    // Plus / Minus Icons
    //----------------------------------

    musicMinusSprite.setPosition(
        BOX_X - ICON_GAP - ICON_SIZE, MUSIC_ROW_Y + ICON_Y_OFFSET);
    musicMinusSprite.setScale(ICON_SCALE, ICON_SCALE);

    musicPlusSprite.setPosition(
        BOX_X + BOX_WIDTH + ICON_GAP, MUSIC_ROW_Y + ICON_Y_OFFSET);
    musicPlusSprite.setScale(ICON_SCALE, ICON_SCALE);

    soundMinusSprite.setPosition(
        BOX_X - ICON_GAP - ICON_SIZE, SOUND_ROW_Y + ICON_Y_OFFSET);
    soundMinusSprite.setScale(ICON_SCALE, ICON_SCALE);

    soundPlusSprite.setPosition(
        BOX_X + BOX_WIDTH + ICON_GAP, SOUND_ROW_Y + ICON_Y_OFFSET);
    soundPlusSprite.setScale(ICON_SCALE, ICON_SCALE);

    fpsMinusSprite.setPosition(
        BOX_X - ICON_GAP - ICON_SIZE, FPS_ROW_Y + ICON_Y_OFFSET);
    fpsMinusSprite.setScale(ICON_SCALE, ICON_SCALE);

    fpsPlusSprite.setPosition(
        BOX_X + BOX_WIDTH + ICON_GAP, FPS_ROW_Y + ICON_Y_OFFSET);
    fpsPlusSprite.setScale(ICON_SCALE, ICON_SCALE);

    resolutionMinusSprite.setPosition(
        RES_BOX_X - ICON_GAP - ICON_SIZE, RESOLUTION_ROW_Y + ICON_Y_OFFSET);
    resolutionMinusSprite.setScale(ICON_SCALE, ICON_SCALE);

    resolutionPlusSprite.setPosition(
        RES_BOX_X + RES_BOX_WIDTH + ICON_GAP, RESOLUTION_ROW_Y + ICON_Y_OFFSET);
    resolutionPlusSprite.setScale(ICON_SCALE, ICON_SCALE);

    //----------------------------------
    // Switch
    //----------------------------------

    fullscreenSprite.setPosition(
        BOX_X, FULLSCREEN_ROW_Y - (SWITCH_SIZE - BOX_HEIGHT) / 2.f);
    fullscreenSprite.setScale(SWITCH_SCALE, SWITCH_SCALE);

    //----------------------------------
    // Times Icon
    //----------------------------------

    resolutionTimesSprite.setPosition(
        RES_BOX_X + RES_BOX_WIDTH / 2.f - TIMES_SIZE / 2.f,
        RESOLUTION_ROW_Y + RES_BOX_HEIGHT / 2.f - TIMES_SIZE / 2.f);
    resolutionTimesSprite.setScale(TIMES_SCALE, TIMES_SCALE);

    //----------------------------------
    // Values (initial strings, positions fixed in updateTexts)
    //----------------------------------

    musicValueText.setString("100");
    musicValueText.setCharacterSize(VALUE_CHAR_SIZE);

    soundValueText.setString("100");
    soundValueText.setCharacterSize(VALUE_CHAR_SIZE);

    fpsValueText.setString("60");
    fpsValueText.setCharacterSize(VALUE_CHAR_SIZE);

    resolutionWidthText.setString("1920");
    resolutionWidthText.setCharacterSize(VALUE_CHAR_SIZE);

    resolutionHeightText.setString("1080");
    resolutionHeightText.setCharacterSize(VALUE_CHAR_SIZE);

    //----------------------------------
    // Back Button
    //----------------------------------

    backButton.setPosition(BACK_BUTTON_X, BACK_BUTTON_Y);
    backButton.setScale(BACK_BUTTON_SCALE, BACK_BUTTON_SCALE);

    // BACK.png đã vẽ sẵn chữ "BACK" trong ảnh, nên không đặt thêm
    // sf::Text đè lên để tránh bị chồng chữ.
    backButton.setText("");
}

void SettingMenu::updateTexts()
{
    //----------------------------------
    // Music
    //----------------------------------

    musicValueText.setString(std::to_string(musicVolume));
    centerTextOnPoint(musicValueText,
        BOX_X + BOX_WIDTH / 2.f,
        MUSIC_ROW_Y + BOX_HEIGHT / 2.f);

    //----------------------------------
    // Sound
    //----------------------------------

    soundValueText.setString(std::to_string(soundVolume));
    centerTextOnPoint(soundValueText,
        BOX_X + BOX_WIDTH / 2.f,
        SOUND_ROW_Y + BOX_HEIGHT / 2.f);

    //----------------------------------
    // FPS
    //----------------------------------

    fpsValueText.setString(std::to_string(fpsLimit));
    centerTextOnPoint(fpsValueText,
        BOX_X + BOX_WIDTH / 2.f,
        FPS_ROW_Y + BOX_HEIGHT / 2.f);

    //----------------------------------
    // Resolution
    //----------------------------------

    resolutionWidthText.setString(
        std::to_string(resolutions[resolutionIndex].x));
    resolutionHeightText.setString(
        std::to_string(resolutions[resolutionIndex].y));

    float resCenterY = RESOLUTION_ROW_Y + RES_BOX_HEIGHT / 2.f;

    centerTextOnPoint(resolutionWidthText,
        RES_BOX_X + RES_BOX_WIDTH * 0.28f, resCenterY);
    centerTextOnPoint(resolutionHeightText,
        RES_BOX_X + RES_BOX_WIDTH * 0.72f, resCenterY);

    //----------------------------------
    // Fullscreen Switch
    //----------------------------------

    if (fullscreen)
    {
        if (switchOnTexture != nullptr)
            fullscreenSprite.setTexture(*switchOnTexture, true);
    }
    else
    {
        if (switchOffTexture != nullptr)
            fullscreenSprite.setTexture(*switchOffTexture, true);
    }

    fullscreenSprite.setScale(SWITCH_SCALE, SWITCH_SCALE);
}

bool SettingMenu::isMouseOver(const sf::Sprite& sprite,
    const sf::RenderWindow& window) const
{
    sf::Vector2f mousePos =
        window.mapPixelToCoords(sf::Mouse::getPosition(window));

    return sprite.getGlobalBounds().contains(mousePos);
}

//==================================================
// Menu
//==================================================

void SettingMenu::processEvent(const sf::Event& event,
    const sf::RenderWindow& window)
{
    backButton.processEvent(event, window);

    // ===== ADDED: ESC = Back, giống các menu khác (PauseMenu, ...) =====
    if (event.type == sf::Event::KeyPressed &&
        event.key.code == sf::Keyboard::Escape)
    {
        backButton.press();
        if (audio) audio->playSound("select");
        result = SettingMenuResult::Back;
        return;
    }

    if (event.type != sf::Event::MouseButtonReleased)
        return;

    if (event.mouseButton.button != sf::Mouse::Left)
        return;

    //-----------------------------
    // Music
    //-----------------------------

    if (isMouseOver(musicPlusSprite, window))
    {
        setMusicVolume(musicVolume + 5);
    }
    else if (isMouseOver(musicMinusSprite, window))
    {
        setMusicVolume(musicVolume - 5);
    }

    //-----------------------------
    // Sound
    //-----------------------------

    else if (isMouseOver(soundPlusSprite, window))
    {
        setSoundVolume(soundVolume + 5);
    }
    else if (isMouseOver(soundMinusSprite, window))
    {
        setSoundVolume(soundVolume - 5);
    }

    //-----------------------------
    // Fullscreen
    //-----------------------------

    else if (isMouseOver(fullscreenSprite, window))
    {
        setFullscreen(!fullscreen);
    }

    //-----------------------------
    // FPS
    //-----------------------------

    else if (isMouseOver(fpsPlusSprite, window))
    {
        int currentIndex = 0;

        for (int i = 0; i < FPS_STEP_COUNT; i++)
        {
            if (FPS_STEPS[i] == fpsLimit)
            {
                currentIndex = i;
                break;
            }
        }

        if (currentIndex < FPS_STEP_COUNT - 1)
            currentIndex++;

        setFPSLimit(FPS_STEPS[currentIndex]);
    }
    else if (isMouseOver(fpsMinusSprite, window))
    {
        int currentIndex = 0;

        for (int i = 0; i < FPS_STEP_COUNT; i++)
        {
            if (FPS_STEPS[i] == fpsLimit)
            {
                currentIndex = i;
                break;
            }
        }

        if (currentIndex > 0)
            currentIndex--;

        setFPSLimit(FPS_STEPS[currentIndex]);
    }

    //-----------------------------
    // Resolution
    //-----------------------------

    else if (isMouseOver(resolutionPlusSprite, window))
    {
        setResolution(resolutionIndex + 1);
    }
    else if (isMouseOver(resolutionMinusSprite, window))
    {
        setResolution(resolutionIndex - 1);
    }

    //-----------------------------
    // Back
    //-----------------------------

    else
    {
        sf::Vector2f mousePos =
            window.mapPixelToCoords(sf::Mouse::getPosition(window));

        if (backButton.contains(mousePos))
        {
            backButton.press();
            if (audio) audio->playSound("select");
            result = SettingMenuResult::Back;
        }
    }
}

void SettingMenu::update(float)
{
    backButton.update();
}

void SettingMenu::draw(sf::RenderWindow& window) const
{
    //-----------------------------
    // Background (giống màn Select Character)
    //-----------------------------

    window.draw(backgroundSprite);

    //-----------------------------
    // Panel
    //-----------------------------

    window.draw(panelSprite);

    //-----------------------------
    // Title
    //-----------------------------

    window.draw(title);

    //-----------------------------
    // Label Boxes (trang trí, vẽ trước label text)
    //-----------------------------

    drawLabelBox(window, musicLabelBox);
    drawLabelBox(window, soundLabelBox);
    drawLabelBox(window, fullscreenLabelBox);
    drawLabelBox(window, fpsLabelBox);
    drawLabelBox(window, resolutionLabelBox);

    //-----------------------------
    // Labels
    //-----------------------------

    window.draw(musicLabel);
    window.draw(soundLabel);
    window.draw(fullscreenLabel);
    window.draw(fpsLabel);
    window.draw(resolutionLabel);

    //-----------------------------
    // Boxes
    //-----------------------------

    window.draw(musicBox);
    window.draw(soundBox);
    window.draw(fpsBox);
    window.draw(resolutionBox);

    //-----------------------------
    // Plus
    //-----------------------------

    window.draw(musicPlusSprite);
    window.draw(soundPlusSprite);
    window.draw(fpsPlusSprite);
    window.draw(resolutionPlusSprite);

    //-----------------------------
    // Minus
    //-----------------------------

    window.draw(musicMinusSprite);
    window.draw(soundMinusSprite);
    window.draw(fpsMinusSprite);
    window.draw(resolutionMinusSprite);

    //-----------------------------
    // Switch
    //-----------------------------

    window.draw(fullscreenSprite);

    //-----------------------------
    // Times
    //-----------------------------

    window.draw(resolutionTimesSprite);

    //-----------------------------
    // Values
    //-----------------------------

    window.draw(musicValueText);
    window.draw(soundValueText);
    window.draw(fpsValueText);
    window.draw(resolutionWidthText);
    window.draw(resolutionHeightText);

    //-----------------------------
    // Back Button
    //-----------------------------

    backButton.draw(window);
}

//==================================================
// Result
//==================================================

SettingMenuResult SettingMenu::getResult() const
{
    return result;
}

void SettingMenu::clearResult()
{
    result = SettingMenuResult::None;
}

//==================================================
// Getters
//==================================================

int SettingMenu::getMusicVolume() const
{
    return musicVolume;
}

int SettingMenu::getSoundVolume() const
{
    return soundVolume;
}

bool SettingMenu::isFullscreen() const
{
    return fullscreen;
}

int SettingMenu::getFPSLimit() const
{
    return fpsLimit;
}

sf::Vector2u SettingMenu::getResolution() const
{
    return resolutions[resolutionIndex];
}

//==================================================
// Setters
//==================================================

void SettingMenu::setMusicVolume(int volume)
{
    if (volume < 0)
        volume = 0;

    if (volume > 100)
        volume = 100;

    musicVolume = volume;

    updateTexts();

    if (onMusicVolumeChanged)
        onMusicVolumeChanged(musicVolume);
}

void SettingMenu::setOnMusicVolumeChanged(std::function<void(int)> callback)
{
    onMusicVolumeChanged = std::move(callback);
}

void SettingMenu::setSoundVolume(int volume)
{
    if (volume < 0)
        volume = 0;

    if (volume > 100)
        volume = 100;

    soundVolume = volume;

    updateTexts();

    // ===== ADDED: day gia tri xuong AudioManager dung chung - day la
    // buoc con thieu truoc day khien keo Sound tren UI doi so nhung
    // khong anh huong gi den SFX thuc te (kho hazard Nightmare, am bam
    // nut "select",...) vi tat ca deu lay volume tu AudioManager::soundVolume,
    // khong phai tu bien soundVolume rieng cua SettingMenu =====
    if (audio)
        audio->setSoundVolume(soundVolume);
}

void SettingMenu::setFullscreen(bool enable)
{
    fullscreen = enable;

    updateTexts();
}

void SettingMenu::setFPSLimit(int fps)
{
    if (fps < FPS_STEPS[0])
        fps = FPS_STEPS[0];

    if (fps > FPS_STEPS[FPS_STEP_COUNT - 1])
        fps = FPS_STEPS[FPS_STEP_COUNT - 1];

    fpsLimit = fps;

    updateTexts();
}

void SettingMenu::setResolution(int index)
{
    int count = RESOLUTION_COUNT;

    index = index % count;

    if (index < 0)
        index += count;

    resolutionIndex = index;

    updateTexts();
}