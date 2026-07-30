// MenuManager.cpp
#include "MenuManager.h"

//==================================================
// Setup
//==================================================

void MenuManager::setApplyViewFn(ApplyViewFn fn)
{
    applyViewFn = std::move(fn);
}

void MenuManager::registerMenu(AppState state, Menu* menu)
{
    menus[state] = menu;
}

//==================================================
// State
//==================================================

void MenuManager::setState(AppState newState)
{
    currentState = newState;

    if (applyViewFn)
        applyViewFn(currentState);
}

AppState MenuManager::getState() const
{
    return currentState;
}

void MenuManager::reapplyView() const
{
    if (applyViewFn)
        applyViewFn(currentState);
}

Menu* MenuManager::getCurrentMenu() const
{
    auto it = menus.find(currentState);

    if (it == menus.end())
        return nullptr;

    return it->second;
}

//==================================================
// Forward
//==================================================

void MenuManager::processEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    if (Menu* m = getCurrentMenu())
        m->processEvent(event, window);
}

void MenuManager::update(float dt)
{
    if (Menu* m = getCurrentMenu())
        m->update(dt);
}

void MenuManager::draw(sf::RenderWindow& window) const
{
    if (Menu* m = getCurrentMenu())
        m->draw(window);
}