#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <memory>

class AssetManager {
public:
    static AssetManager& get();

    // Load or retrieve texture
    sf::Texture& getTexture(const std::string& filename);

    // Load or retrieve font
    sf::Font& getFont(const std::string& filename);

    // Prevent copying
    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;

private:
    AssetManager() = default;

    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> textures;
    std::unordered_map<std::string, std::unique_ptr<sf::Font>> fonts;
};
