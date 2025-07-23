#pragma once
#include "core/UIElement.hpp"
#include "core/UIEvent.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <algorithm>
#include <functional>

class UICheckBox : public UILeaf {
public:
    UICheckBox(const std::string& name = defaultName())
        : UILeaf(name), checked(false) {
			e_size = {20, 20};
			e_fillcolor = {190,190,190,180};
			borderColor = sf::Color::Black;
		}

    // Builder setters
    UICheckBox& setOffset(const sf::Vector2f& pos) {
        e_offset = pos;
        markLayoutDirty();
        return *this;
    }

    UICheckBox& setSize(const sf::Vector2f& size) {
        e_size = size;
        markLayoutDirty();
        return *this;
    }

    UICheckBox& setLabel(const std::string& str) {
        labelText = str;
        label.setString(labelText);
        markLayoutDirty();
        return *this;
    }

    UICheckBox& setFont(const sf::Font& f) {
        font = f;
        label.setFont(f);
        return *this;
    }

    UICheckBox& setTextColor(const sf::Color& color) {
        textColor = color;
        label.setFillColor(color);
        return *this;
    }

    UICheckBox& setBoxFillColor(const sf::Color& color) {
        e_fillcolor = color;
        return *this;
    }

    UICheckBox& setBoxOutlineColor(const sf::Color& color) {
        borderColor = color;
        return *this;
    }

    UICheckBox& setOutlineThickness(float t) {
        boxOutlineThickness = t;
        return *this;
    }

	UICheckBox& setEnable(bool en){
		enabled = en;
		markLayoutDirty();
		return *this;
	}

    UICheckBox& setChecked(bool state) {
        checked = state;
        return *this;
    }

    UICheckBox& setOnToggle(std::function<void(bool)> cb) {
        onToggle = std::move(cb);
        return *this;
    }

    bool isChecked() const { return checked; }

    void Update(float dt) override {
        if (!enabled) return;
        // No per-frame logic by default
    }

	// --- Drawing ---
	void DrawSelf(sf::RenderTarget& target, sf::RenderStates states) override {
		if (!visible) return;

		// Draw checkbox box
		sf::RectangleShape box(sf::Vector2f(e_size.y, e_size.y));
		box.setPosition(e_position);

		sf::Color fillColor = e_fillcolor;
		if (hovered) {
			fillColor.r = std::max(0, static_cast<int>(fillColor.r * 0.7f));
			fillColor.g = std::max(0, static_cast<int>(fillColor.g * 0.7f));
			fillColor.b = std::max(0, static_cast<int>(fillColor.b * 0.7f));
		}
		box.setFillColor(fillColor);
		box.setOutlineColor(borderColor);
		box.setOutlineThickness(boxOutlineThickness);
		target.draw(box, states);

		// Draw check mark if checked
		if (checked) {
			sf::RectangleShape mark;
			mark.setSize({e_size.y * 0.8f, e_size.y * 0.8f});
			mark.setPosition(e_position.x + e_size.y * 0.1f, e_position.y + e_size.y * 0.1f);

			// High-contrast mark color
			float lum = 0.299f * fillColor.r + 0.587f * fillColor.g + 0.114f * fillColor.b;
			sf::Color markColor = (lum > 128.f) ? sf::Color(20,120,20) : sf::Color(20,255,20);
			mark.setFillColor(markColor);
			target.draw(mark, states);
		}

		// Draw label next to box
		label.setFont(font);
		label.setCharacterSize(textSize);
		label.setFillColor(textColor);
		label.setString(labelText);
		label.setPosition(e_position.x + e_size.y + labelOffset.x,
						e_position.y + (e_size.y - label.getLocalBounds().height) / 2.f - label.getLocalBounds().top);
		target.draw(label, states);
	}

    void CalculateLayout() override {
        // Position
        if (layoutType == LayoutType::Static) {
            e_position = e_offset;
        } else if (layoutType == LayoutType::Relative) {
            if (auto parentPtr = parent.lock()) {
                e_position = parentPtr->e_position + parentPtr->e_padding + e_offset;
            } else {
                e_position = e_offset;
            }
        }

        // Prepare label
        label.setFont(font);
        label.setCharacterSize(textSize);
        label.setFillColor(textColor);
        label.setString(labelText);

		e_size.x = label.getLocalBounds().width + labelOffset.x + e_size.y;
    }

	void HandleEvent(const UIEvent& event) override {
		if (!enabled) return;

		if (event.type == UIEventType::MouseMove) {
			hovered = contains(event.mousePos);
		}

		if (event.type == UIEventType::MouseDown && contains(event.mousePos)) {
			checked = !checked;
			if (onToggle) onToggle(checked);
		}
	}


private:
    bool contains(const sf::Vector2f& pt) const {
        return pt.x >= e_position.x && pt.x <= e_position.x + e_size.x &&
               pt.y >= e_position.y && pt.y <= e_position.y + e_size.y;
    }

    bool checked;
    std::function<void(bool)> onToggle;

    std::string labelText = "Check";
    sf::Text label;
    sf::Font font = AssetManager::get().getFont("fonts/arial.ttf");
    sf::Color textColor = sf::Color::Black;
    sf::Vector2f labelOffset = {2.5f, 0.f};
    unsigned int textSize = 15;
	bool hovered;

    float boxOutlineThickness = 2.f;
};
