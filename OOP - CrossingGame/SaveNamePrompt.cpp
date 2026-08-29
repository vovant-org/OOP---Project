// SaveNamePrompt.cpp
#include "SaveNamePrompt.h"
#include "AudioManager.h"

namespace
{
    void centerText(sf::Text& t, float cx, float y)
    {
        sf::FloatRect b = t.getLocalBounds();
        t.setOrigin(b.left + b.width / 2.f, b.top);
        t.setPosition(cx, y);
    }
}

//==================================================
// Constructor
//==================================================

SaveNamePrompt::SaveNamePrompt()
{
}

//==================================================
// Audio / Window / Font / Button
//==================================================

void SaveNamePrompt::setAudioManager(AudioManager* manager)
{
    audio = manager;
}

void SaveNamePrompt::setWindowSize(float w, float h)
{
    canvasW = w;
    canvasH = h;
    layout();
}

void SaveNamePrompt::setFont(const sf::Font& font)
{
    titleText.setFont(font);
    titleText.setString("SAVE GAME");
    titleText.setCharacterSize(32);
    titleText.setStyle(sf::Text::Bold);
    titleText.setFillColor(sf::Color(255, 220, 80));

    hintText.setFont(font);
    hintText.setString("Nhap ten cho file save (Enter: Luu, Esc: Huy)");
    hintText.setCharacterSize(16);
    hintText.setFillColor(sf::Color(220, 220, 220));

    inputText.setFont(font);
    inputText.setCharacterSize(22);
    inputText.setFillColor(sf::Color::White);

    confirmButton.setFont(font);
    confirmButton.setText("CONFIRM");
    confirmButton.setCharacterSize(20);

    cancelButton.setFont(font);
    cancelButton.setText("CANCEL");
    cancelButton.setCharacterSize(20);

    hasFont = true;

    updateInputDisplay();
    layout();
}

void SaveNamePrompt::setButtonTexture(const sf::Texture& tex, float scaleX, float scaleY)
{
    confirmButton.setTexture(tex);
    confirmButton.setScale(scaleX, scaleY);

    cancelButton.setTexture(tex);
    cancelButton.setScale(scaleX, scaleY);

    btnRenderW = tex.getSize().x * scaleX;
    btnRenderH = tex.getSize().y * scaleY;

    layout();
}

//==================================================
// Open / Input
//==================================================

void SaveNamePrompt::open()
{
    // ===== ADDED: reset ve trang thai rong moi lan mo, tranh giu lai
    // ten cua lan Quick Save truoc do hien lai trong o nhap =====
    nameInput.clear();
    result = SaveNamePromptResult::None;
    updateInputDisplay();
}

const std::string& SaveNamePrompt::getNameInput() const
{
    return nameInput;
}

void SaveNamePrompt::updateInputDisplay()
{
    if (nameInput.empty())
    {
        // ===== ADDED: placeholder mau xam khi chua go gi, giup nguoi
        // choi biet o nay dung de nhap ten (khong bat buoc) =====
        inputText.setString("Nhap ten (khong bat buoc)...");
        inputText.setFillColor(sf::Color(140, 140, 140));
    }
    else
    {
        inputText.setString(nameInput);
        inputText.setFillColor(sf::Color::White);
    }

    if (hasFont)
    {
        sf::FloatRect tb = inputText.getLocalBounds();
        inputText.setOrigin(tb.left, tb.top + tb.height / 2.f);
    }
}

//==================================================
// Layout
//==================================================

void SaveNamePrompt::layout()
{
    overlay.setSize({ canvasW, canvasH });
    overlay.setFillColor(sf::Color(0, 0, 0, 160));
    overlay.setPosition(0.f, 0.f);

    const float panelW = 560.f, panelH = 260.f;
    const float panelX = canvasW / 2.f - panelW / 2.f;
    const float panelY = canvasH / 2.f - panelH / 2.f;

    panel.setSize({ panelW, panelH });
    panel.setPosition(panelX, panelY);
    panel.setFillColor(sf::Color(25, 25, 35, 235));
    panel.setOutlineThickness(3.f);
    panel.setOutlineColor(sf::Color(255, 220, 80));

    if (hasFont)
    {
        centerText(titleText, canvasW / 2.f, panelY + 20.f);
        centerText(hintText, canvasW / 2.f, panelY + 66.f);
    }

    const float boxW = 460.f, boxH = 52.f;
    const float boxX = canvasW / 2.f - boxW / 2.f;
    const float boxY = panelY + 100.f;

    inputBox.setSize({ boxW, boxH });
    inputBox.setPosition(boxX, boxY);
    inputBox.setFillColor(sf::Color(10, 10, 15, 255));
    inputBox.setOutlineThickness(2.f);
    inputBox.setOutlineColor(sf::Color(120, 120, 140));

    if (hasFont)
        inputText.setPosition(boxX + 14.f, boxY + boxH / 2.f);

    // ===== ADDED: 2 nut CONFIRM/CANCEL nam ngang, can giua duoi o nhap,
    // dung btnRenderW/H (kich thuoc THAT SU sau scale) de tinh vi tri
    // chinh xac thay vi doan chung =====
    const float btnY = boxY + boxH + 26.f;
    const float gap = 24.f;
    const float totalW = btnRenderW * 2.f + gap;
    const float startX = canvasW / 2.f - totalW / 2.f;

    confirmButton.setPosition(startX, btnY);
    cancelButton.setPosition(startX + btnRenderW + gap, btnY);
}

//==================================================
// Confirm / Cancel
//==================================================

void SaveNamePrompt::confirm()
{
    if (audio)
        audio->playSound("select");

    result = SaveNamePromptResult::Confirm;
}

void SaveNamePrompt::cancel()
{
    if (audio)
        audio->playSound("select");

    result = SaveNamePromptResult::Cancel;
}

//==================================================
// Result
//==================================================

SaveNamePromptResult SaveNamePrompt::getResult() const
{
    return result;
}

void SaveNamePrompt::clearResult()
{
    result = SaveNamePromptResult::None;
}

//==================================================
// Menu
//==================================================

void SaveNamePrompt::processEvent(const sf::Event& event,
    const sf::RenderWindow& window)
{
    //-----------------------------
    // Go ten
    //-----------------------------
    if (event.type == sf::Event::TextEntered)
    {
        unsigned int code = event.text.unicode;

        if (code == 8)   // Backspace
        {
            if (!nameInput.empty())
                nameInput.pop_back();
            updateInputDisplay();
        }
        // ===== ADDED: chi nhan ky tu ASCII in duoc (32-126). Font pixel
        // dang dung (PixelOperator) khong co du glyph tieng Viet co dau,
        // va file .sav doc/ghi tung dong bang std::getline() nen gioi han
        // ASCII de tranh cac van de encoding UTF-8 khi doc lai sau nay =====
        else if (code >= 32 && code < 127 && nameInput.size() < MAX_NAME_LEN)
        {
            nameInput.push_back(static_cast<char>(code));
            updateInputDisplay();
        }
    }

    //-----------------------------
    // Phim tat
    //-----------------------------
    if (event.type == sf::Event::KeyPressed)
    {
        switch (event.key.code)
        {
        case sf::Keyboard::Enter:
            confirm();
            break;

        case sf::Keyboard::Escape:
            cancel();
            break;

        default:
            break;
        }
    }

    //-----------------------------
    // Chuot
    //-----------------------------
    confirmButton.processEvent(event, window);
    cancelButton.processEvent(event, window);

    if (event.type == sf::Event::MouseButtonReleased &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mousePos = window.mapPixelToCoords(
            { event.mouseButton.x, event.mouseButton.y });

        if (confirmButton.contains(mousePos))
        {
            confirmButton.press();
            confirm();
        }
        else if (cancelButton.contains(mousePos))
        {
            cancelButton.press();
            cancel();
        }
    }
}

void SaveNamePrompt::update(float dt)
{
    (void)dt;

    confirmButton.update();
    cancelButton.update();
}

void SaveNamePrompt::draw(sf::RenderWindow& window) const
{
    window.draw(overlay);
    window.draw(panel);

    if (hasFont)
    {
        window.draw(titleText);
        window.draw(hintText);
    }

    window.draw(inputBox);

    if (hasFont)
        window.draw(inputText);

    confirmButton.draw(window);
    cancelButton.draw(window);
}