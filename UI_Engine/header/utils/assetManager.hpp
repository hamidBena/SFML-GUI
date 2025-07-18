#pragma once

#include "env.hpp"

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include <filesystem>

namespace fs = std::filesystem;

class AssetManager {
public:
    static AssetManager& get();

    sf::Texture& getTexture(const std::string& filename);
    sf::Font& getFont(const std::string& filename);

    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;

private:
    AssetManager() = default;

    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> textures;
    std::unordered_map<std::string, std::unique_ptr<sf::Font>> fonts;

    static fs::path asset_dir;
};
