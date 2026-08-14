#include "Button.h"

Button::Button()
{
    normalColor = sf::Color::White;
    hoverColor = sf::Color(255, 230, 120);
    pressedColor = sf::Color(255, 180, 40);

    text.setFillColor(normalColor);
}

void Button::processEvent(const sf::Event& event,
    const sf::Vector2f& mousePos)
{
    if (!enabled)
        return;

    if (pressAnimating)
    {
        if (pressClock.getElapsedTime() >= pressDuration)
        {
            pressed = false;
            pressAnimating = false;
        }
    }

    hovered = contains(mousePos);

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        pressed = hovered;
    }

    if (event.type == sf::Event::MouseButtonReleased &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        pressed = false;
    }
}

//==================================================
// Private
//==================================================

void Button::centerText()
{
    sf::FloatRect spriteBounds = sprite.getGlobalBounds();
    sf::FloatRect textBounds = text.getLocalBounds();

    text.setPosition(
        spriteBounds.left + (spriteBounds.width - textBounds.width) / 2.f - textBounds.left,
        spriteBounds.top + (spriteBounds.height - textBounds.height) / 2.f - textBounds.top
    );
}

// ===== ADDED: chup lai bounds NGAY LUC NAY (dang o normalScale, chua bi
// hover/press phong to) - dung rieng cho hit-test, khong lien quan gi
// den animation ve sau =====
void Button::updateBaseBounds()
{
    baseBounds = sprite.getGlobalBounds();
}

//==================================================
// Texture
//==================================================

void Button::setTexture(const sf::Texture& texture)
{
    sprite.setTexture(texture);
    centerText();
    updateBaseBounds();   // ===== ADDED =====
}

//==================================================
// Transform
//==================================================

void Button::setPosition(float x, float y)
{
    sprite.setPosition(x, y);
    centerText();
    updateBaseBounds();   // ===== ADDED =====
}

void Button::setScale(float scaleX, float scaleY)
{
    normalScale = { scaleX, scaleY };

    sprite.setScale(scaleX, scaleY);

    centerText();
    updateBaseBounds();   // ===== ADDED =====
}

//==================================================
// Text
//==================================================

void Button::setText(const std::string& str)
{
    text.setString(str);
    centerText();
}

void Button::setFont(const sf::Font& font)
{
    text.setFont(font);
    centerText();
}

void Button::setCharacterSize(unsigned int size)
{
    text.setCharacterSize(size);
    centerText();
}

//==================================================
// Color
//==================================================

void Button::setNormalColor(const sf::Color& color)
{
    normalColor = color;
}

void Button::setHoverColor(const sf::Color& color)
{
    hoverColor = color;
}

void Button::setPressedColor(const sf::Color& color)
{
    pressedColor = color;
}

//==================================================
// State
//==================================================

void Button::setEnabled(bool enable)
{
    enabled = enable;
}

bool Button::isEnabled() const
{
    return enabled;
}

void Button::setFocused(bool focus)
{
    focused = focus;
}

bool Button::isFocused() const
{
    return focused;
}

bool Button::contains(sf::Vector2f mousePos) const
{
    // ===== CHANGED: dung baseBounds (co dinh, khong bi hover/press phong
    // to) thay vi sprite.getGlobalBounds() (thay doi lien tuc theo
    // animation) - vung click/hover gio luon dung bang kich thuoc that
    // cua button luc binh thuong, khong con lan sang nut ke ben khi hover =====
    return baseBounds.contains(mousePos);
}

bool Button::isPressed() const
{
    return pressed;
}

void Button::press()
{
    if (!enabled)
        return;

    pressed = true;
    pressAnimating = true;
    pressClock.restart();
}

//==================================================
// Update
//==================================================

void Button::processEvent(const sf::Event& event,
    const sf::RenderWindow& window)
{
    if (!enabled)
        return;

    if (pressAnimating)
    {
        if (pressClock.getElapsedTime() >= pressDuration)
        {
            pressed = false;
            pressAnimating = false;
        }
    }

    sf::Vector2f mousePos =
        window.mapPixelToCoords(sf::Mouse::getPosition(window));

    hovered = contains(mousePos);

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        pressed = hovered;
    }

    if (event.type == sf::Event::MouseButtonReleased &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        pressed = false;
    }
}

void Button::update()
{
    if (!enabled)
    {
        sprite.setColor(sf::Color(180, 180, 180));

        sprite.setScale(
            normalScale.x,
            normalScale.y);

        text.setFillColor(sf::Color(150, 150, 150));

        centerText();
        return;
    }

    if (pressed)
    {
        sprite.setColor(pressedColor);

        sprite.setScale(
            normalScale.x * 0.97f,
            normalScale.y * 0.97f);

        text.setFillColor(pressedColor);
    }
    else if (hovered || focused)
    {
        sprite.setColor(hoverColor);

        sprite.setScale(
            normalScale.x * hoverScale,
            normalScale.y * hoverScale);

        text.setFillColor(hoverColor);
    }
    else
    {
        sprite.setColor(normalColor);

        sprite.setScale(
            normalScale.x,
            normalScale.y);

        text.setFillColor(normalColor);
    }

    centerText();
}

//==================================================
// Render
//==================================================

void Button::draw(sf::RenderWindow& window) const
{
    window.draw(sprite);
    window.draw(text);
}