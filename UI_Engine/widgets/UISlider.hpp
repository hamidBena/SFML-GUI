#pragma once
#include "core/UIElement.hpp"
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include <iomanip>
#include <sstream>
#include <limits> 

struct UISliderComponents {
    sf::RectangleShape outerTrack;
    sf::RectangleShape innerTrack;
    sf::RectangleShape handle;
    std::optional<sf::Text> valueText;
};

class UISlider : public UILeaf {
public:
    UISlider() {
		setFillColor(sf::Color(80, 200, 80, 255));
		e_size = {150, 25};
	}

	// Standard setters
    UISlider& setOffset(const sf::Vector2f& pos) {
        e_offset = pos;
        return *this;
    }
    UISlider& setSize(const sf::Vector2f& size) {
        e_size = size;
		intr_size = e_size;
        return *this;
    }
	UISlider& setPadding(const sf::Vector2f& pad) {
		e_padding = pad;
		return *this;
	}
	UISlider& setFillColor(const sf::Color& color) {
        e_fillcolor = color;
        return *this;
    }
	UISlider& setBorderColor(const sf::Color& color) {
        e_borderColor = color;
        return *this;
    }
	UISlider& setBorderThickness(float thickness) {
		e_borderThickness = thickness;
        return *this;
    }
	UISlider& setSizeType(SizeType type) {
		sizeType = type;
		return *this;
	}
	UISlider& setLayoutType(LayoutType type) {
        layoutType = type;
        return *this;
    }
	UISlider& setEnable(bool en) {
		enabled = en;
		return *this;
	}
	UISlider& setVisible(bool vis) {
		visible = vis;
		return *this;
	}


	// Widget Specific setters
    UISlider& setValue(float v) { value = std::clamp(v, minValue, maxValue);
								if (boundValue) *boundValue = value; 
								if (onChange) onChange(*this);
								intr_value.setValue(value);
								return *this; }
	UISlider& setBoundValue(float* bound){
		boundValue = bound;
		return *this;
	}
	UISlider& setRange(float min, float max){
		minValue = min; maxValue = max;
		return *this;
	}
	UISlider& setStep(float stp){
		step = stp;
		return *this;
	}
	UISlider& setShowValue(bool sw){ 
		showValue = sw;
		return *this;
	}
	
	// Widget Specific getters
	float getValue() {return value;}

	// Widget Specific callback setters
	void setOnChange(std::function<void (UISlider&)> cb) { onChange = std::move(cb); }


	// --- Drawing ---
	void DrawSelf(sf::RenderTarget& target) override {
        if (boundValue && *boundValue != value)
            setValue(*boundValue);

        shapes = buildShapes();

        target.draw(shapes.outerTrack);
        target.draw(shapes.innerTrack);
        target.draw(shapes.handle);
        if (shapes.valueText) target.draw(*shapes.valueText);
	}

    void HandleWidgetEvent(const UIEvent& event) override {
		sf::FloatRect sliderBounds = shapes.outerTrack.getGlobalBounds();
        if (event.type == UIEventType::MouseMove) {
			if(auto* data = std::get_if<MouseEventData>(&event.data)){
				hovered = sliderBounds.contains(data->pos);
				if(dragging) updateValueFromMouse(data->pos.x);
			}
        }
		if (event.type == UIEventType::MouseDown) {
			if(auto* data = std::get_if<MouseEventData>(&event.data)){
				if(sliderBounds.contains(data->pos)){
					dragging = true;
					updateValueFromMouse(data->pos.x);
				}
			}
        }
		if (event.type == UIEventType::MouseUp) {
            dragging = false;
        }
    }

	void UpdateWidget(const float dt) override {}	//will be added soon

private:
    void updateValueFromMouse(float mouseX) {
        float t = (mouseX - e_position.x) / e_size.x;
        t = std::clamp(t, 0.f, 1.f);
        float newValue = minValue + t * (maxValue - minValue);
        if (step > 0) newValue = minValue + std::round((newValue - minValue) / step) * step;
        setValue(newValue);
    }
    float minValue = 0.f, maxValue = 100.f, step = 1.f;
    float* boundValue = nullptr;

    bool showValue = true;

	//state flags (doesnt require set/get)
    bool hovered = false;
	bool dragging = false;

    sf::Font font = AssetManager::get().getFont("fonts/arial.ttf");
    sf::Color textColor = sf::Color::Black;
    unsigned int textSize = 18;

    std::function<void(UISlider&)> onChange;

	float value = 0.f; Interpolated<float> intr_value;
	UISliderComponents shapes = buildShapes(); 

private:
    UISliderComponents buildShapes() const {
        UISliderComponents shapes;

        const auto pos = intr_position.getValue();
        const auto size = intr_size.getValue();
        const float t = (intr_value - minValue) / (maxValue - minValue);
        const float outerHeight = size.y / 1.5f;

        // --- Outer track ---
        sf::RectangleShape outer(sf::Vector2f(size.x, outerHeight));
        outer.setPosition(pos.x, pos.y + (size.y - outerHeight) / 2.f);
        outer.setFillColor(sf::Color(50, 50, 50, 180));
        outer.setOutlineColor(e_borderColor);
        outer.setOutlineThickness(e_borderThickness);
        shapes.outerTrack = outer;

        // --- Inner track ---
        float innerWidth = size.x * t;
        sf::RectangleShape inner(sf::Vector2f(innerWidth, outerHeight));
        inner.setPosition(pos.x, pos.y + (size.y - outerHeight) / 2.f);
        inner.setFillColor(e_fillcolor);
        shapes.innerTrack = inner;

        // --- Handle ---
        sf::Vector2f handleSize(size.x * 0.05f, size.y * 1.f);
        float handleX = pos.x + t * size.x - handleSize.x / 2.f;
        float handleY = pos.y + (size.y - handleSize.y) / 2.f;

        sf::RectangleShape handle(handleSize);
        handle.setPosition(handleX, handleY);

        sf::Color handleColor = (hovered || dragging) ? sf::Color(100, 180, 255) : sf::Color(200, 200, 200);
        if (dragging) {
            handleColor.r = std::min(255, handleColor.r + 30);
            handleColor.g = std::min(255, handleColor.g + 30);
            handleColor.b = std::min(255, handleColor.b + 30);
        }
        handle.setFillColor(handleColor);
        handle.setOutlineColor(e_borderColor);
        handle.setOutlineThickness(e_borderThickness);
        shapes.handle = handle;

        // --- Value Text ---
        if (showValue) {
            sf::Text txt;
            txt.setFont(font);
            txt.setCharacterSize(textSize);
            txt.setFillColor(textColor);

            std::stringstream ss;
            ss << std::fixed << std::setprecision(2) << value;
            txt.setString(ss.str());

            sf::FloatRect tb = txt.getLocalBounds();
            txt.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
            txt.setPosition(pos.x + size.x / 2.f, pos.y + size.y / 2.f);

            shapes.valueText = txt;
        }

        return shapes;
    }
};
