#pragma once
#include "core/UIElement.hpp"
#include "core/UIEvent.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <algorithm>
#include <functional>

class UIButton : public UILeaf {
public:
    UIButton(const std::string& name = defaultName()) : UILeaf(name) {}

    // Builder setters
    UIButton& setOffset(const sf::Vector2f& pos) {
        e_offset = pos;
		markLayoutDirty();
        return *this;
    }
	UIButton& setLayoutType(LayoutType type) {
		layoutType = type;
		markLayoutDirty();
		return *this;
	}
	UIButton& setSizeType(SizeType type) {
		sizeType = type;
		markLayoutDirty();
		return *this;
	}
    UIButton& setSize(const sf::Vector2f& size) {
        e_size = size;
		markLayoutDirty();
        return *this;
    }
    UIButton& setFillColor(const sf::Color& color) {
        e_fillcolor = color;
        return *this;
    }
    UIButton& setOutlineColor(const sf::Color& color) {
        e_outlinecolor = color;
        return *this;
    }
    UIButton& setOutlineThickness(float t) {
        e_outlineThickness = t;
        return *this;
    }
    UIButton& setTextColor(const sf::Color& color) {
        textColor = color;
        label.setFillColor(color);
        return *this;
    }
    UIButton& setLabel(const std::string& str) {
        labelText = str;
        label.setString(labelText);
        return *this;
    }
    UIButton& setFont(const sf::Font& f) {
        font = f;
        label.setFont(f);
        return *this;
    }
    UIButton& setTextSize(unsigned int size) {
        textSize = size;
        label.setCharacterSize(size);
        return *this;
    }
    // hover effect configuration	(honestly idk why would i need this, but i know i will)
    UIButton& setHoverDarken(float percent) {
        hoverDarken = percent;
        return *this;
    }

	UIButton& setEnable(bool en) {
		enabled = en;
		return *this;
	}

	UIButton& setVisible(bool vis) {
		visible = vis;
		return *this;
	}

	UIButton& setOnTick(std::function<void(UIButton&)> cb) { onTick = std::move(cb); return *this; }

	void Update(const float dt) override {
		if(!enabled) return;
		if(onTick) onTick(*this);
	}

    void DrawSelf(sf::RenderTarget& target, sf::RenderStates states) override {
		if(!visible) return;

        sf::Color drawColor = e_fillcolor;
        // Visual feedback for button state
        if (pressed) {
            // Darken more when held
            drawColor.r = static_cast<sf::Uint8>(drawColor.r * (1.f - hoverDarken * 1.8f));
            drawColor.g = static_cast<sf::Uint8>(drawColor.g * (1.f - hoverDarken * 1.8f));
            drawColor.b = static_cast<sf::Uint8>(drawColor.b * (1.f - hoverDarken * 1.8f));
        } else if (hovered && hoverDarken > 0.f) {
            // Slightly darken on hover
            drawColor.r = static_cast<sf::Uint8>(drawColor.r * (1.f - hoverDarken));
            drawColor.g = static_cast<sf::Uint8>(drawColor.g * (1.f - hoverDarken));
            drawColor.b = static_cast<sf::Uint8>(drawColor.b * (1.f - hoverDarken));
        }
        sf::RectangleShape rect(e_size);
        rect.setPosition(e_position);
        rect.setFillColor(drawColor);
        rect.setOutlineColor(e_outlinecolor);
        rect.setOutlineThickness(e_outlineThickness);
        // scale effect when pressed
        if (pressed) {
            rect.setScale(0.99f, 0.99f);
            rect.setOrigin(e_size.x * 0.0051f , e_size.y * 0.0051f);
        } else {
            rect.setScale(1.f, 1.f);
            rect.setOrigin(0, 0);
        }
        target.draw(rect, states);
        // Center label
        label.setFont(font);
        label.setCharacterSize(textSize);
        label.setFillColor(textColor);
        label.setString(labelText);
        label.setPosition(e_position.x + (e_size.x - label.getLocalBounds().width) / 2.f,
                            e_position.y + (e_size.y - label.getLocalBounds().height) / 2.f - label.getLocalBounds().top);
        target.draw(label, states);
    }

    void CalculateLayout() override {

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
		interpolated_position = e_position;

		// Calculate size based on sizeType
 		if (sizeType == SizeType::FitContent) {
			std::runtime_error("FitContent size type is not supported for UIButton.");
		} else if (sizeType == SizeType::FillParent) {
			// Fill parent logic (e.g., match parent's size)
			if (auto parentPtr = parent.lock()) {
				e_size = parentPtr->e_size-parentPtr->e_padding/0.5f - e_offset;
			}
		} else if (sizeType == SizeType::Percent) {
			// Percent size logic (e.g., based on parent's size)
			if (auto parentPtr = parent.lock()) {
				auto parentArea = parentPtr->e_size - parentPtr->e_padding/0.5f;
				e_size.x = parentArea.x * (e_size.x / 100.f);
				e_size.y = parentArea.y * (e_size.y / 100.f);
			}
		}

		label.setFont(font);
		label.setCharacterSize(textSize);
		label.setFillColor(textColor);
		label.setString(labelText);

    }

	// lambda setters for event handlers
    UIButton& setOnClick(std::function<void()> cb) {
        onClick = std::move(cb);
        return *this;
    }
	UIButton& setOnRelease(std::function<void()> cb) {
		onRelease = std::move(cb);
		return *this;
	}
	UIButton& setOnHover(std::function<void()> cb) {
		onHover = std::move(cb);
		return *this;
	}
	UIButton& setOnLeave(std::function<void()> cb) {
		onLeave = std::move(cb);
		return *this;
	}
	UIButton& setOnPress(std::function<void()> cb) {
		onPress = std::move(cb);
		return *this;
	}

    void HandleEvent(const UIEvent& event) override {
		if (!enabled) return;
		
        if (event.type == UIEventType::MouseMove) {
            bool hoveredNow = contains(event.mousePos);
            if (hoveredNow && !hovered) {
                hovered = true;
                if (onHover) onHover();
            } else if (!hoveredNow && hovered) {
                hovered = false;
                if (onLeave) onLeave();
            }
        } else if (event.type == UIEventType::MouseDown && contains(event.mousePos)) {
            if (!pressed) { // Only trigger onPress on the first press
                pressed = true;
                if (onPress) onPress();
            }
        } else if (event.type == UIEventType::MouseUp) {
            if (pressed && contains(event.mousePos)) {
                if (onClick) onClick();
            }
            if (pressed && onRelease) onRelease();
            pressed = false;
        }
    }

private:
    bool contains(const sf::Vector2f& pt) const {
        return pt.x >= e_position.x && pt.x <= e_position.x + e_size.x &&
               pt.y >= e_position.y && pt.y <= e_position.y + e_size.y;
    }
    std::string labelText = "Button";
    sf::Text label;
    sf::Font font = AssetManager::get().getFont("fonts/arial.ttf");
    sf::Color textColor = sf::Color::Black;
    float e_outlineThickness = 2.f;
    sf::Color e_outlinecolor = sf::Color::Black;
    unsigned int textSize = 24;

    std::function<void()> onClick;
	std::function<void()> onRelease; 
    std::function<void()> onHover;
    std::function<void()> onLeave;
    std::function<void()> onPress;
    std::function<void(UIButton&)> onTick;


    bool wasPressed = false;
    float hoverDarken = 0.15f;
    bool hovered = false;
    bool pressed = false;
};
