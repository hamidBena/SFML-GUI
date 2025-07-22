#pragma once
#include "core/UIElement.hpp"
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include <iomanip>
#include <sstream>
#include <limits> 
class UISlider : public UILeaf {
public:
    UISlider(const std::string& name = defaultName()) : UILeaf(name) {

	}

    // --- Standard setters (copied from StandardLeaf/UILeaf for consistency) ---
    UISlider& setOffset(const sf::Vector2f& pos) { e_offset = pos; markLayoutDirty(); return *this; }
    UISlider& setSize(const sf::Vector2f& size) { e_size = size; markLayoutDirty(); return *this; }
    UISlider& setFillColor(const sf::Color& color) { e_fillcolor = color; return *this; }
    UISlider& setBorder(float thickness, const sf::Color& color) { borderThickness = thickness; borderColor = color; return *this; }
    UISlider& setFont(const sf::Font& f) { font = f; return *this; }
    UISlider& setTextSize(unsigned int size) { textSize = size; return *this; }
    UISlider& setTextColor(const sf::Color& color) { textColor = color; return *this; }
    UISlider& setPadding(const sf::Vector2f& pad) { e_padding = pad; markLayoutDirty(); return *this; }
    UISlider& setAnchor(LayoutAnchor anch) { anchor = anch; markLayoutDirty(); return *this; }
    UISlider& setLayoutType(LayoutType type) { layoutType = type; markLayoutDirty(); return *this; }
    UISlider& setSizeType(SizeType type) { sizeType = type; markLayoutDirty(); return *this; }
    UISlider& setEnable(bool en) { enabled = en; return *this; }
    UISlider& setVisible(bool vis) { visible = vis; return *this; }

    // --- Slider-specific setters ---
    UISlider& setRange(float minVal, float maxVal) { minValue = minVal; maxValue = maxVal; setValue(value); return *this; }
    UISlider& setStep(float s) { step = s; return *this; }
    UISlider& setValue(float v) { value = std::clamp(v, minValue, maxValue); if (boundValue) *boundValue = value; if (onChange) onChange(value); return *this; }
    UISlider& setOnChange(std::function<void(float)> cb) { onChange = std::move(cb); return *this; }
    UISlider& setBoundValue(float* bound) { boundValue = bound; if (bound) setValue(*bound); return *this; }
    UISlider& setShowValue(bool show) { showValue = show; return *this; }
    UISlider& setOnTick(std::function<void(UISlider&, const float&)> cb) { onTick = std::move(cb); return *this; }

	// --- Drawing ---
	void DrawSelf(sf::RenderTarget& target, sf::RenderStates states) override {
		if (!visible) return;

		// Sync with bound value
		if (boundValue) value = std::clamp(*boundValue, minValue, maxValue);

		// --- Outer Slider Track ---
		float outerHeight = e_size.y / 3.f;
		sf::RectangleShape outerTrack({e_size.x, outerHeight});
		outerTrack.setPosition(e_position.x, e_position.y + (e_size.y - outerHeight) / 2.f);
		outerTrack.setFillColor(sf::Color(50, 50, 50, 180));
		outerTrack.setOutlineColor(borderColor);
		outerTrack.setOutlineThickness(borderThickness);
		target.draw(outerTrack, states);

		// --- Inner Filled Bar (based on value) ---
		float t = (value - minValue) / (maxValue - minValue);
		float innerWidth = e_size.x * t;
		sf::RectangleShape innerTrack({innerWidth, outerHeight});
		innerTrack.setPosition(e_position.x, e_position.y + (e_size.y - outerHeight) / 2.f);
		innerTrack.setFillColor(sf::Color(80, 200, 80, 255));
		target.draw(innerTrack, states);

		// --- Slider Handle ---
		float handleSize = e_size.y * 0.25f;
		float handleX = e_position.x + t * e_size.x - handleSize / 2.f;
		float handleY = e_position.y + (e_size.y - handleSize) / 2.f;
		sf::RectangleShape handle({handleSize, handleSize});
		handle.setPosition(handleX, handleY);

		sf::Color handleColor = (hovered || dragging) ? sf::Color(100, 180, 255) : sf::Color(200, 200, 200);
		if (dragging) {
			handleColor.r = std::min(255, handleColor.r + 30);
			handleColor.g = std::min(255, handleColor.g + 30);
			handleColor.b = std::min(255, handleColor.b + 30);
		}
		handle.setFillColor(handleColor);
		handle.setOutlineColor(borderColor);
		handle.setOutlineThickness(borderThickness);
		target.draw(handle, states);

		// --- Draw Value Text in the center of the slider ---
		if (showValue) {
			sf::Text txt;
			std::stringstream ss;
			ss << std::fixed << std::setprecision(2) << value;
			txt.setString(ss.str());

			txt.setFont(font);
			txt.setCharacterSize(textSize);
			txt.setFillColor(textColor);

			sf::FloatRect textBounds = txt.getLocalBounds();
			txt.setOrigin(textBounds.left + textBounds.width / 2.f,
						textBounds.top + textBounds.height / 2.f);
			txt.setPosition(e_position.x + e_size.x / 2.f,
							e_position.y + e_size.y / 2.f);
			target.draw(txt, states);
		}
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
    }

    void HandleEvent(const UIEvent& event) override {
        if (!enabled) return;
        if (event.type == UIEventType::MouseMove) {
            hovered = contains(event.mousePos);
            if (dragging) {
                updateValueFromMouse(event.mousePos.x);
            }
        } else if (event.type == UIEventType::MouseDown) {
            if (contains(event.mousePos)) {
                dragging = true;
                updateValueFromMouse(event.mousePos.x);
            }
        } else if (event.type == UIEventType::MouseUp) {
            dragging = false;
        }
    }

    void Update(const float dt) override {
		if(!enabled) return;

        if (onTick) onTick(*this, dt);
    }

private:
    bool contains(const sf::Vector2f& pt) const {
        return pt.x >= e_position.x && pt.x <= e_position.x + e_size.x &&
               pt.y >= e_position.y && pt.y <= e_position.y + e_size.y;
    }
    void updateValueFromMouse(float mouseX) {
        float t = (mouseX - e_position.x) / e_size.x;
        t = std::clamp(t, 0.f, 1.f);
        float newValue = minValue + t * (maxValue - minValue);
        if (step > 0) newValue = minValue + std::round((newValue - minValue) / step) * step;
        setValue(newValue);
    }
    float minValue = 0.f, maxValue = 100.f, value = 0.f, step = 1.f;
    float* boundValue = nullptr;
    bool showValue = true;
    bool dragging = false;
    bool hovered = false;
    sf::Color borderColor = sf::Color::Black;
    float borderThickness = 2.f;
    sf::Font font = AssetManager::get().getFont("fonts/arial.ttf");
    sf::Color textColor = sf::Color::Black;
    unsigned int textSize = 18;
    std::function<void(float)> onChange;
    std::function<void(UISlider&, const float&)> onTick;
};
