// LeaderboardMenu.cpp
#include "LeaderboardMenu.h"
#include "AudioManager.h"
#include "CGAME.h"

#include <sstream>
#include <algorithm>
#include <iostream>
#include <system_error>
#include <filesystem>   // ===== ADDED: can C++17 (Project > Properties > C++ Language Standard) =====

namespace
{
    // Thu tu PHAI khop voi outCharacter cua CGAME::PeekSaveInfo, cung la
    // thu tu main.cpp dang goi loadCharacterTexture()
    const char* CHAR_NAMES[4] = { "Chicken", "Knight", "Dog", "Luffy" };

    const char* MODE_NAMES[3] = { "EASY", "HARD", "NIGHTMARE" };
    const sf::Color MODE_COLORS[3] =
    {
        sf::Color(120, 220, 120),   // Easy - xanh la
        sf::Color(230, 90, 90),     // Hard - do
        sf::Color(190, 120, 240)    // Nightmare - tim
    };

    //----------------------------------
    // Layout
    //----------------------------------

    constexpr float PANEL_W = 760.f;
    constexpr float PANEL_H = 520.f;
    constexpr float PANEL_TOP_RATIO = 0.064f;   // % chieu cao man hinh

    constexpr float PANEL_PAD_X = 40.f;
    constexpr float PANEL_PAD_TOP = 92.f;    // chua title
    constexpr float PANEL_PAD_BOTTOM = 20.f;

    constexpr float ROW_H = 64.f;
    constexpr float ROW_GAP = 10.f;

    constexpr float AVATAR_SIZE = 48.f;
    constexpr float AVATAR_PAD = 10.f;
    constexpr float AVATAR_INNER_RATIO = 0.72f;   // dau nhan vat nho hon khung 1 chut de thay vien

    constexpr int   AVATAR_CROP_ROW = 1;      // hang 2 trong sprite sheet (giong preview CharacterSelection)
    constexpr float AVATAR_CROP_RATIO = 0.68f; // % frameH lay lam vung vuong "phan tren" cua nhan vat

    constexpr float NAME_X = AVATAR_PAD + AVATAR_SIZE + 14.f;   // can trai
    constexpr float SCORE_CX_RATIO = 0.54f;
    constexpr float LEVEL_CX_RATIO = 0.72f;
    constexpr float MODE_CX_RATIO = 0.89f;

    constexpr float BACK_GAP = 20.f;

    constexpr float SCROLL_WHEEL_STEP = 46.f;
    constexpr float SCROLL_KEY_STEP = 36.f;

    //----------------------------------
    // 9-slice cho GoldenBox (khung ngoai vuong, 4 canh deu can giu vien
    // khong bi meo) va 3-slice ngang cho SilverBox (tam dai, chi can giu
    // 2 dau, giua la vung phang keo dan an toan - giong ky thuat da dung
    // trong ContinueMenu.cpp)
    //----------------------------------

    const sf::IntRect GOLDENBOX_CONTENT(53, 58, 1149, 1142);
    constexpr float GOLDENBOX_CAP_SCREEN = 34.f;   // do day vien tren man hinh (px)
    constexpr float GOLDENBOX_CAP_FRAC = 0.20f;    // % be rong/cao noi dung khong bi keo dan

    const sf::IntRect SILVERBOX_CONTENT(79, 285, 1381, 416);
    constexpr float SILVER_CAP_FRAC = 0.27f;

    // Khung avatar mau theo do kho - vung noi dung thuc su (khong tinh
    // vien trong suot bao quanh) trong tung file Easy/Hard/NightmareAvatar.png
    const sf::IntRect MODE_FRAME_CONTENT[3] =
    {
        sf::IntRect(227, 207, 796, 811),   // Easy
        sf::IntRect(236, 188, 788, 813),   // Hard
        sf::IntRect(243, 216, 768, 790)    // Nightmare
    };

    void drawThreeSlice(sf::RenderWindow& window, const sf::Texture& tex,
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

    // ===== ADDED: 9-slice cho khung vuong GoldenBox - 4 goc giu nguyen ty
    // le (khong meo), 4 canh chi keo dan theo 1 chieu, o giua keo dan ca 2
    // chieu (vung phang, an toan) =====
    void drawNineSlice(sf::RenderWindow& window, const sf::Texture& tex,
        const sf::IntRect& content, float x, float y, float w, float h)
    {
        int capPxX = static_cast<int>(content.width * GOLDENBOX_CAP_FRAC);
        int capPxY = static_cast<int>(content.height * GOLDENBOX_CAP_FRAC);
        if (capPxX * 2 > content.width)  capPxX = content.width / 2;
        if (capPxY * 2 > content.height) capPxY = content.height / 2;

        float capL = GOLDENBOX_CAP_SCREEN;   // do day vien tren man hinh, ca 4 canh
        float midSrcW = static_cast<float>(content.width - capPxX * 2);
        float midSrcH = static_cast<float>(content.height - capPxY * 2);
        float midScreenW = w - capL * 2.f;
        float midScreenH = h - capL * 2.f;

        auto drawPart = [&](int sx, int sy, int sw, int sh,
            float dx, float dy, float dw, float dh)
            {
                if (sw <= 0 || sh <= 0 || dw <= 0.f || dh <= 0.f)
                    return;

                sf::Sprite sp;
                sp.setTexture(tex);
                sp.setTextureRect(sf::IntRect(sx, sy, sw, sh));
                sp.setScale(dw / static_cast<float>(sw), dh / static_cast<float>(sh));
                sp.setPosition(dx, dy);
                window.draw(sp);
            };

        int cl = content.left, ct = content.top;
        int cr = content.left + content.width - capPxX;
        int cb = content.top + content.height - capPxY;

        // 4 goc - scale DONG DEU (khong meo)
        drawPart(cl, ct, capPxX, capPxY, x, y, capL, capL);
        drawPart(cr, ct, capPxX, capPxY, x + w - capL, y, capL, capL);
        drawPart(cl, cb, capPxX, capPxY, x, y + h - capL, capL, capL);
        drawPart(cr, cb, capPxX, capPxY, x + w - capL, y + h - capL, capL, capL);

        // 4 canh - keo dan theo 1 chieu
        drawPart(cl + capPxX, ct, static_cast<int>(midSrcW), capPxY,
            x + capL, y, midScreenW, capL);
        drawPart(cl + capPxX, cb, static_cast<int>(midSrcW), capPxY,
            x + capL, y + h - capL, midScreenW, capL);
        drawPart(cl, ct + capPxY, capPxX, static_cast<int>(midSrcH),
            x, y + capL, capL, midScreenH);
        drawPart(cr, ct + capPxY, capPxX, static_cast<int>(midSrcH),
            x + w - capL, y + capL, capL, midScreenH);

        // Chinh giua - keo dan ca 2 chieu (vung phang, an toan)
        drawPart(cl + capPxX, ct + capPxY, static_cast<int>(midSrcW), static_cast<int>(midSrcH),
            x + capL, y + capL, midScreenW, midScreenH);
    }
}

//==================================================
// Helper
//==================================================

void LeaderboardMenu::centerText(sf::Text& t, float cx, float cy)
{
    sf::FloatRect b = t.getLocalBounds();
    t.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
    t.setPosition(cx, cy);
}

void LeaderboardMenu::leftAlignText(sf::Text& t, float x, float cy)
{
    sf::FloatRect b = t.getLocalBounds();
    t.setOrigin(b.left, b.top + b.height / 2.f);
    t.setPosition(x, cy);
}

//==================================================
// Constructor
//==================================================

LeaderboardMenu::LeaderboardMenu()
{
}

//==================================================
// Setup
//==================================================

void LeaderboardMenu::setAudioManager(AudioManager* manager)
{
    audio = manager;
}

void LeaderboardMenu::setWindowSize(float w, float h)
{
    W = w;
    H = h;

    setupLayout();
}

void LeaderboardMenu::setBackgroundTexture(const sf::Texture& tex, float sx, float sy)
{
    background.setTexture(tex);
    background.setScale(sx, sy);
}

void LeaderboardMenu::setFont(const sf::Font& f)
{
    font = &f;

    titleText.setFont(f);
    titleText.setString("LEADERBOARD");
    titleText.setCharacterSize(40);
    titleText.setStyle(sf::Text::Bold);
    titleText.setFillColor(sf::Color(255, 220, 80));

    emptyHintText.setFont(f);
    emptyHintText.setString("Chua co thanh tich nao duoc luu");
    emptyHintText.setCharacterSize(20);
    emptyHintText.setFillColor(sf::Color(200, 200, 200));

    backButton.setFont(f);
    backButton.setText("BACK");
    backButton.setCharacterSize(26);

    setupLayout();
}

void LeaderboardMenu::setPanelTexture(const sf::Texture& tex)
{
    panelTexture = &tex;
    setupLayout();
}

void LeaderboardMenu::setRowBoxTexture(const sf::Texture& tex)
{
    rowBoxTexture = &tex;
    setupLayout();
}

void LeaderboardMenu::setButtonTexture(const sf::Texture& tex, float scaleX, float scaleY)
{
    buttonTexture = &tex;
    btnScaleX = scaleX;
    btnScaleY = scaleY;
    btnRenderW = tex.getSize().x * scaleX;
    btnRenderH = tex.getSize().y * scaleY;

    backButton.setTexture(tex);
    backButton.setScale(scaleX, scaleY);

    setupLayout();
}

void LeaderboardMenu::loadCharacterTexture(int index, const std::string& path)
{
    if (index < 0 || index >= CHAR_COUNT)
        return;

    if (!characterTextures[index].loadFromFile(path))
    {
        std::cout << "[ERROR] LeaderboardMenu: cannot load character texture: "
            << path << "\n";
        return;
    }

    characterTextures[index].setSmooth(false);
}

void LeaderboardMenu::loadModeFrameTexture(int modeIndex, const std::string& path)
{
    if (modeIndex < 0 || modeIndex >= MODE_COUNT)
        return;

    if (!modeFrameTextures[modeIndex].loadFromFile(path))
    {
        std::cout << "[ERROR] LeaderboardMenu: cannot load mode frame texture: "
            << path << "\n";
        return;
    }
}

//==================================================
// Layout
//==================================================

void LeaderboardMenu::setupLayout()
{
    float panelLeft = W / 2.f - PANEL_W / 2.f;
    float panelTop = H * PANEL_TOP_RATIO;

    panelRect = sf::FloatRect(panelLeft, panelTop, PANEL_W, PANEL_H);

    listRect = sf::FloatRect(
        panelLeft + PANEL_PAD_X,
        panelTop + PANEL_PAD_TOP,
        PANEL_W - 2.f * PANEL_PAD_X,
        PANEL_H - PANEL_PAD_TOP - PANEL_PAD_BOTTOM);

    if (font)
    {
        centerText(titleText, W / 2.f, panelTop + 34.f);
        centerText(emptyHintText, listRect.left + listRect.width / 2.f,
            listRect.top + listRect.height / 2.f);
    }

    backButton.setPosition(W / 2.f - btnRenderW / 2.f,
        panelRect.top + panelRect.height + BACK_GAP);

    // Panel/list vua doi vi tri man hinh -> dung lai entries[] (neu da co
    // du lieu tu refresh() truoc do) de tao lai rows[] dung vi tri moi,
    // KHONG doc lai file / KHONG sap xep lai
    rebuildRows();
}

//==================================================
// Rebuild rows - dung entries[] (da sort) + layout hien tai
//==================================================

void LeaderboardMenu::rebuildRows()
{
    rows.clear();

    if (!font)
        return;

    float rowW = listRect.width;
    float scoreCX = rowW * SCORE_CX_RATIO;
    float levelCX = rowW * LEVEL_CX_RATIO;
    float modeCX = rowW * MODE_CX_RATIO;

    float y = 0.f;

    for (std::size_t i = 0; i < entries.size(); ++i)
    {
        const Entry& e = entries[i];

        RowVisual rv;
        rv.y = y;

        //--- Khung avatar mau theo do kho ---
        if (e.mode >= 0 && e.mode < MODE_COUNT && modeFrameTextures[e.mode].getSize().x > 0)
        {
            const sf::IntRect& c = MODE_FRAME_CONTENT[e.mode];
            rv.avatarFrame.setTexture(modeFrameTextures[e.mode]);
            rv.avatarFrame.setTextureRect(c);
            rv.avatarFrame.setScale(AVATAR_SIZE / c.width, AVATAR_SIZE / c.height);
            rv.avatarFrame.setPosition(AVATAR_PAD, y + (ROW_H - AVATAR_SIZE) / 2.f);
        }

        //--- Dau nhan vat (chi lay phan tren, hang 2 cot 1 trong sprite sheet) ---
        if (e.character >= 0 && e.character < CHAR_COUNT &&
            characterTextures[e.character].getSize().x > 0)
        {
            const sf::Texture& tex = characterTextures[e.character];
            float frameW = static_cast<float>(tex.getSize().x) / 4.f;
            float frameH = static_cast<float>(tex.getSize().y) / 5.f;

            float cropSize = frameH * AVATAR_CROP_RATIO;
            if (cropSize > frameW)
                cropSize = frameW;

            sf::IntRect src(
                static_cast<int>((frameW - cropSize) / 2.f),
                static_cast<int>(frameH * AVATAR_CROP_ROW),
                static_cast<int>(cropSize),
                static_cast<int>(cropSize));

            float innerSize = AVATAR_SIZE * AVATAR_INNER_RATIO;

            rv.avatarHead.setTexture(tex);
            rv.avatarHead.setTextureRect(src);
            rv.avatarHead.setScale(innerSize / src.width, innerSize / src.height);
            rv.avatarHead.setPosition(
                AVATAR_PAD + (AVATAR_SIZE - innerSize) / 2.f,
                y + (ROW_H - AVATAR_SIZE) / 2.f + (AVATAR_SIZE - innerSize) / 2.f);
        }

        //--- Thu hang (#1, #2, ...) ---
        rv.rankText.setFont(*font);
        rv.rankText.setCharacterSize(14);
        rv.rankText.setFillColor(sf::Color(190, 190, 190));
        {
            std::ostringstream oss;
            oss << "#" << (i + 1);
            rv.rankText.setString(oss.str());
        }
        rv.rankText.setPosition(AVATAR_PAD - 2.f, y + 1.f);

        //--- Ten nhan vat ---
        rv.nameText.setFont(*font);
        rv.nameText.setCharacterSize(18);
        rv.nameText.setStyle(sf::Text::Bold);
        rv.nameText.setFillColor(sf::Color(255, 220, 80));
        rv.nameText.setString(
            (e.character >= 0 && e.character < CHAR_COUNT) ? CHAR_NAMES[e.character] : "?");
        leftAlignText(rv.nameText, NAME_X, y + ROW_H / 2.f);

        //--- Diem ---
        rv.scoreText.setFont(*font);
        rv.scoreText.setCharacterSize(18);
        rv.scoreText.setFillColor(sf::Color(220, 220, 220));
        {
            std::ostringstream oss;
            oss << e.score << "pts";
            rv.scoreText.setString(oss.str());
        }
        centerText(rv.scoreText, scoreCX, y + ROW_H / 2.f);

        //--- Level ---
        rv.levelText.setFont(*font);
        rv.levelText.setCharacterSize(18);
        rv.levelText.setFillColor(sf::Color(220, 220, 220));
        {
            std::ostringstream oss;
            oss << "Lv" << e.level;
            rv.levelText.setString(oss.str());
        }
        centerText(rv.levelText, levelCX, y + ROW_H / 2.f);

        //--- Mode ---
        rv.modeText.setFont(*font);
        rv.modeText.setCharacterSize(16);
        rv.modeText.setStyle(sf::Text::Bold);
        rv.modeText.setFillColor(
            (e.mode >= 0 && e.mode < MODE_COUNT) ? MODE_COLORS[e.mode] : sf::Color::White);
        rv.modeText.setString(
            (e.mode >= 0 && e.mode < MODE_COUNT) ? MODE_NAMES[e.mode] : "?");
        centerText(rv.modeText, modeCX, y + ROW_H / 2.f);

        rows.push_back(std::move(rv));

        y += ROW_H + ROW_GAP;
    }

    float contentH = rows.empty() ? 0.f : (y - ROW_GAP);
    maxScroll = std::max(0.f, contentH - listRect.height);
    clampScroll();
}

void LeaderboardMenu::clampScroll()
{
    if (scrollOffset < 0.f) scrollOffset = 0.f;
    if (scrollOffset > maxScroll) scrollOffset = maxScroll;
}

//==================================================
// Refresh - quet lai 12 file save (4 map x 3 mode), gop + sap xep
//==================================================

void LeaderboardMenu::refresh()
{
    entries.clear();

    for (int map = 0; map < MAP_COUNT; ++map)
    {
        for (int mode = 0; mode < MODE_COUNT; ++mode)
        {
            int outMap = 0, outChar = 0, outLevel = 1, outScore = 0, outMode = 0;
            const std::string& path = CGAME::GetSavePathForMap(map, mode);

            if (!CGAME::PeekSaveInfo(path, outMap, outChar, outLevel, outScore, outMode))
                continue;

            Entry e;
            e.map = map;
            e.mode = mode;
            e.character = outChar;
            e.level = outLevel;
            e.score = outScore;

            // ===== Xap xi moc "da ton tai tu luc nao" bang thoi gian SUA
            // FILE GAN NHAT (last write time), vi file save hien khong luu
            // rieng 1 timestamp "tao luc nao". Han che: neu file duoc
            // luu/ghi de nhieu lan (moi lan len level/qua man deu goi
            // SaveGame()), moc nay phan anh LAN LUU GAN NHAT chu khong
            // phai lan dau tien - trong da so truong hop van hop ly vi
            // save cu it bi doi lai sau khi da vuot qua. Neu can chinh
            // xac tuyet doi, phai luu them 1 truong timestamp rieng ngay
            // trong file save (CGAME::SaveGame/LoadGame) =====
            std::error_code ec;
            auto ft = std::filesystem::last_write_time(path, ec);
            e.savedTimeTicks = ec ? 0LL
                : static_cast<long long>(ft.time_since_epoch().count());

            entries.push_back(e);
        }
    }

    std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b)
        {
            if (a.score != b.score) return a.score > b.score;
            if (a.level != b.level) return a.level > b.level;
            return a.savedTimeTicks < b.savedTimeTicks;   // ton tai truoc -> tren
        });

    rebuildRows();
    scrollOffset = 0.f;

    result = LeaderboardMenuResult::None;
}

//==================================================
// Menu
//==================================================

void LeaderboardMenu::processEvent(const sf::Event& event,
    const sf::RenderWindow& window)
{
    //-----------------------------
    // Cuon (chuot lan)
    //-----------------------------

    if (event.type == sf::Event::MouseWheelScrolled)
    {
        sf::Vector2f mp = window.mapPixelToCoords(
            sf::Vector2i(event.mouseWheelScroll.x, event.mouseWheelScroll.y));

        if (listRect.contains(mp))
        {
            scrollOffset -= event.mouseWheelScroll.delta * SCROLL_WHEEL_STEP;
            clampScroll();
        }
    }

    //-----------------------------
    // Ban phim - Up/Down cuon danh sach, Enter/Escape la Back
    //-----------------------------

    if (event.type == sf::Event::KeyPressed)
    {
        switch (event.key.code)
        {
        case sf::Keyboard::Up:
            scrollOffset -= SCROLL_KEY_STEP;
            clampScroll();
            break;

        case sf::Keyboard::Down:
            scrollOffset += SCROLL_KEY_STEP;
            clampScroll();
            break;

        case sf::Keyboard::Enter:
        case sf::Keyboard::Escape:
            backButton.press();
            if (audio) audio->playSound("select");
            result = LeaderboardMenuResult::Back;
            break;

        default:
            break;
        }
    }

    //-----------------------------
    // Chuot - Back
    //-----------------------------

    backButton.processEvent(event, window);

    if (event.type == sf::Event::MouseButtonReleased &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mp = window.mapPixelToCoords(
            { event.mouseButton.x, event.mouseButton.y });

        if (backButton.contains(mp))
        {
            backButton.press();

            if (audio)
                audio->playSound("select");

            result = LeaderboardMenuResult::Back;
        }
    }
}

void LeaderboardMenu::update(float dt)
{
    (void)dt;

    backButton.update();
}

void LeaderboardMenu::draw(sf::RenderWindow& window) const
{
    background.draw(window);

    if (panelTexture)
    {
        drawNineSlice(window, *panelTexture, GOLDENBOX_CONTENT,
            panelRect.left, panelRect.top, panelRect.width, panelRect.height);
    }

    window.draw(titleText);

    if (rows.empty())
    {
        window.draw(emptyHintText);
    }
    else
    {
        // ===== Cuon + clip danh sach bang sf::View: viewport moi duoc
        // "chong" len dung vi tri listRect trong viewport HIEN TAI cua
        // window (co the dang bi letterbox boi applyLetterboxView), nen
        // luon dung bat ke ty le/kich thuoc cua so thuc te the nao =====
        sf::View savedView = window.getView();

        sf::Vector2f oldSize = savedView.getSize();
        sf::Vector2f oldCenter = savedView.getCenter();
        sf::FloatRect oldRect(oldCenter.x - oldSize.x / 2.f, oldCenter.y - oldSize.y / 2.f,
            oldSize.x, oldSize.y);
        sf::FloatRect oldViewport = savedView.getViewport();

        float fracLeft = (oldRect.width > 0.f) ? (listRect.left - oldRect.left) / oldRect.width : 0.f;
        float fracTop = (oldRect.height > 0.f) ? (listRect.top - oldRect.top) / oldRect.height : 0.f;
        float fracW = (oldRect.width > 0.f) ? listRect.width / oldRect.width : 1.f;
        float fracH = (oldRect.height > 0.f) ? listRect.height / oldRect.height : 1.f;

        sf::View listView;
        listView.setSize(listRect.width, listRect.height);
        listView.setCenter(listRect.width / 2.f, scrollOffset + listRect.height / 2.f);
        listView.setViewport(sf::FloatRect(
            oldViewport.left + fracLeft * oldViewport.width,
            oldViewport.top + fracTop * oldViewport.height,
            fracW * oldViewport.width,
            fracH * oldViewport.height));

        window.setView(listView);

        for (const auto& rv : rows)
        {
            if (rowBoxTexture)
            {
                drawThreeSlice(window, *rowBoxTexture, SILVERBOX_CONTENT,
                    0.f, rv.y, listRect.width, ROW_H);
            }

            window.draw(rv.avatarFrame);
            window.draw(rv.avatarHead);
            window.draw(rv.rankText);
            window.draw(rv.nameText);
            window.draw(rv.scoreText);
            window.draw(rv.levelText);
            window.draw(rv.modeText);
        }

        window.setView(savedView);

        // Thanh cuon nho, chi hien khi noi dung vuot khung
        if (maxScroll > 0.f)
        {
            float trackX = listRect.left + listRect.width + 6.f;
            float trackY = listRect.top;
            float trackH = listRect.height;

            sf::RectangleShape track(sf::Vector2f(4.f, trackH));
            track.setPosition(trackX, trackY);
            track.setFillColor(sf::Color(50, 50, 50, 150));
            window.draw(track);

            float visibleRatio = listRect.height / (listRect.height + maxScroll);
            float thumbH = std::max(20.f, trackH * visibleRatio);
            float thumbY = trackY + (trackH - thumbH) * (scrollOffset / maxScroll);

            sf::RectangleShape thumb(sf::Vector2f(4.f, thumbH));
            thumb.setPosition(trackX, thumbY);
            thumb.setFillColor(sf::Color(255, 220, 80, 220));
            window.draw(thumb);
        }
    }

    backButton.draw(window);
}

//==================================================
// Result
//==================================================

LeaderboardMenuResult LeaderboardMenu::getResult() const
{
    return result;
}

void LeaderboardMenu::clearResult()
{
    result = LeaderboardMenuResult::None;
}
