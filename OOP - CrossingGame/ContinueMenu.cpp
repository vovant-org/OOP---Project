// ContinueMenu.cpp
#include "ContinueMenu.h"
#include "AudioManager.h"
#include "CGAME.h"

#include <sstream>
#include <algorithm>
#include <ctime>

namespace
{
    const char* MAP_NAMES[4] = { "CITY", "ANCIENT", "HELL", "SKY" };
    const char* MODE_NAMES[3] = { "EASY", "HARD", "NIGHTMARE" };

    // Layout
    constexpr float SLOT_W_FRAC = 0.82f;
    constexpr float SLOT_H = 120.f;
    constexpr float SLOT_GAP = 20.f;
    constexpr float START_Y = 120.f;        // dinh vung danh sach (duoi title)
    constexpr float BOTTOM_RESERVED = 110.f; // khoang chua nut Back + le duoi
    constexpr float CHAR_IMG_SIZE = 96.f;

    // 3-slice content for silver/golden box (match AboutMenu / Leaderboard)
    const sf::IntRect SILVERBOX_CONTENT(86, 323, 1374, 366);
    constexpr float SILVER_CAP_FRAC = 0.27f;

    void drawThreeSliceLocal(sf::RenderWindow& window, const sf::Texture& tex,
        const sf::IntRect& content, float x, float y, float w, float h)
    {
        int capPx = static_cast<int>(content.width * SILVER_CAP_FRAC);
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

void ContinueMenu::setSlotTexture(const sf::Texture& tex)
{
    slotBoxTexture = &tex;
}

//==================================================
// Refresh - quet dong toan bo Save/
//==================================================

void ContinueMenu::refresh()
{
    // load character textures once
    if (!charTexturesLoaded)
    {
        const std::string paths[4] = {
            "Character/Chicken_character.png",
            "Character/Knight_character.png",
            "Character/Dog_character.png",
            "Character/Luffy_character.png"
        };
        for (int i = 0; i < 4; ++i)
        {
            charTextures[i].loadFromFile(paths[i]);
        }
        charTexturesLoaded = true;
    }

    // ===== CHANGED (Giai doan 2): danh sach DONG thay vi 4 slot co dinh =====
    std::vector<CGAME::SaveData> saves = CGAME::ListAllSaves();

    slotButtons.clear();
    slotInfo.clear();
    slotButtons.resize(saves.size());
    slotInfo.resize(saves.size());

    float slotW = W * SLOT_W_FRAC;
    float x = W / 2.f - slotW / 2.f;

    for (std::size_t i = 0; i < saves.size(); ++i)
    {
        const CGAME::SaveData& sd = saves[i];

        slotInfo[i].exists = sd.exists;
        slotInfo[i].isValid = sd.isValid;
        slotInfo[i].filePath = sd.filePath;
        slotInfo[i].fileSizeBytes = sd.fileSizeBytes;
        slotInfo[i].lastWriteTimeUnix = sd.lastWriteTimeUnix;
        slotInfo[i].characterIndex = sd.characterIndex;
        slotInfo[i].playerName = sd.playerName;
        slotInfo[i].mapIndex = sd.mapIndex;
        slotInfo[i].score = sd.score;
        slotInfo[i].difficultyMode = sd.difficultyMode;
        slotInfo[i].level = sd.level;
        slotInfo[i].saveTime = sd.saveTime;

        Button btn;
        if (buttonTexture)
        {
            btn.setTexture(*buttonTexture);
            btn.setScale(btnScaleX, btnScaleY);
        }
        if (font) btn.setFont(*font);

        btn.setCharacterSize(18);

        // ===== CHANGED (Giai doan 2): toa do Y la toa do NOI DUNG cuc bo
        // (local, bat dau tu 0 cho hang dau tien), KHONG con cong them
        // START_Y va KHONG tru scrollOffset o day nua - viec dat dung vi
        // tri thuc te tren man hinh (bao gom ca cuon) do sf::View rieng
        // trong computeListView() dam nhan luc draw()/xu ly click =====
        float y = static_cast<float>(i) * (SLOT_H + SLOT_GAP);
        btn.setPosition(x, y);
        btn.setFocused(false);
        slotButtons[i] = btn;
    }

    // back button (nam NGOAI vung cuon, toa do man hinh binh thuong)
    backButton.setPosition(W / 2.f - btnRenderW / 2.f, H - 80.f);

    // ===== CHANGED: neu khong co save nao, focus thang vao Back de
    // tranh selectedIndex tro vao danh sach rong =====
    selectedIndex = 0;
    onBack = slotButtons.empty();
    selectedRowIndex = -1;
    result = ContinueMenuResult::None;
    scrollOffset = 0.f;   // ===== ADDED: reset vi tri cuon moi lan mo lai menu =====

    updateFocus();
}

//==================================================
// Layout / Scroll helpers (Giai doan 2)
//==================================================

sf::FloatRect ContinueMenu::listAreaDesign() const
{
    float top = START_Y;
    float bottom = H - BOTTOM_RESERVED;
    float height = bottom - top;
    if (height < 0.f) height = 0.f;
    return sf::FloatRect(0.f, top, W, height);
}

sf::View ContinueMenu::computeListView(const sf::RenderWindow& window) const
{
    sf::FloatRect area = listAreaDesign();

    // View HIEN TAI cua window co the da bi letterbox (xem
    // applyLetterboxView() trong main.cpp) - list view phai la 1 view
    // "con" nam long trong dung viewport cua view do, khong phai toan
    // bo window, de van hien dung vi tri khi fullscreen/ti le man hinh
    // khac 16:9.
    sf::View parentView = window.getView();
    sf::FloatRect parentVp = parentView.getViewport();

    float topFrac = (H > 0.f) ? (area.top / H) : 0.f;
    float heightFrac = (H > 0.f) ? (area.height / H) : 0.f;

    sf::FloatRect subViewport(
        parentVp.left,
        parentVp.top + parentVp.height * topFrac,
        parentVp.width,
        parentVp.height * heightFrac
    );

    sf::View listView;
    listView.setViewport(subViewport);
    listView.setSize(area.width, area.height);
    // Nhin vao toa do NOI DUNG cuc bo (bat dau tu 0), dich xuong theo
    // scrollOffset - day la diem mau chot tao hieu ung cuon
    listView.setCenter(area.width / 2.f, area.height / 2.f + scrollOffset);

    return listView;
}

float ContinueMenu::contentHeight() const
{
    if (slotInfo.empty()) return 0.f;
    return static_cast<float>(slotInfo.size()) * (SLOT_H + SLOT_GAP) - SLOT_GAP;
}

float ContinueMenu::maxScrollOffset() const
{
    float overflow = contentHeight() - listAreaDesign().height;
    return (overflow > 0.f) ? overflow : 0.f;
}

void ContinueMenu::clampScroll()
{
    float maxS = maxScrollOffset();
    if (scrollOffset < 0.f) scrollOffset = 0.f;
    if (scrollOffset > maxS) scrollOffset = maxS;
}

void ContinueMenu::ensureRowVisible(int index)
{
    if (index < 0 || index >= static_cast<int>(slotInfo.size())) return;

    sf::FloatRect area = listAreaDesign();
    float rowTop = static_cast<float>(index) * (SLOT_H + SLOT_GAP);
    float rowBottom = rowTop + SLOT_H;

    if (rowTop < scrollOffset)
        scrollOffset = rowTop;
    else if (rowBottom > scrollOffset + area.height)
        scrollOffset = rowBottom - area.height;

    clampScroll();
}

void ContinueMenu::drawScrollbar(sf::RenderWindow& window) const
{
    float maxS = maxScrollOffset();
    if (maxS <= 0.f) return;   // noi dung vua khung nhin, khong can thanh cuon

    sf::FloatRect area = listAreaDesign();

    constexpr float TRACK_W = 6.f;
    float trackX = W - 22.f;

    sf::RectangleShape track({ TRACK_W, area.height });
    track.setPosition(trackX, area.top);
    track.setFillColor(sf::Color(255, 255, 255, 40));
    window.draw(track);

    float ch = contentHeight();
    float thumbHFrac = (ch > 0.f) ? (area.height / ch) : 1.f;
    if (thumbHFrac > 1.f) thumbHFrac = 1.f;
    if (thumbHFrac < 0.08f) thumbHFrac = 0.08f;   // toi thieu de van con nhin thay/keo duoc

    float thumbH = area.height * thumbHFrac;
    float thumbY = area.top + (scrollOffset / maxS) * (area.height - thumbH);

    sf::RectangleShape thumb({ TRACK_W, thumbH });
    thumb.setPosition(trackX, thumbY);
    thumb.setFillColor(sf::Color(255, 220, 80, 200));
    window.draw(thumb);
}

//==================================================
// Focus / Navigation
//==================================================

void ContinueMenu::updateFocus()
{
    for (auto& b : slotButtons)
        b.setFocused(false);

    backButton.setFocused(false);

    if (!onBack && selectedIndex >= 0 && selectedIndex < static_cast<int>(slotButtons.size()))
    {
        slotButtons[selectedIndex].setFocused(true);
    }
    else
    {
        backButton.setFocused(true);
    }
}

void ContinueMenu::moveVertical(int dir)
{
    // ===== ADDED (Giai doan 2): danh sach rong -> luon o Back, khong co
    // gi de duyet, tranh slotButtons.size()-1 tran so (size_t) =====
    if (slotButtons.empty())
    {
        onBack = true;
        updateFocus();
        return;
    }

    if (onBack)
    {
        // move from back to last slot (or first)
        selectedIndex = (dir < 0) ? static_cast<int>(slotButtons.size()) - 1 : 0;
        onBack = false;
    }
    else
    {
        selectedIndex += dir;
        if (selectedIndex < 0)
        {
            onBack = true;
        }
        else if (selectedIndex >= static_cast<int>(slotButtons.size()))
        {
            onBack = true;
        }
    }

    // clamp
    if (selectedIndex < 0) selectedIndex = 0;
    if (selectedIndex >= static_cast<int>(slotButtons.size())) selectedIndex = static_cast<int>(slotButtons.size()) - 1;

    // ===== ADDED (Giai doan 2): tu dong cuon de hang dang chon luon
    // nam trong khung nhin khi di chuyen bang ban phim =====
    if (!onBack)
        ensureRowVisible(selectedIndex);

    updateFocus();
}

void ContinueMenu::activateSelected()
{
    if (onBack || slotButtons.empty())
    {
        backButton.press();
        if (audio) audio->playSound("select");
        result = ContinueMenuResult::Back;
        return;
    }

    if (selectedIndex < 0 || selectedIndex >= static_cast<int>(slotButtons.size()))
        return;

    // ===== ADDED (Giai doan 2): khong cho Load 1 file save bi hong
    // (isValid=false) - viec cho phep Delete file hong se lam o Giai
    // doan 4 (nut Delete rieng tung dong) =====
    if (!slotInfo[selectedIndex].isValid)
        return;

    slotButtons[selectedIndex].press();
    if (audio) audio->playSound("select");
    selectedRowIndex = selectedIndex;
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

std::string ContinueMenu::getSelectedSavePath() const
{
    if (selectedRowIndex < 0 || selectedRowIndex >= static_cast<int>(slotInfo.size()))
        return std::string();
    return slotInfo[selectedRowIndex].filePath;
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
        case sf::Keyboard::Up:
        case sf::Keyboard::W:
            moveVertical(-1);
            break;

        case sf::Keyboard::Down:
        case sf::Keyboard::S:
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
    // Chuot lan (scroll)
    //-----------------------------
    if (event.type == sf::Event::MouseWheelScrolled)
    {
        // delta duong = lan len -> cuon LEN (giam scrollOffset)
        scrollOffset -= event.mouseWheelScroll.delta * (SLOT_H + SLOT_GAP) * 0.5f;
        clampScroll();
    }

    //-----------------------------
    // Hover cho tung hang trong danh sach: PHAI dung view rieng co scroll
    // (computeListView), KHONG the goi slotButtons[i].processEvent(event,
    // window) nhu truoc vi ham do tu map chuot theo view HIEN TAI cua
    // window (view mac dinh, khong biet gi ve scrollOffset) -> se sai vi
    // tri hover moi khi da cuon xuong. Dung overload moi cua Button nhan
    // thang toa do da map san.
    //-----------------------------
    sf::View listView = computeListView(window);
    sf::Vector2f mouseListPos =
        window.mapPixelToCoords(sf::Mouse::getPosition(window), listView);

    for (auto& b : slotButtons)
        b.processEvent(event, mouseListPos); // processed in list view coordinates

    // Back button nam NGOAI vung cuon -> van dung overload cu (view mac dinh)
    backButton.processEvent(event, window);

    if (event.type == sf::Event::MouseButtonReleased &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mpDefault = window.mapPixelToCoords(
            { event.mouseButton.x, event.mouseButton.y });

        if (backButton.contains(mpDefault))
        {
            onBack = true;
            activateSelected();
            return;
        }

        // Cac hang save: tai su dung mouseListPos da tinh o tren (chuot
        // chua kip di chuyen giua luc bam va luc tha trong cung 1 event)
        for (std::size_t i = 0; i < slotButtons.size(); ++i)
        {
            if (slotButtons[i].contains(mouseListPos))
            {
                selectedIndex = static_cast<int>(i);
                onBack = false;
                activateSelected();
                return;
            }
        }
    }
}

void ContinueMenu::update(float dt)
{
    (void)dt;
    for (auto& b : slotButtons) b.update();
    backButton.update();
}

void ContinueMenu::draw(sf::RenderWindow& window) const
{
    background.draw(window);
    window.draw(titleText);

    if (slotInfo.empty())
    {
        window.draw(hintText);
        backButton.draw(window);
        return;
    }

    // ===== ADDED (Giai doan 2): chuyen sang view rieng chi bao phu vung
    // danh sach (listAreaDesign()), da dich theo scrollOffset - moi thu
    // ve BEN TRONG khoi nay se tu dong bi CAT (clip) dung ranh gioi vung
    // hien thi va cuon dung vi tri, khong can code clip thu cong =====
    sf::View savedView = window.getView();
    sf::View listView = computeListView(window);
    window.setView(listView);

    float slotW = W * SLOT_W_FRAC;
    float x = W / 2.f - slotW / 2.f;

    for (std::size_t i = 0; i < slotInfo.size(); ++i)
    {
        // ===== CHANGED: toa do Y la toa do NOI DUNG cuc bo (khong con
        // START_Y), khop voi cach slotButtons duoc dat trong refresh() =====
        float y = static_cast<float>(i) * (SLOT_H + SLOT_GAP);

        // draw slot background using three-slice to preserve corners
        if (slotBoxTexture)
        {
            drawThreeSliceLocal(window, *slotBoxTexture, SILVERBOX_CONTENT, x, y, slotW, SLOT_H);
        }
        else if (buttonTexture)
        {
            sf::Sprite s;
            s.setTexture(*buttonTexture);
            float sx = slotW / btnRenderW;
            float sy = SLOT_H / btnRenderH;
            s.setScale(sx, sy);
            s.setPosition(x, y);
            window.draw(s);
        }
        else
        {
            sf::RectangleShape rect({ slotW, SLOT_H });
            rect.setPosition(x, y);
            rect.setFillColor(sf::Color(40, 40, 40));
            window.draw(rect);
        }

        // draw character image (chi khi doc du lieu thanh cong)
        if (slotInfo[i].isValid && slotInfo[i].characterIndex >= 0 && slotInfo[i].characterIndex < 4)
        {
            sf::Sprite cs;
            cs.setTexture(charTextures[slotInfo[i].characterIndex]);
            float scale = CHAR_IMG_SIZE / static_cast<float>(charTextures[slotInfo[i].characterIndex].getSize().y);
            cs.setScale(scale, scale);
            cs.setPosition(x + 12.f, y + (SLOT_H - CHAR_IMG_SIZE) / 2.f);
            window.draw(cs);
        }

        // draw texts
        if (font)
        {
            sf::Text nameText;
            nameText.setFont(*font);
            nameText.setCharacterSize(22);
            nameText.setFillColor(sf::Color::White);

            // ===== CHANGED (Giai doan 2): dung isValid thay vi exists -
            // exists gio luon true (file co that tren dia), isValid moi
            // phan anh dung viec parse noi dung co thanh cong hay khong =====
            if (slotInfo[i].isValid && !slotInfo[i].playerName.empty())
                nameText.setString(slotInfo[i].playerName);
            else if (!slotInfo[i].isValid)
                nameText.setString("INVALID SAVE");
            else
                nameText.setString(" ");

            nameText.setPosition(x + CHAR_IMG_SIZE + 24.f, y + 8.f);
            window.draw(nameText);

            if (slotInfo[i].isValid)
            {
                sf::Text infoText;
                infoText.setFont(*font);
                infoText.setCharacterSize(18);
                infoText.setFillColor(sf::Color(200, 200, 200));

                std::ostringstream oss;
                int mi = slotInfo[i].mapIndex;
                std::string mname = (mi >= 0 && mi < 4) ? MAP_NAMES[mi] : "UNKNOWN";
                oss << "MAP: " << mname;
                infoText.setString(oss.str());
                infoText.setPosition(x + CHAR_IMG_SIZE + 24.f, y + 36.f);
                window.draw(infoText);

                oss.str(""); oss.clear();
                int md = slotInfo[i].difficultyMode;
                oss << "MODE: " << ((md >= 0 && md < 3) ? MODE_NAMES[md] : "UNKNOWN");
                infoText.setString(oss.str());
                infoText.setPosition(x + CHAR_IMG_SIZE + 24.f, y + 58.f);
                window.draw(infoText);

                oss.str(""); oss.clear();
                oss << "LEVEL: " << slotInfo[i].level;
                infoText.setString(oss.str());
                infoText.setPosition(x + CHAR_IMG_SIZE + 260.f, y + 36.f);
                window.draw(infoText);

                oss.str(""); oss.clear();
                oss << "SCORE: " << slotInfo[i].score;
                infoText.setString(oss.str());
                infoText.setPosition(x + CHAR_IMG_SIZE + 260.f, y + 58.f);
                window.draw(infoText);

                sf::Text timeText;
                timeText.setFont(*font);
                timeText.setCharacterSize(16);
                timeText.setFillColor(sf::Color(170, 170, 170));
                timeText.setString(slotInfo[i].saveTime);
                timeText.setPosition(x + CHAR_IMG_SIZE + 24.f, y + 82.f);
                window.draw(timeText);
            }

            // draw selection highlight
            if (!onBack && i == static_cast<std::size_t>(selectedIndex))
            {
                sf::RectangleShape frame({ slotW, SLOT_H });
                frame.setPosition(x, y);
                frame.setFillColor(sf::Color::Transparent);
                frame.setOutlineColor(sf::Color(255, 220, 80));
                frame.setOutlineThickness(3.f);
                window.draw(frame);
            }
        }
    }

    // ===== ADDED (Giai doan 2): tra view ve nhu cu TRUOC KHI ve scrollbar
    // va Back button - 2 thu nay nam NGOAI vung cuon, phai o toa do man
    // hinh binh thuong =====
    window.setView(savedView);

    drawScrollbar(window);

    backButton.draw(window);
}