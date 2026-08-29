// SaveNamePrompt.h
#pragma once

#include <SFML/Graphics.hpp>
#include <string>

#include "Menu.h"
#include "Button.h"

class AudioManager;

//==================================================
// Result
//==================================================

enum class SaveNamePromptResult
{
    None = 0,
    Confirm,   // nguoi choi bam Enter / nut CONFIRM -> tien hanh luu voi ten da go
    Cancel     // nguoi choi bam Esc / nut CANCEL -> huy, KHONG luu game
};

//==================================================
// SaveNamePrompt
//
// ===== ADDED (nhap ten save): overlay nho hien ra KHI nguoi choi bam
// phim 'L' luc dang choi, HOAC bam nut "SAVE GAME" trong Pause Menu -
// cho phep go 1 cai ten (toi da MAX_NAME_LEN ky tu) truoc khi thuc su
// ghi file .sav. Dung CHUNG 1 class cho CA 2 truong hop do main.cpp tu
// nho AppState nao da mo prompt (Playing hay Pause) de quay ve dung
// cho sau khi Confirm/Cancel - xem main.cpp, bien
// "saveNamePromptReturnState" =====
//==================================================

class SaveNamePrompt : public Menu
{
private:

    // Gioi han do dai ten save - vua du hien tren 1 dong trong Continue
    // Menu (cot info tuong doi hep), khong lam vo layout cua ContinueMenu
    static constexpr std::size_t MAX_NAME_LEN = 24;

    sf::RectangleShape overlay;    // lam mo toan man hinh, giong PauseMenu
    sf::RectangleShape panel;      // khung nho o giua chua title + input box
    sf::RectangleShape inputBox;

    sf::Text titleText;
    sf::Text hintText;
    sf::Text inputText;

    Button confirmButton;
    Button cancelButton;

    bool hasFont = false;

    float canvasW = 1280.f;
    float canvasH = 720.f;

    // ===== ADDED: kich thuoc THAT SU (sau khi scale) cua confirmButton/
    // cancelButton, luu lai luc setButtonTexture() de layout() co the
    // dat vi tri 2 nut can doi, khong bi chong len nhau =====
    float btnRenderW = 0.f;
    float btnRenderH = 0.f;

    std::string nameInput;

    SaveNamePromptResult result = SaveNamePromptResult::None;

    AudioManager* audio = nullptr;

    void layout();
    void updateInputDisplay();
    void confirm();
    void cancel();

public:

    SaveNamePrompt();

    void setAudioManager(AudioManager* manager);
    void setWindowSize(float w, float h);
    void setFont(const sf::Font& font);
    void setButtonTexture(const sf::Texture& tex, float scaleX, float scaleY);

    // ===== ADDED: goi ngay TRUOC khi doi AppState sang man hinh nay -
    // reset ve input rong + xoa result cu, tranh giu lai ten cua lan
    // save truoc do hien lai =====
    void open();

    const std::string& getNameInput() const;

    void processEvent(const sf::Event& event,
        const sf::RenderWindow& window) override;

    void update(float dt) override;

    void draw(sf::RenderWindow& window) const override;

    SaveNamePromptResult getResult() const;
    void clearResult();
};