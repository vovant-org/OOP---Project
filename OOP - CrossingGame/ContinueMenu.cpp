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
    // ===== CHANGED (Redesign: khong duoc co khoang trong lon giua cac
    // save slot) 20 -> 10px, danh sach lien tuc giong save manager =====
    constexpr float SLOT_GAP = 10.f;
    constexpr float START_Y = 120.f;        // dinh vung danh sach (duoi title)
    constexpr float BOTTOM_RESERVED = 110.f; // khoang chua nut Back + le duoi
    constexpr float CHAR_IMG_SIZE = 96.f;
    constexpr float CHAR_COL_W = CHAR_IMG_SIZE + 24.f; // be rong cot preview nhan vat

    // ===== ADDED (preview trang thai cuoi cung khi save): bo cuc
    // spritesheet nhan vat, PHAI khop voi CPEOPLE.cpp (FRAME_COLUMNS=4,
    // DIRECTION_ROWS=5) - moi hang la 1 huong (0=UP,1=DOWN,2=LEFT,
    // 3=RIGHT,4=DIE), moi cot la 1 frame animation =====
    constexpr int CHAR_SHEET_COLUMNS = 4;
    constexpr int CHAR_SHEET_ROWS = 5;

    // ===== ADDED (Redesign): kich thuoc/vi tri nut DELETE va badge LOAD
    // rieng cua tung hang, nam ben phai slot giong bo cuc tham khao =====
    constexpr float ACTION_BTN_W = 96.f;
    constexpr float ACTION_BTN_H = 30.f;
    constexpr float ACTION_BTN_GAP = 8.f;   // khoang cach doc giua LOAD va DELETE
    constexpr float ACTION_MARGIN_R = 16.f;

    // ===== ADDED (Redesign): bang mau pixel-art navy/cyan cho save slot,
    // thay cho panel ve bang texture 3-slice cu =====
    const sf::Color PANEL_BG(10, 20, 32);
    const sf::Color PANEL_BG_INVALID(8, 14, 20);
    const sf::Color PANEL_PREVIEW_BG(5, 12, 20);
    const sf::Color PANEL_SEP(28, 58, 68);
    const sf::Color BORDER_CYAN(58, 150, 168);
    const sf::Color BORDER_CYAN_DIM(40, 66, 72);
    const sf::Color BORDER_CYAN_BRIGHT(110, 232, 248);
    const sf::Color TEXT_INFO(216, 244, 250);
    const sf::Color TEXT_INFO_DIM(123, 184, 196);
    const sf::Color LOAD_BG(28, 74, 84);
    const sf::Color LOAD_BORDER(110, 232, 248);
    const sf::Color LOAD_TEXT(234, 253, 255);
    const sf::Color DELETE_BG(48, 16, 16);
    const sf::Color DELETE_BG_HOVER(80, 24, 24);
    const sf::Color DELETE_BORDER(190, 80, 80);
    const sf::Color DELETE_BORDER_HOVER(255, 130, 130);
    const sf::Color DELETE_TEXT(255, 216, 216);

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
        slotInfo[i].lastDirection = sd.lastDirection;
        slotInfo[i].lastFrame = sd.lastFrame;

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

// ===== ADDED (Redesign): nguon tinh toan DUY NHAT cho vi tri/kich thuoc
// hang save thu i, theo toa do NOI DUNG cuc bo (0..W, bat dau tu 0 cho
// hang dau) - dung giong het trong draw() va trong xu ly click, tranh
// truong hop vung ve va vung click bi lech nhau =====
sf::FloatRect ContinueMenu::slotRect(std::size_t index) const
{
    float slotW = W * SLOT_W_FRAC;
    float x = W / 2.f - slotW / 2.f;
    float y = static_cast<float>(index) * (SLOT_H + SLOT_GAP);
    return sf::FloatRect(x, y, slotW, SLOT_H);
}

sf::FloatRect ContinueMenu::deleteButtonRect(std::size_t index) const
{
    sf::FloatRect row = slotRect(index);
    float x = row.left + row.width - ACTION_BTN_W - ACTION_MARGIN_R;
    float y = row.top + (row.height / 2.f) + (ACTION_BTN_GAP / 2.f);
    return sf::FloatRect(x, y, ACTION_BTN_W, ACTION_BTN_H);
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

        // ===== ADDED (Redesign): nut DELETE rieng cua tung hang - kiem
        // tra TRUOC vung LOAD/hang vi no nam long ben trong hang do.
        // Dung deleteButtonRect() (cung 1 cong thuc voi luc ve trong
        // draw()) thay vi Button::contains() de vung click luon khop
        // dung vung hien thi =====
        for (std::size_t i = 0; i < slotInfo.size(); ++i)
        {
            if (deleteButtonRect(i).contains(mouseListPos))
            {
                if (audio) audio->playSound("select");
                CGAME::DeleteSave(slotInfo[i].filePath);
                refresh();
                return;
            }
        }

        // Cac hang save (LOAD): tai su dung mouseListPos da tinh o tren
        // (chuot chua kip di chuyen giua luc bam va luc tha trong cung
        // 1 event). ===== CHANGED: dung slotRect() (khop dung panel thuc
        // te ve tren man hinh) thay vi slotButtons[i].contains() =====
        for (std::size_t i = 0; i < slotInfo.size(); ++i)
        {
            if (slotRect(i).contains(mouseListPos))
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

    // ===== ADDED (Redesign): vi tri chuot trong khong gian noi dung
    // cua listView (view hien tai cua window DA la listView, vi vua
    // setView(listView) o tren) - dung de hover nut DELETE luc ve =====
    sf::Vector2f mouseListPosDraw =
        window.mapPixelToCoords(sf::Mouse::getPosition(window));

    for (std::size_t i = 0; i < slotInfo.size(); ++i)
    {
        // ===== CHANGED (Redesign): slotRect() la nguon toa do DUY NHAT,
        // dung chung voi processEvent() =====
        sf::FloatRect r = slotRect(i);
        float x = r.left, y = r.top;
        float slotW = r.width;

        bool valid = slotInfo[i].isValid;
        bool isSelectedRow = (!onBack && i == static_cast<std::size_t>(selectedIndex));

        //--- Panel nen: pixel-art flat, navy toi + vien cyan ---
        sf::RectangleShape panel({ slotW, SLOT_H });
        panel.setPosition(x, y);
        panel.setFillColor(valid ? PANEL_BG : PANEL_BG_INVALID);
        panel.setOutlineThickness(isSelectedRow ? 3.f : 2.f);
        panel.setOutlineColor(isSelectedRow ? BORDER_CYAN_BRIGHT
            : (valid ? BORDER_CYAN : BORDER_CYAN_DIM));
        window.draw(panel);

        //--- Cot preview nhan vat (ben trai), tach rieng bang mot vach ---
        sf::RectangleShape charBox({ CHAR_COL_W, SLOT_H });
        charBox.setPosition(x, y);
        charBox.setFillColor(PANEL_PREVIEW_BG);
        window.draw(charBox);

        sf::RectangleShape charSep({ 2.f, SLOT_H });
        charSep.setPosition(x + CHAR_COL_W, y);
        charSep.setFillColor(PANEL_SEP);
        window.draw(charSep);

        // preview nhan vat: dung DUNG frame/huong tai thoi diem save
        // cuoi cung (lastDirection/lastFrame), thay vi ve nguyen ca tam
        // spritesheet 4x5. Neu huong luu la DIE (4) - VD file save cu tu
        // ban build truoc khi co tinh nang nay, hoac hi hoi hiem gap luc
        // save dung luc chet - fallback ve DOWN (1) de preview khong
        // hien tu the "gap" trong Continue Menu =====
        if (valid && slotInfo[i].characterIndex >= 0 && slotInfo[i].characterIndex < 4)
        {
            const sf::Texture& charTex = charTextures[slotInfo[i].characterIndex];
            int sheetW = static_cast<int>(charTex.getSize().x);
            int sheetH = static_cast<int>(charTex.getSize().y);

            if (sheetW > 0 && sheetH > 0)
            {
                int frameW = sheetW / CHAR_SHEET_COLUMNS;
                int frameH = sheetH / CHAR_SHEET_ROWS;

                int dir = slotInfo[i].lastDirection;
                if (dir < 0 || dir >= CHAR_SHEET_ROWS - 1) dir = 1;   // DIE hoac gia tri la -> DOWN

                int col = slotInfo[i].lastFrame;
                if (col < 0 || col >= CHAR_SHEET_COLUMNS) col = 0;

                sf::Sprite cs;
                cs.setTexture(charTex);
                cs.setTextureRect(sf::IntRect(col * frameW, dir * frameH, frameW, frameH));

                float scale = CHAR_IMG_SIZE / static_cast<float>(frameH);
                float drawnW = frameW * scale;
                cs.setScale(scale, scale);
                cs.setPosition(x + (CHAR_COL_W - drawnW) / 2.f, y + (SLOT_H - CHAR_IMG_SIZE) / 2.f);
                window.draw(cs);
            }
        }

        //--- Thong tin save: 2 cot gon, giua slot ---
        if (font)
        {
            float infoX = x + CHAR_COL_W + 18.f;
            float infoRightX = x + CHAR_COL_W + 18.f + (slotW - CHAR_COL_W) * 0.5f;

            if (!valid)
            {
                sf::Text invalidText;
                invalidText.setFont(*font);
                invalidText.setCharacterSize(16);
                invalidText.setFillColor(TEXT_INFO_DIM);
                invalidText.setString("INVALID SAVE");
                invalidText.setPosition(infoX, y + (SLOT_H - 20.f) / 2.f);
                window.draw(invalidText);
            }
            else
            {
                sf::Text infoText;
                infoText.setFont(*font);
                infoText.setCharacterSize(16);
                infoText.setFillColor(TEXT_INFO);

                std::ostringstream oss;

                // cot trai: MAP / MODE / DATE
                int mi = slotInfo[i].mapIndex;
                oss << "MAP: " << ((mi >= 0 && mi < 4) ? MAP_NAMES[mi] : "UNKNOWN");
                infoText.setString(oss.str());
                infoText.setPosition(infoX, y + 16.f);
                window.draw(infoText);

                oss.str(""); oss.clear();
                int md = slotInfo[i].difficultyMode;
                oss << "MODE: " << ((md >= 0 && md < 3) ? MODE_NAMES[md] : "UNKNOWN");
                infoText.setString(oss.str());
                infoText.setPosition(infoX, y + 42.f);
                window.draw(infoText);

                // cot phai: LEVEL / SCORE
                oss.str(""); oss.clear();
                oss << "LEVEL: " << slotInfo[i].level;
                infoText.setString(oss.str());
                infoText.setPosition(infoRightX, y + 16.f);
                window.draw(infoText);

                oss.str(""); oss.clear();
                oss << "SCORE: " << slotInfo[i].score;
                infoText.setString(oss.str());
                infoText.setPosition(infoRightX, y + 42.f);
                window.draw(infoText);

                // DATE: hang duoi cung, trai qua het chieu rong thong tin
                sf::Text timeText;
                timeText.setFont(*font);
                timeText.setCharacterSize(16);
                timeText.setFillColor(TEXT_INFO_DIM);
                timeText.setString("DATE: " + slotInfo[i].saveTime);
                timeText.setPosition(infoX, y + 72.f);
                window.draw(timeText);
            }
        }

        //--- Badge LOAD (ben phai, phia tren nut DELETE) ---
        if (valid)
        {
            sf::FloatRect loadR(
                x + slotW - ACTION_BTN_W - ACTION_MARGIN_R,
                y + (SLOT_H / 2.f) - ACTION_BTN_GAP / 2.f - ACTION_BTN_H,
                ACTION_BTN_W, ACTION_BTN_H);

            sf::RectangleShape loadBtn({ loadR.width, loadR.height });
            loadBtn.setPosition(loadR.left, loadR.top);
            loadBtn.setFillColor(LOAD_BG);
            loadBtn.setOutlineThickness(1.f);
            loadBtn.setOutlineColor(LOAD_BORDER);
            window.draw(loadBtn);

            if (font)
            {
                sf::Text loadText;
                loadText.setFont(*font);
                loadText.setCharacterSize(16);
                loadText.setFillColor(LOAD_TEXT);
                loadText.setString("LOAD");
                centerTextFull(loadText, loadR.left + loadR.width / 2.f, loadR.top + loadR.height / 2.f);
                window.draw(loadText);
            }
        }

        //--- Nut DELETE (luon co, ke ca save hong, de nguoi choi xoa) ---
        {
            sf::FloatRect delR = deleteButtonRect(i);
            bool hovered = delR.contains(mouseListPosDraw);

            sf::RectangleShape delBtn({ delR.width, delR.height });
            delBtn.setPosition(delR.left, delR.top);
            delBtn.setFillColor(hovered ? DELETE_BG_HOVER : DELETE_BG);
            delBtn.setOutlineThickness(1.f);
            delBtn.setOutlineColor(hovered ? DELETE_BORDER_HOVER : DELETE_BORDER);
            window.draw(delBtn);

            if (font)
            {
                sf::Text delText;
                delText.setFont(*font);
                delText.setCharacterSize(16);
                delText.setFillColor(DELETE_TEXT);
                delText.setString("DELETE");
                centerTextFull(delText, delR.left + delR.width / 2.f, delR.top + delR.height / 2.f);
                window.draw(delText);
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