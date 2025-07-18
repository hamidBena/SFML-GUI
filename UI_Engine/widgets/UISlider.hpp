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
    UISlider(const std::string& name = defaultName()) : UILeaf(name) {}

    // --- Standard setters (copied from StandardLeaf/UILeaf for consistency) ---
    UISlider& setOffset(const sf::Vector2f& pos) { e_offset = pos; CalculateLayout(); return *this; }
    UISlider& setSize(const sf::Vector2f& size) { e_size = size; CalculateLayout(); return *this; }
    UISlider& setFillColor(const sf::Color& color) { e_fillcolor = color; CalculateLayout(); return *this; }
    UISlider& setBorder(float thickness, const sf::Color& color) { borderThickness = thickness; borderColor = color; CalculateLayout(); return *this; }
    UISlider& setFont(const sf::Font& f) { font = f; return *this; }
    UISlider& setTextSize(unsigned int size) { textSize = size; return *this; }
    UISlider& setTextColor(const sf::Color& color) { textColor = color; return *this; }
    UISlider& setPadding(const sf::Vector2f& pad) { e_padding = pad; CalculateLayout(); return *this; }
    UISlider& setAnchor(LayoutAnchor anch) { anchor = anch; CalculateLayout(); return *this; }
    UISlider& setLayoutType(LayoutType type) { layoutType = type; CalculateLayout(); return *this; }
    UISlider& setSizeType(SizeType type) { sizeType = type; CalculateLayout(); return *this; }
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
        if (boundValue) value =std::clamp(*boundValue, minValue, maxValue);

		// track
        float trackHeight = e_size.y / 6.f;
        sf::RectangleShape track({e_size.x, trackHeight});
        track.setPosition(e_position.x, e_position.y + e_size.y / 2.f - trackHeight / 2.f);
        track.setFillColor(e_fillcolor);
        track.setOutlineColor(borderColor);
        track.setOutlineThickness(borderThickness);
        target.draw(track, states);

        // Draw handle as rectangle with hover/sliding effect
        float t = (value - minValue) / (maxValue - minValue);
        float handleWidth = e_size.y * 0.4f;
        float handleHeight = e_size.y * 0.9f;
        float handleX = e_position.x + t * e_size.x - handleWidth / 2.f;
        float handleY = e_position.y + e_size.y / 2.f - handleHeight / 2.f;
        sf::RectangleShape handle({handleWidth, handleHeight});
        handle.setPosition(handleX, handleY);
        sf::Color handleColor = (hovered || dragging) ? sf::Color(60, 160, 255) : sf::Color(200, 200, 200);
        if (dragging) {
            // Sliding effect: brighten handle
            handleColor.r = std::min(255, handleColor.r + 40);
            handleColor.g = std::min(255, handleColor.g + 40);
            handleColor.b = std::min(255, handleColor.b + 40);
        }
        handle.setFillColor(handleColor);
        handle.setOutlineColor(borderColor);
        handle.setOutlineThickness(borderThickness);
        target.draw(handle, states);

        // Draw value text
        if (showValue) {
            sf::Text txt;
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2) << value;
            txt.setString(ss.str());

            txt.setFont(font);
            txt.setCharacterSize(textSize);
            txt.setFillColor(textColor);
            txt.setPosition(e_position.x + e_size.x + 10, e_position.y + e_size.y / 2.f - txt.getLocalBounds().height / 2.f - txt.getLocalBounds().top);
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
