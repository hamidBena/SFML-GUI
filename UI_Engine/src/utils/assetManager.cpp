#include "utils/assetManager.hpp"
#include <iostream>
// Locate assets/ at static init time
fs::path AssetManager::asset_dir = [] {
    fs::path current = env::exe_dir();
    while (!current.empty()) {
        fs::path try_path = current / "assets";
        if (fs::exists(try_path) && fs::is_directory(try_path)) {
			std::cout <<try_path << '\n';
            return try_path;
        }
        current = current.parent_path();
    }
    throw std::runtime_error("Could not locate 'assets/' directory.");
}();

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
    if (!texture->loadFromFile((asset_dir / filename).string())) {
        throw std::runtime_error("Failed to load texture: " + filename);
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
    if (!font->loadFromFile((asset_dir / filename).string())) {
        throw std::runtime_error("Failed to load font: " + filename);
    }

    sf::Font& ref = *font;
    fonts[filename] = std::move(font);
    return ref;
}
