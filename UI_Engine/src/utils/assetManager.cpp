#include "utils/assetManager.hpp"
#include <stdexcept>

AssetManager& AssetManager::get() {
    static AssetManager instance;
    return instance;
}

sf::Texture& AssetManager::getTexture(const std::string& filename) {
    auto it = textures.find(filename);
    if (it != textures.end()) {
        return *it->second;
    }

    auto texture = std::make_unique<sf::Texture>();
    if (!texture->loadFromFile(filename)) {
        throw std::runtime_error("AssetManager: Failed to load texture: " + filename);
    }

    sf::Texture& ref = *texture;
    textures[filename] = std::move(texture);
    return ref;
}

sf::Font& AssetManager::getFont(const std::string& filename) {
    auto it = fonts.find(filename);
    if (it != fonts.end()) {
        return *it->second;
    }

    auto font = std::make_unique<sf::Font>();
    if (!font->loadFromFile(filename)) {
        throw std::runtime_error("AssetManager: Failed to load font: " + filename);
    }

    sf::Font& ref = *font;
    fonts[filename] = std::move(font);
    return ref;
}
