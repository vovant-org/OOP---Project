#pragma once

#include <SFML/Graphics.hpp>

#include <string>
#include <unordered_map>

class ResourceManager
{
private:

    //--------------------------------------------------
    // Resources
    //--------------------------------------------------

    std::unordered_map<std::string, sf::Texture> textures;
    std::unordered_map<std::string, sf::Font> fonts;

public:

    ResourceManager() = default;
    ~ResourceManager() = default;

    //--------------------------------------------------
    // Texture
    //--------------------------------------------------

    bool loadTexture(const std::string& id,
        const std::string& filename);

    bool hasTexture(const std::string& id) const;

    sf::Texture& getTexture(const std::string& id);

    const sf::Texture& getTexture(const std::string& id) const;

    //--------------------------------------------------
    // Font
    //--------------------------------------------------

    bool loadFont(const std::string& id,
        const std::string& filename);

    bool hasFont(const std::string& id) const;

    sf::Font& getFont(const std::string& id);

    const sf::Font& getFont(const std::string& id) const;

    //--------------------------------------------------
    // Utility
    //--------------------------------------------------

    void clear();
};