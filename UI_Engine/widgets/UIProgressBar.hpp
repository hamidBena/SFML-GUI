#pragma once
#include "core/UIElement.hpp"
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include <iomanip>
#include <sstream>
#include <limits> 
struct UIProgressBarComponents {
    sf::RectangleShape outerTrack;
    sf::RectangleShape innerTrack;
    std::optional<sf::Text> valueText;
};

class UIProgressBar : public UILeaf {
public:
    UIProgressBar() {
		setFillColor(sf::Color(80, 200, 80, 255));

		e_size = {150, 25};
	}

	// Standard setters
    UIProgressBar& setOffset(const sf::Vector2f& pos) {
        e_offset = pos;
        return *this;
    }
    UIProgressBar& setSize(const sf::Vector2f& size) {
        e_size = size;
		intr_size = e_size;
        return *this;
    }
	UIProgressBar& setPadding(const sf::Vector2f& pad) {
		e_padding = pad;
		return *this;
	}
	UIProgressBar& setFillColor(const sf::Color& color) {
        e_fillcolor = color;
        return *this;
    }
	UIProgressBar& setBorderColor(const sf::Color& color) {
        e_borderColor = color;
        return *this;
    }
	UIProgressBar& setBorderThickness(float thickness) {
		e_borderThickness = thickness;
        return *this;
    }
	UIProgressBar& setSizeType(SizeType type) {
		sizeType = type;
		return *this;
	}
	UIProgressBar& setLayoutType(LayoutType type) {
        layoutType = type;
        return *this;
    }
	UIProgressBar& setEnable(bool en) {
		enabled = en;
		return *this;
	}
	UIProgressBar& setVisible(bool vis) {
		visible = vis;
		return *this;
	}

	// WidgetSpecific setters
	UIProgressBar& setBoundValue(float* bound){
		boundValue = bound;
		return *this;
	}
	UIProgressBar& setRange(float min, float max){
		minValue = min; maxValue = max;
		return *this;
	}
	UIProgressBar& setValue(float val){
		value = val;
		if(boundValue) *boundValue = val;
		intr_value = val;
		return *this;
	}

	void syncBound(){
		if(!boundValue) return;
		value = *boundValue;
		intr_value = *boundValue;
	}

	// --- Drawing ---
	void DrawSelf(sf::RenderTarget& target) override {
		syncBound();
		shapes = buildShapes();
        target.draw(shapes.outerTrack);
        target.draw(shapes.innerTrack);
        if (shapes.valueText) target.draw(*shapes.valueText);
	}

	void HandleWidgetEvent(const UIEvent& event) override {}	//no events needed for a progressbar

	void UpdateWidget(const float dt) override {}	//will be added soon

private:
    float minValue = 0.f, maxValue = 100.f;
    float* boundValue = nullptr;
	float value = 0.f;	Interpolated<float> intr_value;

    bool showValue = true;
    sf::Font font = AssetManager::get().getFont("fonts/arial.ttf");
	sf::Color textColor;
	int textSize = 18;

	std::function<void(float)> onChange;

	UIProgressBarComponents shapes = buildShapes();

private:
	UIProgressBarComponents buildShapes() const {
		UIProgressBarComponents shapes;

		// Layout & normalized t
		const auto  pos   = intr_position.getValue();
		const auto  size  = intr_size.getValue();
		const float range = maxValue - minValue;
		const float rawT  = (range != 0.f)
							? (intr_value - minValue) / range
							: 0.f;
		const float t     = std::clamp(rawT, 0.f, 1.f);

		// Track dimensions
		const float outerH = size.y / 1.5f;
		const float trackY = pos.y + (size.y - outerH) * 0.5f;

		// ——— Outer Track ———
		shapes.outerTrack.setSize({ size.x, outerH });
		shapes.outerTrack.setPosition(pos.x, trackY);
		shapes.outerTrack.setFillColor({ 50, 50, 50, 180 });
		shapes.outerTrack.setOutlineColor(e_borderColor);
		shapes.outerTrack.setOutlineThickness(e_borderThickness);

		// ——— Inner Filled Bar ———
		shapes.innerTrack.setSize({ size.x * t, outerH });
		shapes.innerTrack.setPosition(pos.x, trackY);
		shapes.innerTrack.setFillColor(e_fillcolor);

		// ——— Optional Value Text ———
		if (showValue) {
			shapes.valueText.emplace();

			auto& txt = *shapes.valueText;
			const float pct = t * 100.f;

			std::ostringstream ss;
			ss << std::fixed << std::setprecision(2) << pct;
			txt.setString(ss.str());

			txt.setFont(font);
			txt.setCharacterSize(textSize);
			txt.setFillColor(textColor);

			auto bounds = txt.getLocalBounds();
			txt.setOrigin(
				bounds.left + bounds.width  * 0.5f,
				bounds.top  + bounds.height * 0.5f
			);
			txt.setPosition(
				pos.x + size.x * 0.5f,
				pos.y + size.y * 0.5f
			);
		}
		// else shapes.valueText stays disengaged

		return shapes;
	}

};
