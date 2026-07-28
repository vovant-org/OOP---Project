#include "ResourceManager.h"

#include <stdexcept>

//==================================================
// Texture
//==================================================

bool ResourceManager::loadTexture(
    const std::string& id,
    const std::string& filename)
{
    sf::Texture texture;

    if (!texture.loadFromFile(filename))
        return false;

    textures[id] = std::move(texture);

    return true;
}

bool ResourceManager::hasTexture(const std::string& id) const
{
    return textures.find(id) != textures.end();
}

sf::Texture& ResourceManager::getTexture(const std::string& id)
{
    auto it = textures.find(id);

    if (it == textures.end())
        throw std::runtime_error("Texture not found: " + id);

    return it->second;
}

const sf::Texture& ResourceManager::getTexture(const std::string& id) const
{
    auto it = textures.find(id);

    if (it == textures.end())
        throw std::runtime_error("Texture not found: " + id);

    return it->second;
}

//==================================================
// Font
//==================================================

bool ResourceManager::loadFont(
    const std::string& id,
    const std::string& filename)
{
    sf::Font font;

    if (!font.loadFromFile(filename))
        return false;

    fonts[id] = std::move(font);

    return true;
}

bool ResourceManager::hasFont(const std::string& id) const
{
    return fonts.find(id) != fonts.end();
}

sf::Font& ResourceManager::getFont(const std::string& id)
{
    auto it = fonts.find(id);

    if (it == fonts.end())
        throw std::runtime_error("Font not found: " + id);

    return it->second;
}

const sf::Font& ResourceManager::getFont(const std::string& id) const
{
    auto it = fonts.find(id);

    if (it == fonts.end())
        throw std::runtime_error("Font not found: " + id);

    return it->second;
}

//==================================================
// Utility
//==================================================

void ResourceManager::clear()
{
    textures.clear();
    fonts.clear();
}