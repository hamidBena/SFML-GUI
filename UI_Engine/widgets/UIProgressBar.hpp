#pragma once
#include "core/UIElement.hpp"
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include <iomanip>
#include <sstream>
#include <limits> 
class UIProgressBar : public UILeaf {
public:
    UIProgressBar(const std::string& name = defaultName()) : UILeaf(name) {

	}

    // --- Standard setters (copied from StandardLeaf/UILeaf for consistency) ---
    UIProgressBar& setOffset(const sf::Vector2f& pos) { e_offset = pos; markLayoutDirty(); return *this; }
    UIProgressBar& setSize(const sf::Vector2f& size) { e_size = size; markLayoutDirty(); return *this; }
    UIProgressBar& setFillColor(const sf::Color& color) { e_fillcolor = color; return *this; }
    UIProgressBar& setBorder(float thickness, const sf::Color& color) { borderThickness = thickness; borderColor = color; return *this; }
    UIProgressBar& setFont(const sf::Font& f) { font = f; return *this; }
    UIProgressBar& setTextSize(unsigned int size) { textSize = size; return *this; }
    UIProgressBar& setTextColor(const sf::Color& color) { textColor = color; return *this; }
    UIProgressBar& setPadding(const sf::Vector2f& pad) { e_padding = pad; markLayoutDirty(); return *this; }
    UIProgressBar& setAnchor(LayoutAnchor anch) { anchor = anch; markLayoutDirty(); return *this; }
    UIProgressBar& setLayoutType(LayoutType type) { layoutType = type; markLayoutDirty(); return *this; }
    UIProgressBar& setSizeType(SizeType type) { sizeType = type; markLayoutDirty(); return *this; }
    UIProgressBar& setEnable(bool en) { enabled = en; markLayoutDirty(); return *this; }
    UIProgressBar& setVisible(bool vis) { visible = vis; return *this; }

    // --- Slider-specific setters ---
    UIProgressBar& setRange(float minVal, float maxVal) { minValue = minVal; maxValue = maxVal; setValue(value); return *this; }
    UIProgressBar& setValue(float v) { value = std::clamp(v, minValue, maxValue); 
										if (boundValue) *boundValue = value; 
										if (onChange) onChange(value); 
										intr_value.setValue(value);
										return *this; }
    UIProgressBar& setOnChange(std::function<void(float)> cb) { onChange = std::move(cb); return *this; }
    UIProgressBar& setBoundValue(float* bound) { boundValue = bound; if (bound) setValue(*bound); return *this; }
    UIProgressBar& setShowValue(bool show) { showValue = show; return *this; }
    UIProgressBar& setOnTick(std::function<void(UIProgressBar&, const float&)> cb) { onTick = std::move(cb); return *this; }

	float getValue(){return value;}

	// --- Drawing ---
	void DrawSelf(sf::RenderTarget& target, sf::RenderStates states) override {
		if (!visible) return;

		// Sync with bound value
		if (boundValue){
			if(*boundValue != value) setValue(*boundValue);
		}

		// --- Outer Slider Track ---
		float outerHeight = e_size.y / 3.f;
		sf::RectangleShape outerTrack({e_size.x, outerHeight});
		outerTrack.setPosition(e_position.x, e_position.y + (e_size.y - outerHeight) / 2.f);
		outerTrack.setFillColor(sf::Color(50, 50, 50, 180));
		outerTrack.setOutlineColor(borderColor);
		outerTrack.setOutlineThickness(borderThickness);
		target.draw(outerTrack, states);

		// --- Inner Filled Bar (based on value) ---
		float t = (intr_value - minValue) / (maxValue - minValue);
		float innerWidth = e_size.x * t;
		sf::RectangleShape innerTrack({innerWidth, outerHeight});
		innerTrack.setPosition(e_position.x, e_position.y + (e_size.y - outerHeight) / 2.f);
		innerTrack.setFillColor(sf::Color(80, 200, 80, 255));
		target.draw(innerTrack, states);

		// --- Draw Value Text in the center of the slider ---
		if (showValue) {
			sf::Text txt;
			std::stringstream ss;
			float t = (value - minValue) / (maxValue - minValue) * 100;
			ss << std::fixed << std::setprecision(2) << t;
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
    sf::Color borderColor = sf::Color::Black;
    sf::Font font = AssetManager::get().getFont("fonts/arial.ttf");
    sf::Color textColor = sf::Color::Black;
    unsigned int textSize = 18;
    std::function<void(float)> onChange;
    std::function<void(UIProgressBar&, const float&)> onTick;

	Interpolated<float> intr_value;
};
