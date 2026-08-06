// ContinueMenu.cpp
#include "ContinueMenu.h"
#include "AudioManager.h"
#include "CGAME.h"

#include <sstream>
#include <algorithm>

namespace
{
    const char* MAP_NAMES[4] = { "CITY", "ANCIENT", "HELL", "SKY" };

    // ===== ADDED: 3 cot tuong ung 3 difficultyMode (0=Easy 1=Hard 2=Nightmare) =====
    const char* MODE_NAMES[3] = { "EASY", "HARD", "NIGHTMARE" };
    const sf::Color MODE_COLORS[3] =
    {
        sf::Color(120, 220, 120),   // Easy - xanh la
        sf::Color(230, 90, 90),     // Hard - do
        sf::Color(190, 120, 240)    // Nightmare - tim
    };

    // Bo cuc 3 cot header + danh sach save ben duoi
    constexpr float HEADER_W = 260.f;
    constexpr float HEADER_H = 76.f;
    constexpr float HEADER_Y_RATIO = 0.20f;   // % chieu cao man hinh
    constexpr float COL_SPACING = 320.f;      // khoang cach tam giua 2 cot lien ke
    constexpr float LIST_GAP = 24.f;          // khoang cach header -> nut save dau tien
    constexpr float ROW_SPACING = 66.f;       // khoang cach giua 2 nut save
    constexpr float BACK_Y_RATIO = 0.85f;

    // ===== ADDED: 3-slice ngang cho SilverBox - tranh meo hinh o 2 dau
    // khi keo dan chieu rong (phan giua la vung phang, keo dan an toan) =====
    // Vung noi dung thuc su (khong tinh) trong SilverBox.png/button_normal.png
    const sf::IntRect SILVERBOX_CONTENT(79, 285, 1381, 416);
    constexpr float CAP_FRAC = 0.27f;   // % be rong 2 dau (goc + oc vit) khong bi keo dan

    void drawThreeSlice(sf::RenderWindow& window, const sf::Texture& tex,
        const sf::IntRect& content, float x, float y, float w, float h)
    {
        int capPx = static_cast<int>(content.width * CAP_FRAC);
        if (capPx * 2 > content.width)
            capPx = content.width / 2;

        float scale = h / static_cast<float>(content.height);
        float capScreenW = capPx * scale;

        sf::Sprite left;
        left.setTexture(tex);
        left.setTextureRect(sf::IntRect(content.left, content.top, capPx, content.height));
        left.setScale(scale, scale);
        left.setPosition(x, y);
        window.draw(left);

        sf::Sprite right;
        right.setTexture(tex);
        right.setTextureRect(sf::IntRect(content.left + content.width - capPx, content.top,
            capPx, content.height));
        right.setScale(scale, scale);
        right.setPosition(x + w - capScreenW, y);
        window.draw(right);

        int midSrcW = content.width - capPx * 2;
        float midScreenW = w - capScreenW * 2.f;

        if (midSrcW > 0 && midScreenW > 0.f)
        {
            sf::Sprite mid;
            mid.setTexture(tex);
            mid.setTextureRect(sf::IntRect(content.left + capPx, content.top, midSrcW, content.height));
            mid.setScale(midScreenW / static_cast<float>(midSrcW), scale);
            mid.setPosition(x + capScreenW, y);
            window.draw(mid);
        }
    }
}

//==================================================
// Helper
//==================================================

void ContinueMenu::centerText(sf::Text& t, float cx, float y)
{
    sf::FloatRect b = t.getLocalBounds();
    t.setOrigin(b.left + b.width / 2.f, b.top);
    t.setPosition(cx, y);
}

void ContinueMenu::centerTextFull(sf::Text& t, float cx, float cy)
{
    sf::FloatRect b = t.getLocalBounds();
    t.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
    t.setPosition(cx, cy);
}

float ContinueMenu::columnCenterX(int mode) const
{
    return W / 2.f + static_cast<float>(mode - 1) * COL_SPACING;
}

sf::FloatRect ContinueMenu::headerRect(int mode) const
{
    float x = columnCenterX(mode) - HEADER_W / 2.f;
    float y = H * HEADER_Y_RATIO;
    return sf::FloatRect(x, y, HEADER_W, HEADER_H);
}

int ContinueMenu::totalButtons() const
{
    int total = 0;
    for (int c = 0; c < MODE_COUNT; ++c)
        total += static_cast<int>(saveButtons[c].size());
    return total;
}

//==================================================
// Constructor
//==================================================

ContinueMenu::ContinueMenu()
{
}

//==================================================
// Setup
//==================================================

void ContinueMenu::setAudioManager(AudioManager* manager)
{
    audio = manager;
}

void ContinueMenu::setWindowSize(float w, float h)
{
    W = w;
    H = h;
}

void ContinueMenu::setBackgroundTexture(const sf::Texture& tex, float sx, float sy)
{
    background.setTexture(tex);
    background.setScale(sx, sy);
}

void ContinueMenu::setFont(const sf::Font& f)
{
    font = &f;

    titleText.setFont(f);
    titleText.setString("CONTINUE");
    titleText.setCharacterSize(44);
    titleText.setStyle(sf::Text::Bold);
    titleText.setFillColor(sf::Color(255, 220, 80));
    centerText(titleText, W / 2.f, H * 0.06f);

    hintText.setFont(f);
    hintText.setString("Chua co tien trinh nao duoc luu");
    hintText.setCharacterSize(22);
    hintText.setFillColor(sf::Color(200, 200, 200));
    centerText(hintText, W / 2.f, H * 0.5f);

    backButton.setFont(f);
    backButton.setText("BACK");
    backButton.setCharacterSize(26);

    for (int m = 0; m < MODE_COUNT; ++m)
    {
        modeLabelText[m].setFont(f);
        modeLabelText[m].setCharacterSize(24);
        modeLabelText[m].setStyle(sf::Text::Bold);

        modeEmptyHint[m].setFont(f);
        modeEmptyHint[m].setCharacterSize(16);
        modeEmptyHint[m].setFillColor(sf::Color(170, 170, 170));
        modeEmptyHint[m].setString("Chua co save");
    }
}

void ContinueMenu::setButtonTexture(const sf::Texture& tex, float scaleX, float scaleY)
{
    buttonTexture = &tex;
    btnScaleX = scaleX;
    btnScaleY = scaleY;
    btnRenderW = tex.getSize().x * scaleX;
    btnRenderH = tex.getSize().y * scaleY;

    backButton.setTexture(tex);
    backButton.setScale(scaleX, scaleY);
}

void ContinueMenu::setModeBoxTexture(const sf::Texture& tex)
{
    modeBoxTexture = &tex;
}

//==================================================
// Refresh - quet lai 4 map x 3 mode = 12 file save
//==================================================

void ContinueMenu::refresh()
{
    for (int c = 0; c < MODE_COUNT; ++c)
    {
        saveButtons[c].clear();
        saveButtonMapIndex[c].clear();
    }

    for (int mode = 0; mode < MODE_COUNT; ++mode)
    {
        sf::FloatRect r = headerRect(mode);

        // Label header cua mode
        modeLabelText[mode].setString(MODE_NAMES[mode]);
        modeLabelText[mode].setFillColor(MODE_COLORS[mode]);
        centerTextFull(modeLabelText[mode], r.left + r.width / 2.f, r.top + r.height / 2.f);

        float y = r.top + r.height + LIST_GAP;

        for (int m = 0; m < 4; ++m)
        {
            int outMap = 0, outChar = 0, outLevel = 1, outScore = 0, outMode = 1;

            if (!CGAME::PeekSaveInfo(CGAME::GetSavePathForMap(m, mode),
                outMap, outChar, outLevel, outScore, outMode))
                continue;

            Button btn;

            if (buttonTexture)
            {
                btn.setTexture(*buttonTexture);
                btn.setScale(btnScaleX, btnScaleY);
            }

            if (font)
                btn.setFont(*font);

            std::ostringstream oss;
            oss << MAP_NAMES[m] << "  Lv" << outLevel << "  " << outScore << "pts";
            btn.setText(oss.str());
            btn.setCharacterSize(18);
            btn.setPosition(r.left + r.width / 2.f - btnRenderW / 2.f, y);
            btn.setFocused(false);

            saveButtons[mode].push_back(btn);
            saveButtonMapIndex[mode].push_back(m);

            y += ROW_SPACING;
        }

        // Hint nho khi mode nay chua co save nao
        centerText(modeEmptyHint[mode], r.left + r.width / 2.f, r.top + r.height + LIST_GAP);
    }

    backButton.setPosition(W / 2.f - btnRenderW / 2.f, H * BACK_Y_RATIO);

    // Chon o dau tien co san (cot dau tien khong rong), khong thi Back
    selectedCol = 0;
    selectedRow = 0;
    onBack = true;

    for (int c = 0; c < MODE_COUNT; ++c)
    {
        if (!saveButtons[c].empty())
        {
            selectedCol = c;
            selectedRow = 0;
            onBack = false;
            break;
        }
    }

    result = ContinueMenuResult::None;
    selectedMapIndex = -1;
    selectedModeIndex = -1;

    updateFocus();
}

//==================================================
// Focus / Navigation
//==================================================

void ContinueMenu::updateFocus()
{
    for (int c = 0; c < MODE_COUNT; ++c)
        for (auto& b : saveButtons[c])
            b.setFocused(false);

    backButton.setFocused(false);

    if (!onBack &&
        selectedCol >= 0 && selectedCol < MODE_COUNT &&
        selectedRow >= 0 && selectedRow < static_cast<int>(saveButtons[selectedCol].size()))
    {
        saveButtons[selectedCol][selectedRow].setFocused(true);
    }
    else
    {
        backButton.setFocused(true);
    }
}

void ContinueMenu::moveHorizontal(int dir)
{
    if (onBack || totalButtons() == 0)
        return;

    int col = selectedCol;

    for (int i = 0; i < MODE_COUNT; ++i)
    {
        col = (col + dir + MODE_COUNT) % MODE_COUNT;

        if (!saveButtons[col].empty())
        {
            selectedCol = col;
            selectedRow = std::min(selectedRow,
                static_cast<int>(saveButtons[col].size()) - 1);
            updateFocus();
            return;
        }
    }
}

void ContinueMenu::moveVertical(int dir)
{
    if (totalButtons() == 0)
    {
        onBack = true;
        updateFocus();
        return;
    }

    if (onBack)
    {
        // Tim mot cot co save de nhay vao (uu tien cot dang nho, roi
        // quet sang cac cot khac)
        int col = selectedCol;
        if (col < 0 || col >= MODE_COUNT || saveButtons[col].empty())
        {
            for (int c = 0; c < MODE_COUNT; ++c)
            {
                if (!saveButtons[c].empty()) { col = c; break; }
            }
        }

        selectedCol = col;
        selectedRow = (dir < 0)
            ? static_cast<int>(saveButtons[col].size()) - 1
            : 0;
        onBack = false;
    }
    else
    {
        int rows = static_cast<int>(saveButtons[selectedCol].size());
        selectedRow += dir;

        if (selectedRow < 0 || selectedRow >= rows)
            onBack = true;
    }

    updateFocus();
}

void ContinueMenu::activateSelected()
{
    if (onBack)
    {
        backButton.press();

        if (audio)
            audio->playSound("select");

        result = ContinueMenuResult::Back;
        return;
    }

    if (selectedCol < 0 || selectedCol >= MODE_COUNT)
        return;

    if (selectedRow < 0 || selectedRow >= static_cast<int>(saveButtons[selectedCol].size()))
        return;

    saveButtons[selectedCol][selectedRow].press();

    if (audio)
        audio->playSound("select");

    selectedMapIndex = saveButtonMapIndex[selectedCol][selectedRow];
    selectedModeIndex = selectedCol;
    result = ContinueMenuResult::Selected;
}

//==================================================
// Result
//==================================================

ContinueMenuResult ContinueMenu::getResult() const
{
    return result;
}

void ContinueMenu::clearResult()
{
    result = ContinueMenuResult::None;
}

//==================================================
// Menu
//==================================================

void ContinueMenu::processEvent(const sf::Event& event,
    const sf::RenderWindow& window)
{
    //-----------------------------
    // Keyboard
    //-----------------------------

    if (event.type == sf::Event::KeyPressed)
    {
        switch (event.key.code)
        {
        case sf::Keyboard::Left:
            moveHorizontal(-1);
            break;

        case sf::Keyboard::Right:
            moveHorizontal(1);
            break;

        case sf::Keyboard::Up:
            moveVertical(-1);
            break;

        case sf::Keyboard::Down:
            moveVertical(1);
            break;

        case sf::Keyboard::Enter:
            activateSelected();
            break;

        case sf::Keyboard::Escape:
            backButton.press();
            if (audio) audio->playSound("select");
            result = ContinueMenuResult::Back;
            break;

        default:
            break;
        }
    }

    //-----------------------------
    // Mouse
    //-----------------------------

    for (int c = 0; c < MODE_COUNT; ++c)
        for (auto& b : saveButtons[c])
            b.processEvent(event, window);

    backButton.processEvent(event, window);

    if (event.type == sf::Event::MouseButtonReleased &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mp = window.mapPixelToCoords(
            { event.mouseButton.x, event.mouseButton.y });

        for (int c = 0; c < MODE_COUNT; ++c)
        {
            for (std::size_t i = 0; i < saveButtons[c].size(); i++)
            {
                if (saveButtons[c][i].contains(mp))
                {
                    selectedCol = c;
                    selectedRow = static_cast<int>(i);
                    onBack = false;
                    activateSelected();
                    return;
                }
            }
        }

        if (backButton.contains(mp))
        {
            onBack = true;
            activateSelected();
        }
    }
}

void ContinueMenu::update(float dt)
{
    (void)dt;

    for (int c = 0; c < MODE_COUNT; ++c)
        for (auto& b : saveButtons[c])
            b.update();

    backButton.update();
}

void ContinueMenu::draw(sf::RenderWindow& window) const
{
    background.draw(window);

    window.draw(titleText);

    bool anySave = (totalButtons() > 0);

    for (int c = 0; c < MODE_COUNT; ++c)
    {
        // Header (SilverBox 3-slice) + nhan mode
        if (modeBoxTexture)
        {
            sf::FloatRect r = headerRect(c);
            drawThreeSlice(window, *modeBoxTexture, SILVERBOX_CONTENT,
                r.left, r.top, r.width, r.height);
        }

        window.draw(modeLabelText[c]);

        if (saveButtons[c].empty())
        {
            window.draw(modeEmptyHint[c]);
        }
        else
        {
            for (const auto& b : saveButtons[c])
                b.draw(window);
        }
    }

    if (!anySave)
        window.draw(hintText);

    backButton.draw(window);
}
