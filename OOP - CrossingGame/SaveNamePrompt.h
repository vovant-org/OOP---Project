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

    // ===== ADDED (Load by name - phim 'T' o Main Menu): dong bao loi
    // nho, hien NGAY DUOI o nhap khi tim khong thay save nao trung ten
    // nguoi choi vua go. Rong ("") = khong hien gi ca =====
    sf::Text errorText;

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

    // ===== ADDED: toa do Y cua dong loi (tinh san trong layout()), de
    // setErrorMessage() co the can giua lai text moi ma khong can goi
    // lai toan bo layout() =====
    float errorY = 0.f;

    std::string nameInput;

    // ===== ADDED (fix loi go phim mo dialog bi tinh la nhap chu dau
    // tien): xem giai thich chi tiet o khai bao ham open() ben duoi -
    // CHI bat true khi dialog duoc mo bang phim tat (L/T), KHONG bat khi
    // mo bang chuot =====
    bool ignoreNextTextEvent = false;

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

    // ===== ADDED (dung chung cho nhieu ngu canh): class nay ban dau
    // chi phuc vu "nhap ten LUC SAVE" (phim 'L' / nut "SAVE GAME"), gio
    // dung lai cho CA "nhap ten LUC LOAD" (phim 'T' o Main Menu - xem
    // main.cpp). setTitle/setHint/setButtonLabels cho phep tuy chinh
    // chu hien thi cho tung ngu canh, KHONG can tao them 1 class rieng
    // gan nhu giong het =====
    void setTitle(const std::string& title);
    void setHint(const std::string& hint);
    void setButtonLabels(const std::string& confirmLabel, const std::string& cancelLabel);

    // ===== ADDED (Load by name): hien 1 dong loi mau do NGAY DUOI o
    // nhap (VD: khong tim thay save nao trung ten) - goi voi chuoi rong
    // de an di. Tu dong bi xoa moi khi nguoi choi go/xoa ky tu, hoac moi
    // lan open() =====
    void setErrorMessage(const std::string& message);

    // ===== ADDED: goi ngay TRUOC khi doi AppState sang man hinh nay -
    // reset ve input rong + xoa result cu, tranh giu lai ten cua lan
    // save truoc do hien lai. "openedByKeyboardShortcut" = true CHI KHI
    // dialog nay duoc mo bang 1 PHIM TAT (VD 'L' luc Playing, 'T' o Main
    // Menu) - luc do phim vua bam se "rot" them 1 su kien TextEntered
    // ngay sau, can bo qua 1 lan de khong bi tinh nham la nguoi choi go
    // vao o nhap. Neu mo bang CHUOT (VD nut "SAVE GAME" trong Pause
    // Menu) thi PHAI truyen false (mac dinh), vi khong co su kien
    // TextEntered "rot" nao ca - bat nham se nuot mat ky tu THAT dau
    // tien nguoi choi go sau nay =====
    void open(bool openedByKeyboardShortcut = false);

    const std::string& getNameInput() const;

    void processEvent(const sf::Event& event,
        const sf::RenderWindow& window) override;

    void update(float dt) override;

    void draw(sf::RenderWindow& window) const override;

    SaveNamePromptResult getResult() const;
    void clearResult();
};