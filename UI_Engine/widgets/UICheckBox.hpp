#pragma once
#include "core/UIElement.hpp"
#include "core/UIEvent.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <algorithm>
#include <functional>

struct UICheckBoxComponents {
    sf::RectangleShape box;
    std::optional<sf::RectangleShape> mark;
    sf::Text label;
};

class UICheckBox : public UILeaf {
public:
    UICheckBox(){
			e_size = {20, 20};
			e_fillcolor = {190,190,190,180};
			e_borderColor = sf::Color::Black;
		}

    // Standard setters
    UICheckBox& setOffset(const sf::Vector2f& pos) {
        e_offset = pos;
        return *this;
    }
    UICheckBox& setSize(const sf::Vector2f& size) {
        e_size = size;
		intr_size = e_size;
        return *this;
    }
	UICheckBox& setPadding(const sf::Vector2f& pad) {
		e_padding = pad;
		return *this;
	}
	UICheckBox& setFillColor(const sf::Color& color) {
        e_fillcolor = color;
        return *this;
    }
	UICheckBox& setBorderColor(const sf::Color& color) {
        e_borderColor = color;
        return *this;
    }
	UICheckBox& setBorderThickness(float thickness) {
		e_borderThickness = thickness;
        return *this;
    }
	UICheckBox& setSizeType(SizeType type) {
		sizeType = type;
		return *this;
	}
	UICheckBox& setLayoutType(LayoutType type) {
        layoutType = type;
        return *this;
    }
	UICheckBox& setEnable(bool en) {
		enabled = en;
		return *this;
	}
	UICheckBox& setVisible(bool vis) {
		visible = vis;
		return *this;
	}


    UICheckBox& setOnToggle(std::function<void(bool)> cb) {
        onToggle = std::move(cb);
        return *this;
    }

	// --- Drawing ---
	void DrawSelf(sf::RenderTarget& target) override {
        shapes = buildShapes();
        target.draw(shapes.box);
        if (shapes.mark) target.draw(*shapes.mark);
        target.draw(shapes.label);
	}

	void HandleWidgetEvent(const UIEvent& event) override {
		sf::FloatRect boxBounds = shapes.box.getGlobalBounds();
		if (event.type == UIEventType::MouseDown) {
			if(auto* data = std::get_if<MouseEventData>(&event.data)){
				if(boxBounds.contains(data->pos)){
					checked = !checked;
					if(onToggle) onToggle(checked);
				} 
			}
		}
	}

	void UpdateWidget(const float dt) override {}	//will be added soon


private:
    bool contains(const sf::Vector2f& pt) const {
        return pt.x >= e_position.x && pt.x <= e_position.x + e_size.x &&
               pt.y >= e_position.y && pt.y <= e_position.y + e_size.y;
    }

    bool checked;
    std::function<void(bool)> onToggle;

    std::string labelText = "Check";
    sf::Font font = AssetManager::get().getFont("fonts/arial.ttf");
    sf::Color textColor = sf::Color::Black;
    sf::Vector2f labelOffset = {2.5f, 0.f};
    unsigned int textSize = 15;

	UICheckBoxComponents shapes = buildShapes();

private:
	UICheckBoxComponents buildShapes() const {
        UICheckBoxComponents shapes;

        // Box
        sf::RectangleShape box;
        auto pos = intr_position.getValue();
        auto sz  = intr_size.getValue();
        box.setSize({sz.y, sz.y});
        box.setPosition(pos);
        
        sf::Color fillColor = e_fillcolor;
        if (hovered) {
            fillColor.r = static_cast<sf::Uint8>(fillColor.r * 0.7f);
            fillColor.g = static_cast<sf::Uint8>(fillColor.g * 0.7f);
            fillColor.b = static_cast<sf::Uint8>(fillColor.b * 0.7f);
        }
        box.setFillColor(fillColor);
        box.setOutlineColor(e_borderColor);
        box.setOutlineThickness(e_borderThickness);
        shapes.box = box;

        // Check mark
        if (checked) {
            sf::RectangleShape mark;
            mark.setSize({sz.y * 0.8f, sz.y * 0.8f});
            mark.setPosition(pos + sf::Vector2f(sz.y * 0.1f, sz.y * 0.1f));

            float lum = 0.299f * fillColor.r + 0.587f * fillColor.g + 0.114f * fillColor.b;
            sf::Color markColor = (lum > 128.f) ? sf::Color(20, 120, 20) : sf::Color(20, 255, 20);
            mark.setFillColor(markColor);
            shapes.mark = mark;
        }

        // Label
        shapes.label.setFont(font);
        shapes.label.setCharacterSize(textSize);
        shapes.label.setFillColor(textColor);
        shapes.label.setString(labelText);
        auto lb = shapes.label.getLocalBounds();
        shapes.label.setPosition(
            pos.x + sz.y + labelOffset.x,
            pos.y + (sz.y - lb.height) / 2.f - lb.top + labelOffset.y
        );

        return shapes;
    }
};
