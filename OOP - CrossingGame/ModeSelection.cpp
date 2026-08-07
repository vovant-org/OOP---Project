// ModeSelection.cpp
#include "ModeSelection.h"
#include "AudioManager.h"
#include <iostream>
#include <stdexcept>

//==================================================
// Constructor
//==================================================

ModeSelection::ModeSelection()
{
}

//==================================================
// Setup
//==================================================

void ModeSelection::setAudioManager(AudioManager* manager)
{
    audio = manager;
}

void ModeSelection::setWindowSize(float w, float h)
{
    W = w;
    H = h;
}

void ModeSelection::setBackgroundTexture(const sf::Texture& tex,
    float sx, float sy)
{
    background.setTexture(tex);
    background.setScale(sx, sy);
}

bool ModeSelection::loadFont(const std::string& path)
{
    if (!font.loadFromFile(path))
    {
        std::cout << "[ModeSelection] Cannot load font: " << path << "\n";
        return false;
    }
    titleText.setFont(font);
    hintText.setFont(font);

    for (auto& t : nameTexts)
        t.setFont(font);

    return true;
}

bool ModeSelection::loadBoxTexture(const std::string& path)
{
    if (!boxTexture.loadFromFile(path))
    {
        std::cout << "[ModeSelection] Cannot load box texture: "
            << path << "\n";
        return false;
    }

    boxTexture.setSmooth(true);

    for (auto& sp : boxSprites)
        sp.setTexture(boxTexture);

    return true;
}

void ModeSelection::setupButtons(const sf::Texture& buttonTex,
    float btnW, float btnH,
    float scaleX, float scaleY)
{
    (void)btnW;
    (void)btnH;

    playButton.setTexture(buttonTex);
    playButton.setScale(scaleX, scaleY);

    backButton.setTexture(buttonTex);
    backButton.setScale(scaleX, scaleY);

    // ===== CHANGED: Adventure/Custom (buoc ChooseType) gio dung texture
    // SilverBox (boxTexture, da duoc nap qua loadBoxTexture()) thay vi
    // button_normal, va phong to hon so voi cac nut khac =====
    sf::Vector2u boxSz = boxTexture.getSize();
    if (boxSz.x > 0 && boxSz.y > 0)
    {
        const float CHOICE_BTN_W = 300.f;
        const float CHOICE_BTN_H = 110.f;
        adventureButton.setTexture(boxTexture);
        adventureButton.setScale(CHOICE_BTN_W / (float)boxSz.x, CHOICE_BTN_H / (float)boxSz.y);

        customButton.setTexture(boxTexture);
        customButton.setScale(CHOICE_BTN_W / (float)boxSz.x, CHOICE_BTN_H / (float)boxSz.y);
    }
    else
    {
        // Fallback neu boxTexture chua duoc nap truoc setupButtons()
        adventureButton.setTexture(buttonTex);
        adventureButton.setScale(scaleX, scaleY);

        customButton.setTexture(buttonTex);
        customButton.setScale(scaleX, scaleY);
    }

    confirmButton.setTexture(buttonTex);
    confirmButton.setScale(scaleX, scaleY);
}

//==================================================
// Layout
//==================================================

void ModeSelection::setupLayout()
{
    float cx = W / 2.f;

    //--------------------------------------------------
    // Title
    //--------------------------------------------------
    titleText.setString("SELECT MODE");
    titleText.setCharacterSize(44);
    titleText.setFillColor(sf::Color(255, 220, 80));
    titleText.setStyle(sf::Text::Bold);
    centerText(titleText, cx, H * 0.06f);

    //--------------------------------------------------
    // 3 box xep ngang, chia deu
    //--------------------------------------------------
    const float BOX_SIZE = 240.f;
    const float GAP = 50.f;
    const float totalWidth = MODE_COUNT * BOX_SIZE + (MODE_COUNT - 1) * GAP;
    const float startX = cx - totalWidth / 2.f;
    const float boxY = H * 0.28f;

    for (int i = 0; i < MODE_COUNT; i++)
    {
        float bx = startX + i * (BOX_SIZE + GAP);

        // Box (tint theo mau rieng cua mode)
        sf::Vector2u sz = boxTexture.getSize();
        if (sz.x > 0 && sz.y > 0)
        {
            float scaleX = BOX_SIZE / (float)sz.x;
            float scaleY = BOX_SIZE / (float)sz.y;
            boxSprites[i].setScale(scaleX, scaleY);
        }
        boxSprites[i].setPosition(bx, boxY);
        boxSprites[i].setColor(modeInfos[i].color);

        // Vien highlight quanh box (chi hien khi dang chon)
        highlightBorders[i].setSize({ BOX_SIZE, BOX_SIZE });
        highlightBorders[i].setPosition(bx, boxY);
        highlightBorders[i].setFillColor(sf::Color::Transparent);
        highlightBorders[i].setOutlineThickness(5.f);
        highlightBorders[i].setOutlineColor(sf::Color::White);

        // Ten mode, can giua trong box
        nameTexts[i].setString(modeInfos[i].name);
        nameTexts[i].setCharacterSize(22);
        nameTexts[i].setStyle(sf::Text::Bold);
        nameTexts[i].setFillColor(sf::Color::White);
        nameTexts[i].setOutlineColor(sf::Color::Black);
        nameTexts[i].setOutlineThickness(2.f);
        centerText(nameTexts[i], bx + BOX_SIZE / 2.f, boxY + BOX_SIZE / 2.f - 14.f);
    }

    //--------------------------------------------------
    // Nut PLAY / BACK
    //--------------------------------------------------
    float buttonY = H * 0.62f;

    playButton.setText("PLAY");
    playButton.setFont(font);
    playButton.setCharacterSize(26);
    playButton.setPosition(cx - 250.f, buttonY);

    backButton.setText("BACK");
    backButton.setFont(font);
    backButton.setCharacterSize(26);
    backButton.setPosition(cx + 10.f, buttonY);

    //--------------------------------------------------
    // Hint
    //--------------------------------------------------
    hintText.setCharacterSize(18);
    hintText.setFillColor(sf::Color(120, 120, 140, 200));

    //--------------------------------------------------
    // ===== ADDED: buoc ChooseType (Adventure/Custom) - chi xuat hien
    // sau khi chon Nightmare o luoi chinh, dung lai vi tri hang box =====
    //--------------------------------------------------
    nightmareChoiceTitle.setFont(font);
    nightmareChoiceTitle.setString("CHOOSE TYPE");
    nightmareChoiceTitle.setCharacterSize(36);
    nightmareChoiceTitle.setStyle(sf::Text::Bold);
    // ===== CHANGED: tim sam dam hon (thay vi tim tuoi cua modeInfos[2]) =====
    nightmareChoiceTitle.setFillColor(sf::Color(80, 0, 110));
    nightmareChoiceTitle.setOutlineColor(sf::Color(20, 0, 30));
    nightmareChoiceTitle.setOutlineThickness(2.f);
    centerText(nightmareChoiceTitle, cx, boxY + 20.f);

    adventureButton.setText("ADVENTURE");
    adventureButton.setFont(font);
    adventureButton.setCharacterSize(22);

    customButton.setText("CUSTOM");
    customButton.setFont(font);
    customButton.setCharacterSize(22);

    //--------------------------------------------------
    // ===== ADDED: buoc EnterLevel - nhap Level muon vuot qua de THANG (1..999), chi
    // danh cho Nightmare + Custom. Moi qua 1 level +100 diem (logic
    // cong diem nam ben CGAME, xem ghi chu o main.cpp) =====
    //--------------------------------------------------
    customPromptText.setFont(font);
    // ===== CHANGED: so nguoi choi nhap gio la LEVEL CAN VUOT QUA DE WIN,
    // khong phai level bat dau nua (level luon bat dau tu 1) - xem
    // CGAME::SetStartingLevel() =====
    customPromptText.setString("Nhap Level muon vuot qua de THANG (1 - 999):");
    customPromptText.setCharacterSize(24);
    customPromptText.setFillColor(sf::Color::White);
    centerText(customPromptText, cx, boxY + 20.f);

    customInputText.setFont(font);
    customInputText.setCharacterSize(36);
    customInputText.setStyle(sf::Text::Bold);
    customInputText.setFillColor(sf::Color(255, 220, 80));

    customErrorText.setFont(font);
    customErrorText.setCharacterSize(18);
    customErrorText.setFillColor(sf::Color(230, 90, 90));

    confirmButton.setText("CONFIRM");
    confirmButton.setFont(font);
    confirmButton.setCharacterSize(24);

    updateHighlight();
    updateNightmareTypeHighlight();
    updateCustomInputDisplay();
    applyStepLayout();
}

//==================================================
// ===== ADDED: dat lai vi tri nut/text theo nightmareStep hien tai -
// goi lai moi khi nightmareStep doi (khong doi kich thuoc man hinh) =====
//==================================================

void ModeSelection::applyStepLayout()
{
    float cx = W / 2.f;
    float buttonY = H * 0.62f;
    float boxY = H * 0.28f;

    switch (nightmareStep)
    {
    case NightmareSubStep::Grid:
        playButton.setPosition(cx - 250.f, buttonY);
        backButton.setPosition(cx + 10.f, buttonY);
        hintText.setString("LEFT / RIGHT chon do kho  |  ENTER = Play  |  ESC = Back");
        break;

    case NightmareSubStep::ChooseType:
    {
        // ===== CHANGED: dua 2 nut Adventure/Custom len cao hon (gan
        // title hon) va cach nhau hop ly voi kich thuoc moi (300x110) =====
        float choiceBtnY = boxY + 130.f;
        adventureButton.setPosition(cx - 320.f, choiceBtnY);
        customButton.setPosition(cx + 20.f, choiceBtnY);
        backButton.setPosition(cx - 125.f, choiceBtnY + 150.f);
        hintText.setString("LEFT / RIGHT chon kieu choi  |  ENTER = Xac nhan  |  ESC = Quay lai");
        break;
    }

    case NightmareSubStep::EnterLevel:
        confirmButton.setPosition(cx - 125.f, buttonY);
        backButton.setPosition(cx - 125.f, buttonY + 90.f);
        hintText.setString("Go so 0-9 (1-999)  |  ENTER = Xac nhan  |  ESC = Quay lai");
        break;
    }

    centerText(hintText, cx, H * 0.955f);
    centerText(customInputText, cx, boxY + 80.f);
    centerText(customErrorText, cx, boxY + 140.f);
}

//==================================================
// ===== ADDED: ve lai chuoi customInputText tu customLevelInput, kem
// con tro nhap "_" o cuoi =====
//==================================================

void ModeSelection::updateCustomInputDisplay()
{
    std::string shown = customLevelInput.empty()
        ? std::string("_")
        : customLevelInput + "_";

    customInputText.setString(shown);
    centerText(customInputText, W / 2.f, H * 0.28f + 80.f);
}

//==================================================
// Result
//==================================================

ModeSelectionResult ModeSelection::getResult() const { return result; }
void                ModeSelection::clearResult() { result = ModeSelectionResult::None; }
int                 ModeSelection::getSelectedMode() const { return selectedIndex; }

// ===== ADDED =====
bool ModeSelection::isCustomNightmare() const { return customNightmare; }
int  ModeSelection::getCustomStartLevel() const { return customStartLevel; }

void ModeSelection::resetNightmareFlow()
{
    nightmareStep = NightmareSubStep::Grid;
    nightmareTypeIndex = 0;
    customNightmare = false;
    customStartLevel = 1;
    customLevelInput.clear();
    customErrorText.setString("");

    updateNightmareTypeHighlight();
    updateCustomInputDisplay();
    applyStepLayout();
}

//==================================================
// Navigation
//==================================================

void ModeSelection::selectPrev()
{
    selectedIndex = (selectedIndex - 1 + MODE_COUNT) % MODE_COUNT;
    updateHighlight();
}

void ModeSelection::selectNext()
{
    selectedIndex = (selectedIndex + 1) % MODE_COUNT;
    updateHighlight();
}

void ModeSelection::updateHighlight()
{
    for (int i = 0; i < MODE_COUNT; i++)
        highlightBorders[i].setOutlineColor(
            i == selectedIndex ? sf::Color::White : sf::Color::Transparent);
}

// ===== ADDED: chi 2 lua chon (Adventure/Custom) nen Left/Right deu toggle =====

void ModeSelection::selectNightmareTypePrev()
{
    nightmareTypeIndex = (nightmareTypeIndex - 1 + 2) % 2;
    updateNightmareTypeHighlight();
}

void ModeSelection::selectNightmareTypeNext()
{
    nightmareTypeIndex = (nightmareTypeIndex + 1) % 2;
    updateNightmareTypeHighlight();
}

void ModeSelection::updateNightmareTypeHighlight()
{
    // Dung Focused co san cua Button de tu doi mau giong hover
    adventureButton.setFocused(nightmareTypeIndex == 0);
    customButton.setFocused(nightmareTypeIndex == 1);
}

// ===== ADDED: Escape/BACK - lui 1 buoc tuy dang o nightmareStep nao =====

void ModeSelection::goBackStep()
{
    backButton.press();
    if (audio) audio->playSound("select");

    switch (nightmareStep)
    {
    case NightmareSubStep::Grid:
        result = ModeSelectionResult::Back;
        break;

    case NightmareSubStep::ChooseType:
        nightmareStep = NightmareSubStep::Grid;
        applyStepLayout();
        break;

    case NightmareSubStep::EnterLevel:
        nightmareStep = NightmareSubStep::ChooseType;
        customLevelInput.clear();
        customErrorText.setString("");
        updateCustomInputDisplay();
        applyStepLayout();
        break;
    }
}

// ===== ADDED: validate Level nguoi choi vua go (1..999) roi xac nhan =====

void ModeSelection::tryConfirmCustomLevel()
{
    if (customLevelInput.empty())
    {
        customErrorText.setString("Vui long nhap 1 so tu 1 den 999");
        centerText(customErrorText, W / 2.f, H * 0.28f + 140.f);
        return;
    }

    int value = 0;
    try
    {
        value = std::stoi(customLevelInput);
    }
    catch (...)
    {
        value = 0;
    }

    if (value < 1 || value > 999)
    {
        customErrorText.setString("Level phai tu 1 den 999");
        centerText(customErrorText, W / 2.f, H * 0.28f + 140.f);
        return;
    }

    customNightmare = true;
    customStartLevel = value;

    confirmButton.press();
    if (audio) audio->playSound("select");
    result = ModeSelectionResult::Selected;
}

//==================================================
// processEvent
//==================================================

void ModeSelection::processEvent(const sf::Event& event,
    const sf::RenderWindow& window)
{
    //================================================
    // Buoc Grid - luoi chinh Easy/Hard/Nightmare. Giu nguyen hanh vi cu,
    // chi khac o cho: neu dang chon Nightmare thi ENTER/PLAY KHONG
    // Selected ngay, ma chuyen sang buoc ChooseType (Adventure/Custom)
    //================================================
    if (nightmareStep == NightmareSubStep::Grid)
    {
        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
            case sf::Keyboard::Left:
            case sf::Keyboard::A:
                selectPrev(); break;

            case sf::Keyboard::Right:
            case sf::Keyboard::D:
                selectNext(); break;

            case sf::Keyboard::Return:
                playButton.press();
                if (audio) audio->playSound("select");

                if (selectedIndex == 2)   // Nightmare -> can chon them Adventure/Custom
                {
                    nightmareStep = NightmareSubStep::ChooseType;
                    nightmareTypeIndex = 0;
                    updateNightmareTypeHighlight();
                    applyStepLayout();
                }
                else
                {
                    customNightmare = false;   // Easy/Hard luon la du lieu co san
                    result = ModeSelectionResult::Selected;
                }
                break;

            case sf::Keyboard::Escape:
                goBackStep();
                break;

            default: break;
            }
        }

        playButton.processEvent(event, window);
        backButton.processEvent(event, window);

        if (event.type == sf::Event::MouseButtonReleased &&
            event.mouseButton.button == sf::Mouse::Left)
        {
            sf::Vector2f mp = window.mapPixelToCoords(
                sf::Mouse::getPosition(window));

            for (int i = 0; i < MODE_COUNT; i++)
            {
                if (boxSprites[i].getGlobalBounds().contains(mp))
                {
                    selectedIndex = i;
                    updateHighlight();
                    if (audio) audio->playSound("select");
                }
            }

            if (playButton.contains(mp))
            {
                playButton.press();
                if (audio) audio->playSound("select");

                if (selectedIndex == 2)
                {
                    nightmareStep = NightmareSubStep::ChooseType;
                    nightmareTypeIndex = 0;
                    updateNightmareTypeHighlight();
                    applyStepLayout();
                }
                else
                {
                    customNightmare = false;
                    result = ModeSelectionResult::Selected;
                }
            }

            if (backButton.contains(mp))
            {
                goBackStep();
            }
        }

        return;
    }

    //================================================
    // Buoc ChooseType - CHI danh cho Nightmare: chon Adventure (choi du
    // lieu co san) hoac Custom (tu nhap Level muon vuot qua de THANG)
    //================================================
    if (nightmareStep == NightmareSubStep::ChooseType)
    {
        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
            case sf::Keyboard::Left:
            case sf::Keyboard::A:
                selectNightmareTypePrev(); break;

            case sf::Keyboard::Right:
            case sf::Keyboard::D:
                selectNightmareTypeNext(); break;

            case sf::Keyboard::Return:
                if (nightmareTypeIndex == 0)
                {
                    adventureButton.press();
                    if (audio) audio->playSound("select");
                    customNightmare = false;
                    result = ModeSelectionResult::Selected;
                }
                else
                {
                    customButton.press();
                    if (audio) audio->playSound("select");
                    nightmareStep = NightmareSubStep::EnterLevel;
                    customLevelInput.clear();
                    customErrorText.setString("");
                    updateCustomInputDisplay();
                    applyStepLayout();
                }
                break;

            case sf::Keyboard::Escape:
                goBackStep();
                break;

            default: break;
            }
        }

        adventureButton.processEvent(event, window);
        customButton.processEvent(event, window);
        backButton.processEvent(event, window);

        if (event.type == sf::Event::MouseButtonReleased &&
            event.mouseButton.button == sf::Mouse::Left)
        {
            sf::Vector2f mp = window.mapPixelToCoords(
                sf::Mouse::getPosition(window));

            if (adventureButton.contains(mp))
            {
                adventureButton.press();
                if (audio) audio->playSound("select");
                customNightmare = false;
                result = ModeSelectionResult::Selected;
            }

            if (customButton.contains(mp))
            {
                customButton.press();
                if (audio) audio->playSound("select");
                nightmareStep = NightmareSubStep::EnterLevel;
                customLevelInput.clear();
                customErrorText.setString("");
                updateCustomInputDisplay();
                applyStepLayout();
            }

            if (backButton.contains(mp))
            {
                goBackStep();
            }
        }

        return;
    }

    //================================================
    // Buoc EnterLevel - nhap Level muon vuot qua de THANG (1..999) khi choi Custom
    //================================================
    if (nightmareStep == NightmareSubStep::EnterLevel)
    {
        if (event.type == sf::Event::TextEntered)
        {
            unsigned int code = event.text.unicode;

            if (code == 8)   // Backspace
            {
                if (!customLevelInput.empty())
                    customLevelInput.pop_back();
                customErrorText.setString("");
                updateCustomInputDisplay();
            }
            else if (code >= '0' && code <= '9' && customLevelInput.size() < 3)
            {
                customLevelInput.push_back(static_cast<char>(code));
                customErrorText.setString("");
                updateCustomInputDisplay();
            }
        }

        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
            case sf::Keyboard::Return:
                tryConfirmCustomLevel();
                break;

            case sf::Keyboard::Escape:
                goBackStep();
                break;

            default: break;
            }
        }

        confirmButton.processEvent(event, window);
        backButton.processEvent(event, window);

        if (event.type == sf::Event::MouseButtonReleased &&
            event.mouseButton.button == sf::Mouse::Left)
        {
            sf::Vector2f mp = window.mapPixelToCoords(
                sf::Mouse::getPosition(window));

            if (confirmButton.contains(mp))
                tryConfirmCustomLevel();

            if (backButton.contains(mp))
                goBackStep();
        }

        return;
    }
}

//==================================================
// update / draw
//==================================================

void ModeSelection::update(float dt)
{
    (void)dt;

    switch (nightmareStep)
    {
    case NightmareSubStep::Grid:
        playButton.update();
        break;

    case NightmareSubStep::ChooseType:
        adventureButton.update();
        customButton.update();
        break;

    case NightmareSubStep::EnterLevel:
        confirmButton.update();
        break;
    }

    backButton.update();
}

void ModeSelection::draw(sf::RenderWindow& window) const
{
    background.draw(window);

    sf::RectangleShape ov;
    ov.setSize({ W, H });
    ov.setFillColor(sf::Color(0, 0, 0, 120));
    window.draw(ov);

    window.draw(titleText);

    switch (nightmareStep)
    {
    case NightmareSubStep::Grid:
        for (int i = 0; i < MODE_COUNT; i++)
        {
            window.draw(boxSprites[i]);
            window.draw(highlightBorders[i]);
            window.draw(nameTexts[i]);
        }
        playButton.draw(window);
        break;

    case NightmareSubStep::ChooseType:
        window.draw(nightmareChoiceTitle);
        adventureButton.draw(window);
        customButton.draw(window);
        break;

    case NightmareSubStep::EnterLevel:
        window.draw(customPromptText);
        window.draw(customInputText);
        window.draw(customErrorText);
        confirmButton.draw(window);
        break;
    }

    backButton.draw(window);

    window.draw(hintText);
}

//==================================================
// Helpers
//==================================================

void ModeSelection::centerText(sf::Text& t, float cx, float y)
{
    sf::FloatRect b = t.getLocalBounds();
    t.setOrigin(b.left + b.width / 2.f, b.top);
    t.setPosition(cx, y);
}