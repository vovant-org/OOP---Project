// AboutMenu.cpp
#include "AboutMenu.h"
#include "AudioManager.h"

#include <algorithm>

namespace
{
    constexpr float PANEL_W = 780.f;
    constexpr float PANEL_H = 560.f;
    constexpr float PANEL_TOP_RATIO = 0.055f;

    constexpr float PANEL_PAD_X = 40.f;
    constexpr float PANEL_PAD_TOP = 90.f;    // chua title
    constexpr float PANEL_PAD_BOTTOM = 20.f;

    constexpr float CARD_PAD_X = 24.f;
    constexpr float CARD_PAD_TOP = 16.f;
    constexpr float CARD_PAD_BOTTOM = 16.f;
    constexpr float HEADER_BODY_GAP = 8.f;
    constexpr float CARD_GAP = 18.f;   // khoang cach giua 2 card

    // ===== Trang thai THU GON: card chi cao vua du header (+ le tren/
    // duoi doi xung CARD_PAD_TOP) - giong het khoang cach da dung o dinh
    // card khi MO RONG nen header khong bi "nhay" vi tri giua 2 trang
    // thai, chi rieng chieu cao card va viec co ve body hay khong doi =====
    constexpr float CARD_PAD_BOTTOM_COLLAPSED = CARD_PAD_TOP;

    constexpr float BACK_GAP = 20.f;

    constexpr float SCROLL_WHEEL_STEP = 46.f;
    constexpr float SCROLL_KEY_STEP = 40.f;

    // ===== Nut mui ten (DownArrow) o ben phai moi card, dung de dong/
    // mo rong card do =====
    constexpr float ARROW_DRAW_W = 20.f;    // be rong hien thi (khi ve)
    constexpr float ARROW_HIT_W = 44.f;     // vung bam (rong hon anh that de de bam)
    constexpr float ARROW_HIT_H = 44.f;

    // ===== 9-slice cho GoldenBox (khung vuong, 4 canh deu) va 3-slice
    // ngang cho ThinSliverBox (tam dai, 2 dau giu nguyen, giua keo dan) -
    // giong het ky thuat da dung trong LeaderboardMenu.cpp =====
    const sf::IntRect GOLDENBOX_CONTENT(53, 58, 1149, 1142);
    constexpr float GOLDENBOX_CAP_SCREEN = 30.f;
    constexpr float GOLDENBOX_CAP_FRAC = 0.20f;

    // ThinSliverBox.png: vung noi dung thuc (khong tinh vien trong suot)
    const sf::IntRect SILVERBOX_CONTENT(86, 323, 1374, 366);
    constexpr float SILVER_CAP_FRAC = 0.27f;

    // DownArrow.png: vung noi dung thuc (khong tinh vien trong suot)
    const sf::IntRect ARROW_CONTENT(234, 479, 786, 304);

    // ===== Mau hover/press cho icon mui ten - lay DUNG mau mac dinh cua
    // class Button (xem Button::Button()) de dong bo phong cach voi cac
    // nut khac trong game (BACK...), du mui ten khong dung Button that
    // (ly do xem ghi chu o AboutMenu::processEvent) =====
    const sf::Color ARROW_NORMAL_COLOR = sf::Color::White;
    const sf::Color ARROW_HOVER_COLOR = sf::Color(255, 230, 120);
    const sf::Color ARROW_PRESSED_COLOR = sf::Color(255, 180, 40);
    constexpr float ARROW_HOVER_SCALE = 1.15f;   // mui ten nho nen boost manh hon nut thuong (Button dung 1.05f)
    const sf::Time ARROW_PRESS_FLASH = sf::milliseconds(90);   // "nhap nhay" mau khi vua bam, giong Button::pressDuration

    // ===== Noi dung 3 muc - Sua truc tiep o day neu can doi text. Font
    // dang dung (PixelOperator) KHONG co dau tieng Viet nen giu nguyen
    // khong dau, giong toan bo text khac trong game =====
    const char* SECTION_HEADERS[3] = { "MO TA GAME", "DIEU KHIEN", "CREDITS" };

    const char* SECTION_BODIES[3] =
    {
        "CROSSING GAME - the loai Casual / Arcade (kieu Frogger)\n"
        "Dua nhan vat vuot qua 4 vung dat: City, Ancient, Hell, Sky,\n"
        "ne xe co, thu hoang va hiem hoa rieng cua tung map.\n"
        "Chon do kho Easy / Hard / Nightmare, ghi diem va leo hang\n"
        "tren Leaderboard.",

        "Mui ten        Di chuyen nhan vat / Chon muc\n"
        "Enter          Xac nhan / Bat dau\n"
        "Esc            Quay lai / Huy\n"
        "F11            Bat / tat toan man hinh",

        "Nhom: OOP - 04\n"
        "Vo Van Thien       - 25127148\n"
        "Vu Thanh Nam       - 25127097\n"
        "Nguyen Ngoc Hai    - 25127044\n"
        "Pham Tien Dat      - 25127034"
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

    void drawNineSlice(sf::RenderWindow& window, const sf::Texture& tex,
        const sf::IntRect& content, float x, float y, float w, float h)
    {
        int capPxX = static_cast<int>(content.width * GOLDENBOX_CAP_FRAC);
        int capPxY = static_cast<int>(content.height * GOLDENBOX_CAP_FRAC);
        if (capPxX * 2 > content.width)  capPxX = content.width / 2;
        if (capPxY * 2 > content.height) capPxY = content.height / 2;

        float capL = GOLDENBOX_CAP_SCREEN;
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

        drawPart(cl, ct, capPxX, capPxY, x, y, capL, capL);
        drawPart(cr, ct, capPxX, capPxY, x + w - capL, y, capL, capL);
        drawPart(cl, cb, capPxX, capPxY, x, y + h - capL, capL, capL);
        drawPart(cr, cb, capPxX, capPxY, x + w - capL, y + h - capL, capL, capL);

        drawPart(cl + capPxX, ct, static_cast<int>(midSrcW), capPxY,
            x + capL, y, midScreenW, capL);
        drawPart(cl + capPxX, cb, static_cast<int>(midSrcW), capPxY,
            x + capL, y + h - capL, midScreenW, capL);
        drawPart(cl, ct + capPxY, capPxX, static_cast<int>(midSrcH),
            x, y + capL, capL, midScreenH);
        drawPart(cr, ct + capPxY, capPxX, static_cast<int>(midSrcH),
            x + w - capL, y + capL, capL, midScreenH);

        drawPart(cl + capPxX, ct + capPxY, static_cast<int>(midSrcW), static_cast<int>(midSrcH),
            x + capL, y + capL, midScreenW, midScreenH);
    }
}

//==================================================
// Helper
//==================================================

void AboutMenu::centerText(sf::Text& t, float cx, float cy)
{
    sf::FloatRect b = t.getLocalBounds();
    t.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
    t.setPosition(cx, cy);
}

//==================================================
// Constructor
//==================================================

AboutMenu::AboutMenu()
{
}

//==================================================
// Setup
//==================================================

void AboutMenu::setAudioManager(AudioManager* manager)
{
    audio = manager;
}

void AboutMenu::setWindowSize(float w, float h)
{
    W = w;
    H = h;

    setupLayout();
}

void AboutMenu::setBackgroundTexture(const sf::Texture& tex, float sx, float sy)
{
    background.setTexture(tex);
    background.setScale(sx, sy);
}

void AboutMenu::setFont(const sf::Font& f)
{
    font = &f;

    titleText.setFont(f);
    titleText.setString("ABOUT");
    titleText.setCharacterSize(40);
    titleText.setStyle(sf::Text::Bold);
    titleText.setFillColor(sf::Color(255, 220, 80));

    for (int i = 0; i < SECTION_COUNT; ++i)
    {
        sections[i].header.setFont(f);
        sections[i].header.setString(SECTION_HEADERS[i]);
        sections[i].header.setCharacterSize(20);
        sections[i].header.setStyle(sf::Text::Bold);
        sections[i].header.setFillColor(sf::Color(255, 220, 80));

        sections[i].body.setFont(f);
        sections[i].body.setString(SECTION_BODIES[i]);
        sections[i].body.setCharacterSize(17);
        sections[i].body.setFillColor(sf::Color(220, 220, 220));
        sections[i].body.setLineSpacing(1.3f);
    }

    backButton.setFont(f);
    backButton.setText("BACK");
    backButton.setCharacterSize(26);

    setupLayout();
}

void AboutMenu::setPanelTexture(const sf::Texture& tex)
{
    panelTexture = &tex;
    setupLayout();
}

void AboutMenu::setSectionBoxTexture(const sf::Texture& tex)
{
    sectionBoxTexture = &tex;
    setupLayout();
}

void AboutMenu::setArrowTexture(const sf::Texture& tex)
{
    arrowTexture = &tex;
}

void AboutMenu::setButtonTexture(const sf::Texture& tex, float scaleX, float scaleY)
{
    backButton.setTexture(tex);
    backButton.setScale(scaleX, scaleY);

    setupLayout();
}

//==================================================
// Layout
//==================================================

void AboutMenu::setupLayout()
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
        centerText(titleText, W / 2.f, panelTop + 34.f);

    backButton.setPosition(W / 2.f - 110.f, panelRect.top + panelRect.height + BACK_GAP);

    //--- Xep 3 card doc. Header luon o CUNG mot vi tri tuong doi so voi
    // dinh card (CARD_PAD_TOP) o CA HAI trang thai, nen khi thu gon/mo
    // rong header (va nut arrow di kem) khong bi nhay vi tri - chi rieng
    // chieu cao card (va viec co ve body hay khong) thay doi. Chieu cao
    // khi MO RONG tinh tu do dai THUC TE cua body text (getLocalBounds())
    // - card nao noi dung dai hon tu dong cao hon, khong can hardcode so
    // dong ---
    if (!font)
        return;

    float y = 0.f;

    for (int i = 0; i < SECTION_COUNT; ++i)
    {
        SectionCard& sc = sections[i];

        sc.header.setPosition(CARD_PAD_X, y + CARD_PAD_TOP);

        float headerH = sc.header.getLocalBounds().top + sc.header.getLocalBounds().height;

        float bodyY = y + CARD_PAD_TOP + headerH + HEADER_BODY_GAP;
        sc.body.setPosition(CARD_PAD_X, bodyY);

        float bodyH = sc.body.getLocalBounds().top + sc.body.getLocalBounds().height;

        sc.collapsedHeight = CARD_PAD_TOP + headerH + CARD_PAD_BOTTOM_COLLAPSED;
        sc.expandedHeight = (bodyY - y) + bodyH + CARD_PAD_BOTTOM;
        sc.height = sc.expanded ? sc.expandedHeight : sc.collapsedHeight;
        sc.y = y;

        // Nut mui ten: cung hang voi header, sat le phai card
        sc.arrowCenter = sf::Vector2f(
            listRect.width - CARD_PAD_X - ARROW_DRAW_W / 2.f,
            y + CARD_PAD_TOP + headerH / 2.f);

        y += sc.height + CARD_GAP;
    }

    float contentH = (SECTION_COUNT > 0) ? (y - CARD_GAP) : 0.f;
    maxScroll = std::max(0.f, contentH - listRect.height);
    clampScroll();
}

bool AboutMenu::tryToggleCardAt(const sf::Vector2f& localPoint)
{
    for (int i = 0; i < SECTION_COUNT; ++i)
    {
        SectionCard& sc = sections[i];

        sf::FloatRect hitRect(
            sc.arrowCenter.x - ARROW_HIT_W / 2.f,
            sc.arrowCenter.y - ARROW_HIT_H / 2.f,
            ARROW_HIT_W, ARROW_HIT_H);

        if (hitRect.contains(localPoint))
        {
            sc.expanded = !sc.expanded;
            sc.arrowPressClock.restart();   // bat dau "nhap nhay" mau pressed cho icon
            setupLayout();   // cap nhat lai chieu cao/vi tri toan bo danh sach
            return true;
        }
    }

    return false;
}

void AboutMenu::clampScroll()
{
    if (scrollOffset < 0.f) scrollOffset = 0.f;
    if (scrollOffset > maxScroll) scrollOffset = maxScroll;
}

//==================================================
// Menu
//==================================================

void AboutMenu::processEvent(const sf::Event& event,
    const sf::RenderWindow& window)
{
    //-----------------------------
    // Hover mui ten - cap nhat MOI LAN goi ham nay (khong phu thuoc loai
    // event), giong cach Button::processEvent luon tu doi chieu chuot
    // hien tai de tinh 'hovered' =====
    //-----------------------------

    {
        sf::Vector2f mp = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        bool insideList = listRect.contains(mp);
        sf::Vector2f localPoint(mp.x - listRect.left, mp.y - listRect.top + scrollOffset);

        for (int i = 0; i < SECTION_COUNT; ++i)
        {
            SectionCard& sc = sections[i];

            sf::FloatRect hitRect(
                sc.arrowCenter.x - ARROW_HIT_W / 2.f,
                sc.arrowCenter.y - ARROW_HIT_H / 2.f,
                ARROW_HIT_W, ARROW_HIT_H);

            sc.arrowHovered = insideList && hitRect.contains(localPoint);
        }
    }

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
            result = AboutMenuResult::Back;
            break;

        default:
            break;
        }
    }

    //-----------------------------
    // Chuot - nut mui ten (thu gon/mo rong tung card)
    //-----------------------------

    if (event.type == sf::Event::MouseButtonReleased &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mp = window.mapPixelToCoords(
            { event.mouseButton.x, event.mouseButton.y });

        if (listRect.contains(mp))
        {
            // Chuyen tu toa do man hinh sang toa do LOCAL ben trong danh
            // sach cuon (cung he quy chieu voi sc.y / sc.arrowCenter) -
            // giong cach anh xa dung trong MouseWheelScrolled/draw()
            sf::Vector2f localPoint(mp.x - listRect.left, mp.y - listRect.top + scrollOffset);

            if (tryToggleCardAt(localPoint))
            {
                if (audio)
                    audio->playSound("select");
            }
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

            result = AboutMenuResult::Back;
        }
    }
}

void AboutMenu::update(float dt)
{
    (void)dt;

    backButton.update();
}

void AboutMenu::draw(sf::RenderWindow& window) const
{
    background.draw(window);

    if (panelTexture)
    {
        drawNineSlice(window, *panelTexture, GOLDENBOX_CONTENT,
            panelRect.left, panelRect.top, panelRect.width, panelRect.height);
    }

    window.draw(titleText);

    // ===== Cuon + clip 3 card bang sf::View - viewport moi duoc "chong"
    // len dung vi tri listRect trong viewport HIEN TAI cua window (co
    // the dang bi letterbox boi applyLetterboxView), giong het ky thuat
    // da dung trong LeaderboardMenu.cpp =====
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

    for (int i = 0; i < SECTION_COUNT; ++i)
    {
        const SectionCard& sc = sections[i];

        if (sectionBoxTexture)
        {
            drawThreeSlice(window, *sectionBoxTexture, SILVERBOX_CONTENT,
                0.f, sc.y, listRect.width, sc.height);
        }

        window.draw(sc.header);

        if (sc.expanded)
            window.draw(sc.body);

        if (arrowTexture)
        {
            float scale = ARROW_DRAW_W / static_cast<float>(ARROW_CONTENT.width);

            // Rê chuot qua / vua bam -> phong to nhe + doi mau, dung
            // dung bo mau cua Button de dong bo phong cach
            bool pressedFlash = sc.arrowPressClock.getElapsedTime() < ARROW_PRESS_FLASH;
            float hoverBoost = sc.arrowHovered ? ARROW_HOVER_SCALE : 1.f;

            sf::Color tint = ARROW_NORMAL_COLOR;
            if (pressedFlash)      tint = ARROW_PRESSED_COLOR;
            else if (sc.arrowHovered) tint = ARROW_HOVER_COLOR;

            sf::Sprite arrow;
            arrow.setTexture(*arrowTexture);
            arrow.setTextureRect(ARROW_CONTENT);
            arrow.setOrigin(ARROW_CONTENT.width / 2.f, ARROW_CONTENT.height / 2.f);
            arrow.setScale(scale * hoverBoost, scale * hoverBoost);
            arrow.setPosition(sc.arrowCenter);
            arrow.setColor(tint);

            // Thu gon = mui ten huong XUONG (moi de mo); mo rong = xoay
            // 180 do, huong LEN (moi de dong lai)
            if (sc.expanded)
                arrow.setRotation(180.f);

            window.draw(arrow);
        }
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

    backButton.draw(window);
}

//==================================================
// Result
//==================================================

AboutMenuResult AboutMenu::getResult() const
{
    return result;
}

void AboutMenu::clearResult()
{
    result = AboutMenuResult::None;
}
