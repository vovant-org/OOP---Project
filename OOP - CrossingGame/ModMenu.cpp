// ModMenu.cpp
#include "ModMenu.h"
#include "ModManager.h"
#include "AudioManager.h"

#include <algorithm>
#include <iostream>

//==================================================
// Constructor
//==================================================

ModMenu::ModMenu()
{
}

//==================================================
// Setup
//==================================================

void ModMenu::setAudioManager(AudioManager* manager)
{
    audio = manager;
}

void ModMenu::setModManager(ModManager* manager)
{
    modManager = manager;
}

void ModMenu::setWindowSize(float w, float h)
{
    W = w;
    H = h;
}

void ModMenu::setBackgroundTexture(const sf::Texture& tex, float sx, float sy)
{
    background.setTexture(tex);
    background.setScale(sx, sy);
}

bool ModMenu::loadFont(const std::string& path)
{
    if (!font.loadFromFile(path))
    {
        std::cout << "[ModMenu] Cannot load font: " << path << "\n";
        return false;
    }

    hasFont = true;

    titleText.setFont(font);
    hintText.setFont(font);
    statusText.setFont(font);
    charNameText.setFont(font);   // ===== ADDED =====

    return true;
}

// ===== ADDED: nap texture mui ten < > =====
bool ModMenu::loadArrowTexture(const std::string& path)
{
    if (!leftArrowTexture.loadFromFile(path))
    {
        std::cout << "[ModMenu] Cannot load arrow: " << path << "\n";
        return false;
    }

    leftArrowTexture.setSmooth(true);
    return true;
}

void ModMenu::setupButtons(const sf::Texture& buttonTex, float scaleX, float scaleY)
{
    importButton.setTexture(buttonTex);
    importButton.setScale(scaleX, scaleY);

    resetButton.setTexture(buttonTex);
    resetButton.setScale(scaleX, scaleY);

    backButton.setTexture(buttonTex);
    backButton.setScale(scaleX, scaleY);

    if (hasFont)
    {
        importButton.setFont(font);
        resetButton.setFont(font);
        backButton.setFont(font);
    }

    importButton.setText("IMPORT SKIN");
    resetButton.setText("RESET SKIN");
    backButton.setText("BACK");

    importButton.setCharacterSize(22);
    resetButton.setCharacterSize(22);
    backButton.setCharacterSize(24);

    // ===== ADDED: nut < > chuyen nhan vat dang duoc MOD - cung kieu
    // voi CharacterSelection/MapSelection (mui tien trai, nextButton
    // dung lai scale.x am de lat thanh mui ten phai) =====
    const float ARROW_SIZE = 56.f;
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
    prevButton.setText("");
    nextButton.setText("");
}

void ModMenu::setupLayout()
{
    float cx = W / 2.f;

    //--------------------------------------------------
    // Preview box - ben trai
    //--------------------------------------------------
    float pvW = 260.f;
    float pvH = 260.f;
    float pvX = cx - 340.f;
    float pvY = H * 0.28f;

    previewBox.setSize({ pvW, pvH });
    previewBox.setPosition(pvX, pvY);
    previewBox.setFillColor(sf::Color(10, 15, 30, 200));

    previewBorder.setSize({ pvW + 6.f, pvH + 6.f });
    previewBorder.setPosition(pvX - 3.f, pvY - 3.f);
    previewBorder.setFillColor(sf::Color::Transparent);
    previewBorder.setOutlineColor(sf::Color(255, 220, 80, 200));
    previewBorder.setOutlineThickness(3.f);

    if (hasFont)
    {
        titleText.setString("MOD - CUSTOM SKIN");
        titleText.setCharacterSize(40);
        titleText.setStyle(sf::Text::Bold);
        titleText.setFillColor(sf::Color(255, 220, 80));
        centerText(titleText, cx, H * 0.08f);

        hintText.setString(
            "Chon anh de doi skin nhan vat dang chon\n"
            "Tu dong can chinh & lat huong khi di chuyen\n"
            "< > / TRAI-PHAI: doi nhan vat");
        hintText.setCharacterSize(18);
        hintText.setStyle(sf::Text::Bold);
        hintText.setFillColor(sf::Color(255, 220, 80));
        hintText.setLineSpacing(1.3f);
        hintText.setPosition(cx - 20.f, pvY);

        statusText.setCharacterSize(16);
        statusText.setFillColor(sf::Color(140, 220, 140));
        statusText.setPosition(pvX, pvY + pvH + 16.f);

        // ===== ADDED: ten nhan vat dang duoc chon, ngay tren preview =====
        charNameText.setCharacterSize(24);
        charNameText.setStyle(sf::Text::Bold);
        charNameText.setFillColor(sf::Color(255, 220, 80));
    }

    //--------------------------------------------------
    // Nut < prev / > next - 2 ben preview, doi nhan vat dang MOD
    //--------------------------------------------------
    const float arrowHalf = 28.f; // ARROW_SIZE / 2
    prevButton.setPosition(pvX - 50.f, pvY + pvH / 2.f - arrowHalf);
    nextButton.setPosition(pvX + pvW + 50.f, pvY + pvH / 2.f - arrowHalf);

    //--------------------------------------------------
    // Buttons - ===== CHANGED: ha xuong (0.40 -> 0.50) de khong con bi
    // hintText (da rut gon con 3 dong) de len =====
    //--------------------------------------------------
    float buttonX = cx - 20.f;
    float buttonStartY = H * 0.50f;
    float spacing = 62.f;

    importButton.setPosition(buttonX, buttonStartY);
    resetButton.setPosition(buttonX, buttonStartY + spacing);
    backButton.setPosition(pvX, H * 0.82f);

    updateCharNameText();
    reloadPreview();
}

void ModMenu::centerText(sf::Text& t, float cx, float y)
{
    sf::FloatRect b = t.getLocalBounds();
    t.setOrigin(b.left + b.width / 2.f, b.top);
    t.setPosition(cx, y);
}

//==================================================
// Preview
//==================================================

void ModMenu::reloadPreview()
{
    // Luon doc LAI tu dia - de preview phan anh DUNG skin dang duoc
    // game su dung cho nhan vat DANG DUOC CHON (ke ca sau khi
    // Import/Reset/doi nhan vat).
    if (!previewTexture.loadFromFile(modCharacters[selectedCharIndex].path))
        return;

    previewTexture.setSmooth(false);
    previewSprite.setTexture(previewTexture);

    previewFrameW = previewTexture.getSize().x / 4;
    previewFrameH = previewTexture.getSize().y / 5;

    if (previewFrameW <= 0 || previewFrameH <= 0)
        return;

    // Frame dau tien cua hang "quay xuong" (row 0) lam preview
    previewSprite.setTextureRect(sf::IntRect(0, 0, previewFrameW, previewFrameH));
    previewSprite.setOrigin(previewFrameW / 2.f, previewFrameH / 2.f);

    float pvW = previewBox.getSize().x;
    float pvH = previewBox.getSize().y;
    float maxW = pvW * 0.70f;
    float maxH = pvH * 0.70f;
    float scale = std::min(maxW / previewFrameW, maxH / previewFrameH);
    previewSprite.setScale(scale, scale);

    sf::FloatRect pb = previewBox.getGlobalBounds();
    previewSprite.setPosition(pb.left + pb.width / 2.f, pb.top + pb.height / 2.f);
}

// ===== ADDED: cap nhat text ten nhan vat dang chon, canh giua phia
// tren preview box =====
void ModMenu::updateCharNameText()
{
    if (!hasFont) return;

    charNameText.setString(modCharacters[selectedCharIndex].name);

    sf::FloatRect pb = previewBox.getGlobalBounds();
    centerText(charNameText, pb.left + pb.width / 2.f, pb.top - 34.f);
}

// ===== ADDED: doi sang nhan vat truoc/sau (vong lai dau-cuoi), giong
// CharacterSelection::selectPrev/selectNext - chuyen ModManager sang
// muc tieu moi va nap lai preview =====
void ModMenu::selectPrev()
{
    selectedCharIndex = (selectedCharIndex - 1 + MOD_CHARACTER_COUNT) % MOD_CHARACTER_COUNT;

    if (modManager)
        modManager->SetTarget(modCharacters[selectedCharIndex].path);

    statusText.setString("");
    updateCharNameText();
    reloadPreview();
}

void ModMenu::selectNext()
{
    selectedCharIndex = (selectedCharIndex + 1) % MOD_CHARACTER_COUNT;

    if (modManager)
        modManager->SetTarget(modCharacters[selectedCharIndex].path);

    statusText.setString("");
    updateCharNameText();
    reloadPreview();
}

void ModMenu::refresh()
{
    if (modManager)
        modManager->SetTarget(modCharacters[selectedCharIndex].path);

    reloadPreview();
    updateCharNameText();
    statusText.setString("");
}

//==================================================
// Menu
//==================================================

void ModMenu::processEvent(const sf::Event& event,
    const sf::RenderWindow& window)
{
    importButton.processEvent(event, window);
    resetButton.processEvent(event, window);
    backButton.processEvent(event, window);
    prevButton.processEvent(event, window);   // ===== ADDED =====
    nextButton.processEvent(event, window);   // ===== ADDED =====

    if (event.type == sf::Event::KeyPressed)
    {
        switch (event.key.code)
        {
        // ===== ADDED: doi nhan vat bang ban phim, giong CharacterSelection =====
        case sf::Keyboard::Left:
        case sf::Keyboard::A:
            selectPrev();
            break;

        case sf::Keyboard::Right:
        case sf::Keyboard::D:
            selectNext();
            break;

        case sf::Keyboard::Escape:
            backButton.press();
            if (audio) audio->playSound("select");
            result = ModMenuResult::Back;
            break;

        default:
            break;
        }
    }

    if (event.type == sf::Event::MouseButtonReleased &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mp = window.mapPixelToCoords(
            { event.mouseButton.x, event.mouseButton.y });

        if (prevButton.contains(mp))   // ===== ADDED =====
        {
            if (audio) audio->playSound("select");
            selectPrev();
        }
        else if (nextButton.contains(mp))   // ===== ADDED =====
        {
            if (audio) audio->playSound("select");
            selectNext();
        }
        else if (importButton.contains(mp))
        {
            importButton.press();
            if (audio) audio->playSound("select");

            if (modManager)
            {
                // Mo hop thoai chon file CUA WINDOWS - nguoi choi tu do
                // duyet toi bat ky anh nao tren may (vd MemeCat.png)
                std::string chosen = ModManager::OpenImageDialog();

                if (!chosen.empty())
                {
                    std::string msg;
                    bool ok = modManager->ApplySkin(chosen, msg);

                    // ===== ADDED: ghi ro ten nhan vat vua duoc doi skin =====
                    if (ok) msg += "  (" + modCharacters[selectedCharIndex].name + ")";

                    statusText.setFillColor(ok ? sf::Color(140, 220, 140)
                        : sf::Color(230, 100, 100));
                    statusText.setString(msg);

                    if (ok)
                        reloadPreview();
                }
            }
        }
        else if (resetButton.contains(mp))
        {
            resetButton.press();
            if (audio) audio->playSound("select");

            if (modManager)
            {
                std::string msg;
                bool ok = modManager->ResetSkin(msg);

                if (ok) msg += "  (" + modCharacters[selectedCharIndex].name + ")";

                statusText.setFillColor(ok ? sf::Color(140, 220, 140)
                    : sf::Color(230, 100, 100));
                statusText.setString(msg);

                if (ok)
                    reloadPreview();
            }
        }
        else if (backButton.contains(mp))
        {
            backButton.press();
            if (audio) audio->playSound("select");
            result = ModMenuResult::Back;
        }
    }
}

void ModMenu::update(float dt)
{
    (void)dt;

    importButton.update();
    resetButton.update();
    backButton.update();
    prevButton.update();   // ===== ADDED =====
    nextButton.update();   // ===== ADDED =====
}

void ModMenu::draw(sf::RenderWindow& window) const
{
    background.draw(window);

    sf::RectangleShape overlay;
    overlay.setSize({ W, H });
    overlay.setFillColor(sf::Color(0, 0, 0, 100));
    window.draw(overlay);

    if (hasFont)
    {
        window.draw(titleText);
        window.draw(hintText);
        window.draw(statusText);
        window.draw(charNameText);   // ===== ADDED =====
    }

    window.draw(previewBorder);
    window.draw(previewBox);

    if (previewTexture.getSize().x > 0)
        window.draw(previewSprite);

    importButton.draw(window);
    resetButton.draw(window);
    backButton.draw(window);
    prevButton.draw(window);   // ===== ADDED =====
    nextButton.draw(window);   // ===== ADDED =====
}

//==================================================
// Result
//==================================================

ModMenuResult ModMenu::getResult() const
{
    return result;
}

void ModMenu::clearResult()
{
    result = ModMenuResult::None;
}
