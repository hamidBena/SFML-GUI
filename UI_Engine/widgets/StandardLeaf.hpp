#pragma once
#include "core/UIElement.hpp"
#include <SFML/Graphics/Text.hpp>
#include <string>

/*
	default template for leaf elements
*/

class UILabel : public UILeaf {
public:
    UILabel(const std::string& name = defaultName()) : UILeaf(name) {}

    UILabel& setText(const std::string& str) {
        text.setString(str);
        labelText = str;
        CalculateLayout();
        return *this;
    }
    UILabel& setFont(const sf::Font& f) {
        font = f;
        text.setFont(f);
        CalculateLayout();
        return *this;
    }
    UILabel& setTextSize(unsigned int size) {
        textSize = size;
        text.setCharacterSize(size);
        CalculateLayout();
        return *this;
    }
    UILabel& setTextColor(const sf::Color& color) {
        textColor = color;
        text.setFillColor(color);
        return *this;
    }

	UILabel& setEnable(bool en) {
		enabled = en;
		return *this;
	}

	UILabel& setVisible(bool vis) {
		visible = vis;
		return *this;
	}

	UILabel& setOnTick(std::function<void(UILabel&)> cb) { onTick = std::move(cb); return *this; }

    void DrawSelf(sf::RenderTarget& target, sf::RenderStates states) override {
		if(!visible) return; // Skip rendering if not visible

        text.setFont(font);
        text.setCharacterSize(textSize);
        text.setFillColor(textColor);
        text.setString(labelText);
        text.setPosition(e_position);
        target.draw(text, states);
    }

    void CalculateLayout() override {
        // Position logic (similar to UIButton)
        if(layoutType == LayoutType::Static) {
            e_position = e_offset;
        } else if(layoutType == LayoutType::Relative) {
            if (auto parentPtr = parent.lock()) {
                e_position = parentPtr->e_position + parentPtr->e_padding + e_offset;
            } else {
                e_position = e_offset;
            }
        } else if(layoutType == LayoutType::Percent) {
            if (auto parentPtr = parent.lock()) {
                sf::Vector2f parentSize = parentPtr->e_size - parentPtr->e_padding * 2.0f;
                e_position.x = parentPtr->e_position.x + parentPtr->e_padding.x + (parentSize.x * (e_offset.x / 100.f));
                e_position.y = parentPtr->e_position.y + parentPtr->e_padding.y + (parentSize.y * (e_offset.y / 100.f));
            }
        } else if(layoutType == LayoutType::Anchor) {
            e_position = e_offset;
        }
        // Size: auto-fit to text
        e_size.x = text.getLocalBounds().width;
        e_size.y = text.getLocalBounds().height;
    }

private:
    std::string labelText = "Label";
    sf::Text text;
    sf::Font font = AssetManager::get().getFont("fonts/arial.ttf");
    sf::Color textColor = sf::Color::Black;
    unsigned int textSize = 18;

	std::function<void(UISlider&)> onTick;

};