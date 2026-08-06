// CharacterSelection.cpp
#include "CharacterSelection.h"
#include "AudioManager.h"
#include <iostream>

// ===== ADDED =====
void CharacterSelection::setAudioManager(AudioManager* manager)
{
    audio = manager;
}

//==================================================
// Helper: scale sprite theo kích thước mong muốn (px) thay vì
// dùng hệ số scale cố định — tránh icon bị to/lệch khi đổi ảnh
//==================================================
static void fitSpriteToSize(sf::Sprite& sp, float targetW, float targetH, bool flipX = false)
{
    sf::Vector2u sz = sp.getTexture()->getSize();
    if (sz.x == 0 || sz.y == 0) return;

    float sx = targetW / (float)sz.x;
    float sy = targetH / (float)sz.y;
    sp.setScale(flipX ? -sx : sx, sy);
}

//==================================================
// Constructor
//==================================================

CharacterSelection::CharacterSelection()
{
    // Sẽ được setup đầy đủ sau khi gọi setupLayout()
}

//==================================================
// Setup
//==================================================

void CharacterSelection::setWindowSize(float w, float h)
{
    W = w;
    H = h;
}

void CharacterSelection::setBackgroundTexture(const sf::Texture& tex,
    float sx, float sy)
{
    background.setTexture(tex);
    background.setScale(sx, sy);
}

bool CharacterSelection::loadFont(const std::string& path)
{
    if (!font.loadFromFile(path))
    {
        std::cout << "[CharacterSelection] Cannot load font: " << path << "\n";
        return false;
    }

    titleText.setFont(font);
    charNameText.setFont(font);
    statSpeedText.setFont(font);
    statHPText.setFont(font);
    statSkillText.setFont(font);
    hintText.setFont(font);

    return true;
}

bool CharacterSelection::loadCharacterTexture(int index, const std::string& path)
{
    if (index < 0 || index >= CHARACTER_COUNT) return false;

    if (!charTextures[index].loadFromFile(path))
    {
        std::cout << "[CharacterSelection] Cannot load character " << index
            << ": " << path << "\n";
        return false;
    }

    charTextures[index].setSmooth(false);
    charSprites[index].setTexture(charTextures[index]);
    return true;
}

bool CharacterSelection::loadUITextures(
    const std::string& arrow,
    const std::string& heart,
    const std::string& lightning)
{
    bool ok = true;

    ok &= leftArrowTexture.loadFromFile(arrow);
    ok &= heartTexture.loadFromFile(heart);
    ok &= lightningTexture.loadFromFile(lightning);

    leftArrowTexture.setSmooth(true);
    heartTexture.setSmooth(true);
    lightningTexture.setSmooth(true);

    return ok;
}

void CharacterSelection::setupButtons(const sf::Texture& buttonTex,
    float btnW, float btnH,
    float scaleX, float scaleY)
{
    // Nút < >  đặt 2 bên preview
    // Scale theo kích thước px mong muốn (56x56) thay vì hệ số cố định,
    // để không phụ thuộc độ phân giải ảnh mũi tên gốc
    const float ARROW_SIZE = 56.f;
    {
        sf::Vector2u arrSz = leftArrowTexture.getSize();
        if (arrSz.x > 0 && arrSz.y > 0)
        {
            float asx = ARROW_SIZE / (float)arrSz.x;
            float asy = ARROW_SIZE / (float)arrSz.y;

            prevButton.setTexture(leftArrowTexture);
            prevButton.setScale(asx, asy);

            nextButton.setTexture(leftArrowTexture);
            nextButton.setScale(-asx, asy);
        }
    }

    selectButton.setTexture(buttonTex);
    backButton.setTexture(buttonTex);

    // Nút SELECT
    selectButton.setTexture(buttonTex);
    selectButton.setScale(scaleX, scaleY);

    // Nút BACK
    backButton.setTexture(buttonTex);
    backButton.setScale(scaleX, scaleY);
}

void CharacterSelection::setupLayout()
{
    float cx = W / 2.f;

    //--------------------------------------------------
    // Preview box (giữa màn hình, chiếm ~30% chiều rộng)
    //--------------------------------------------------
    float pvW = W * 0.40f;
    float pvH = H * 0.42f;      // giảm bớt để chừa chỗ cho info box + nút bên dưới
    float pvX = cx - pvW / 2.f;
    float pvY = H * 0.13f;

    previewBox.setSize({ pvW, pvH });
    previewBox.setPosition(pvX, pvY);
    previewBox.setFillColor(sf::Color(10, 15, 30, 200));

    previewBorder.setSize({ pvW + 6.f, pvH + 6.f });
    previewBorder.setPosition(pvX - 3.f, pvY - 3.f);
    previewBorder.setFillColor(sf::Color::Transparent);
    previewBorder.setOutlineColor(sf::Color(255, 220, 80, 200));
    previewBorder.setOutlineThickness(3.f);

    //--------------------------------------------------
    // Info box (bên dưới preview)
    //--------------------------------------------------
    float infoH = H * 0.17f;
    infoBox.setSize({ pvW, infoH });
    infoBox.setPosition(pvX, pvY + pvH + 10.f);
    infoBox.setFillColor(sf::Color(10, 15, 30, 180));
    // outline nhẹ
    infoBox.setOutlineColor(sf::Color(100, 100, 120, 150));
    infoBox.setOutlineThickness(1.f);

    //--------------------------------------------------
    // Title
    //--------------------------------------------------
    titleText.setString("SELECT CHARACTER");
    titleText.setCharacterSize(44);
    titleText.setFillColor(sf::Color(255, 220, 80));
    titleText.setStyle(sf::Text::Bold);
    centerText(titleText, cx, H * 0.04f);

    //--------------------------------------------------
    // Nút < prev — bên trái preview (canh giữa theo chiều cao preview)
    //--------------------------------------------------
    const float arrowHalf = 28.f; // ARROW_SIZE / 2
    prevButton.setText("");
    prevButton.setPosition(pvX - 50.f, pvY + pvH / 2.f - arrowHalf);

    //--------------------------------------------------
    // Nút > next — bên phải preview
    //--------------------------------------------------
    nextButton.setText("");
    nextButton.setPosition(pvX + pvW + 50.f, pvY + pvH / 2.f - arrowHalf);

    //--------------------------------------------------
    // Dot indicators — ngay dưới info box
    //--------------------------------------------------
    float infoBottom = pvY + pvH + 10.f + infoH;
    float dotY = infoBottom + 18.f;
    float dotSpacing = 28.f;
    float dotStartX = cx - (CHARACTER_COUNT - 1) * dotSpacing / 2.f;

    for (int i = 0; i < CHARACTER_COUNT; i++)
    {
        dots[i].setRadius(7.f);
        dots[i].setOrigin(7.f, 7.f);
        dots[i].setPosition(dotStartX + i * dotSpacing, dotY);
    }

    //--------------------------------------------------
    // Nút SELECT / BACK — đặt dưới dots, đủ chỗ trước hint
    //--------------------------------------------------
    float buttonY = H * 0.80f;

    selectButton.setText("SELECT");
    selectButton.setFont(font);
    selectButton.setCharacterSize(24);
    selectButton.setPosition(cx - 250.f, buttonY);

    backButton.setText("BACK");
    backButton.setFont(font);
    backButton.setCharacterSize(24);
    backButton.setPosition(cx + 10.f, buttonY);

    //--------------------------------------------------
    // Hint
    //--------------------------------------------------
    hintText.setString("LEFT / RIGHT  |  ENTER = Select  |  ESC = Back");
    hintText.setCharacterSize(18);
    hintText.setFillColor(sf::Color(120, 120, 140, 200));
    centerText(hintText, cx, H * 0.955f);

    //--------------------------------------------------
    // Cập nhật nội dung lần đầu
    //--------------------------------------------------
    updatePreview();
    updateDots();
    updateStatsText();
}

//==================================================
// Result
//==================================================

CharacterSelectionResult CharacterSelection::getResult() const
{
    return result;
}

void CharacterSelection::clearResult()
{
    result = CharacterSelectionResult::None;
}

int CharacterSelection::getSelectedIndex() const
{
    return selectedIndex;
}

//==================================================
// Navigation
//==================================================

void CharacterSelection::selectPrev()
{
    selectedIndex = (selectedIndex - 1 + CHARACTER_COUNT) % CHARACTER_COUNT;
    updatePreview();
    updateDots();
    updateStatsText();
}

void CharacterSelection::selectNext()
{
    selectedIndex = (selectedIndex + 1) % CHARACTER_COUNT;
    updatePreview();
    updateDots();
    updateStatsText();
}

//==================================================
// Update preview sprite
//==================================================

void CharacterSelection::updatePreview()
{
    previewFrame = 0;
    frameTimer = 0.f;

    auto& tex = charTextures[selectedIndex];
    auto& sp = charSprites[selectedIndex];

    if (tex.getSize().x == 0) return;

    // Sprite sheet: 4 cols x 5 rows
    // Row 0=down(front), 1=left, 2=right, 3=up(back), 4=die
    // ===== CHANGED: dung row 1 de preview (thay vi row 0) =====
    frameW = tex.getSize().x / 4;
    frameH = tex.getSize().y / 5;

    sp.setTextureRect(sf::IntRect(0, frameH, frameW, frameH));
    sp.setOrigin((float)frameW / 2.f, (float)frameH / 2.f);

    // Scale vừa 60% preview box
    float pvW = previewBox.getSize().x;
    float pvH = previewBox.getSize().y;
    float maxW = pvW * 0.60f;
    float maxH = pvH * 0.70f;
    float scale = std::min(maxW / frameW, maxH / frameH);
    sp.setScale(scale, scale);

    // Căn giữa preview box
    sf::FloatRect pb = previewBox.getGlobalBounds();
    sp.setPosition(pb.left + pb.width / 2.f,
        pb.top + pb.height / 2.f);
}

//==================================================
// Update dot indicators
//==================================================

void CharacterSelection::updateDots()
{
    for (int i = 0; i < CHARACTER_COUNT; i++)
    {
        if (i == selectedIndex)
        {
            dots[i].setRadius(9.f);
            dots[i].setOrigin(9.f, 9.f);
            dots[i].setFillColor(sf::Color(255, 220, 80));
        }
        else
        {
            dots[i].setRadius(6.f);
            dots[i].setOrigin(6.f, 6.f);
            dots[i].setFillColor(sf::Color(80, 80, 100));
        }
    }
}

//==================================================
// Update stat texts
//==================================================

void CharacterSelection::updateStatsText()
{
    const auto& c = charInfos[selectedIndex];
    float       boxX = infoBox.getPosition().x;
    float       boxW = infoBox.getSize().x;
    float       lx = boxX + 20.f;
    float       ty = infoBox.getPosition().y + 14.f;
    float       dy = 36.f;

    // Tên nhân vật (chỉ tên thôi — theo yêu cầu)
    charNameText.setString(c.name);
    charNameText.setCharacterSize(32);
    charNameText.setFillColor(sf::Color(255, 220, 80));
    charNameText.setStyle(sf::Text::Bold);
    centerText(charNameText,
        boxX + boxW / 2.f,
        infoBox.getPosition().y + 10.f);

    statSpeedText.setString("Speed");
    statSpeedText.setCharacterSize(18);
    statSpeedText.setFillColor(sf::Color(200, 220, 255));
    statSpeedText.setPosition(lx, ty + dy * 1.f);

    statHPText.setString("HP");
    statHPText.setCharacterSize(18);
    statHPText.setFillColor(sf::Color(200, 220, 255));
    statHPText.setPosition(lx, ty + dy * 2.f);


}

//==================================================
// processEvent
//==================================================

void CharacterSelection::processEvent(const sf::Event& event,
    const sf::RenderWindow& window)
{
    // ── Keyboard ──────────────────────────────────
    if (event.type == sf::Event::KeyPressed)
    {
        switch (event.key.code)
        {
        case sf::Keyboard::Left:
        case sf::Keyboard::A:
            selectPrev();
            break;

        case sf::Keyboard::Right:
        case sf::Keyboard::D:
            selectNext();
            break;

        case sf::Keyboard::Return:
            selectButton.press();
            if (audio) audio->playSound("select");
            result = CharacterSelectionResult::Selected;
            break;

        case sf::Keyboard::Escape:
            backButton.press();
            if (audio) audio->playSound("select");
            result = CharacterSelectionResult::Back;
            break;

        default:
            break;
        }
    }

    // ── Mouse ─────────────────────────────────────
    prevButton.processEvent(event, window);
    nextButton.processEvent(event, window);
    selectButton.processEvent(event, window);
    backButton.processEvent(event, window);

    if (event.type == sf::Event::MouseButtonReleased &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mp = window.mapPixelToCoords(
            sf::Mouse::getPosition(window));

        if (prevButton.contains(mp))   selectPrev();
        if (nextButton.contains(mp))   selectNext();
        if (selectButton.contains(mp)) {
            if (audio) audio->playSound("select");
            result = CharacterSelectionResult::Selected;
        }
        if (backButton.contains(mp))
        {
            if (audio) audio->playSound("select");
            result = CharacterSelectionResult::Back;
        }
    }
}

//==================================================
// update (no deltaTime — required by Menu interface)
//==================================================

void CharacterSelection::update(float dt)
{
    prevButton.update();
    nextButton.update();
    selectButton.update();
    backButton.update();

    // Animate preview
    if (frameW <= 0) return;

    frameTimer += dt;
    if (frameTimer >= frameDuration)
    {
        frameTimer = 0.f;
        previewFrame = (previewFrame + 1) % 4;

        auto& sp = charSprites[selectedIndex];
        sp.setTextureRect(sf::IntRect(
            previewFrame * frameW, frameH,   // ===== CHANGED: row 1 =====
            frameW, frameH));
    }
}

//==================================================
// draw
//==================================================

void CharacterSelection::drawIcons(sf::RenderWindow& window) const
{
    sf::Sprite heart(heartTexture);
    sf::Sprite lightning(lightningTexture);

    // Kích thước icon cố định theo px, không phụ thuộc ảnh gốc
    const float ICON_SIZE = 22.f;
    const float ICON_SPACING = ICON_SIZE + 6.f;

    fitSpriteToSize(heart, ICON_SIZE, ICON_SIZE);
    fitSpriteToSize(lightning, ICON_SIZE, ICON_SIZE);

    float startX = infoBox.getPosition().x + 130.f;

    float speedY = infoBox.getPosition().y + 50.f;
    float hpY = infoBox.getPosition().y + 86.f;

    for (int i = 0;i < charInfos[selectedIndex].speed;i++)
    {
        lightning.setPosition(startX + i * ICON_SPACING, speedY);
        window.draw(lightning);
    }

    for (int i = 0;i < charInfos[selectedIndex].hp;i++)
    {
        heart.setPosition(startX + i * ICON_SPACING, hpY);
        window.draw(heart);
    }
}

void CharacterSelection::draw(sf::RenderWindow& window) const
{
    // Background
    background.draw(window);

    // Overlay tối nhẹ
    sf::RectangleShape overlay;
    overlay.setSize({ W, H });
    overlay.setFillColor(sf::Color(0, 0, 0, 100));
    window.draw(overlay);

    // Title
    window.draw(titleText);

    // Preview frame
    window.draw(previewBorder);
    window.draw(previewBox);

    // Character sprite
    const auto& sp = charSprites[selectedIndex];
    if (charTextures[selectedIndex].getSize().x > 0)
        window.draw(sp);

    // Info box
    window.draw(infoBox);
    window.draw(charNameText);
    window.draw(statSpeedText);
    window.draw(statHPText);

    drawIcons(window);

    window.draw(statSkillText);

    // Buttons
    prevButton.draw(window);
    nextButton.draw(window);
    selectButton.draw(window);
    backButton.draw(window);

    // Dot indicators
    for (const auto& dot : dots)
        window.draw(dot);

    // Hint
    window.draw(hintText);
}

//==================================================
// Private helpers
//==================================================

void CharacterSelection::centerText(sf::Text& t, float cx, float y)
{
    sf::FloatRect b = t.getLocalBounds();
    t.setOrigin(b.left + b.width / 2.f, b.top);
    t.setPosition(cx, y);
}