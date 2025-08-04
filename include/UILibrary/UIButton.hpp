#pragma once
#include "core/UIElement.hpp"
#include "core/UIEvent.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <algorithm>
#include <functional>

struct UIButtonComponents {
    sf::RectangleShape background;
    sf::Text label;
};

class UIButton : public UILeaf {
public:
    UIButton() {
		setFillColor(sf::Color(60, 160, 60, 255));
		setLabelColor(sf::Color(220, 230, 220, 255));
		setBorderColor(sf::Color(20, 20, 20, 255));

		sizeType = SizeType::FitContent;
		e_padding = {5, 5};
	}

    // Standard setters
    UIButton& setOffset(const sf::Vector2f& pos) {
        e_offset = pos;
        return *this;
    }
    UIButton& setSize(const sf::Vector2f& size) {
        e_size = size;
		intr_size = e_size;
        return *this;
    }
	UIButton& setPadding(const sf::Vector2f& pad) {
		e_padding = pad;
		return *this;
	}
	UIButton& setFillColor(const sf::Color& color) {
        e_fillcolor = color;
        return *this;
    }
	UIButton& setBorderColor(const sf::Color& color) {
        e_borderColor = color;
        return *this;
    }
	UIButton& setBorderThickness(float thickness) {
		e_borderThickness = thickness;
        return *this;
    }
	UIButton& setSizeType(SizeType type) {
		sizeType = type;
		return *this;
	}
	UIButton& setLayoutType(LayoutType type) {
        layoutType = type;
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

	//Widget Specific setters
	UIButton& setLabel(std::string lbl){labelText = lbl; return *this;}
	UIButton& setLabelColor(sf::Color cl){labelColor = cl; return *this;}
	UIButton& setLabelSize(unsigned int size){labelSize = size; return *this;}
	UIButton& setHoverDarken(float darken){hoverDarken = darken; return *this;}

	//Widget specific getters
	std::string getLabel(){return labelText;}
	sf::Color getLabelColor(){return labelColor;}
	unsigned int getLabelSize(){return labelSize;}
	float getHoverDarken(){return hoverDarken;}
	bool is_pressed(){return pressed;}
	float getDurationHeld(){return durationHeld;}

	//Widget specific callback setters
	void setOnClick(std::function<void(UIButton&, const float&)> cb) { onClick = std::move(cb); }
	void setOnPress(std::function<void(UIButton&)> cb) { onPress = std::move(cb); }
	void setOnRelease(std::function<void(UIButton&, const float&)> cb) { onRelease = std::move(cb); }
	void setOnHeld(std::function<void(UIButton&, const float&)> cb) { onHeld = std::move(cb); }


	void DrawSelf(sf::RenderTarget& target) override {
  		shapes = buildShapes();
        target.draw(shapes.background);
        target.draw(shapes.label);
    }

    void HandleWidgetEvent(const UIEvent& event) override {
		if (event.type == UIEventType::MouseDown) {
			if(!pressed && hovered){
				pressed = true;
				durationHeld = 0; //reset the held duration
				if(onPress) onPress(*this);
			}
		}
		
		if(event.type == UIEventType::MouseUp) {
			if(pressed && hovered){
				if(onClick) onClick(*this, durationHeld);
			}

			if(pressed && onRelease) onRelease(*this, durationHeld);
			pressed = false;
		}
    }

	void UpdateWidget(const float dt) override {
		if(pressed){
			durationHeld += dt;
			if(onHeld) onHeld(*this, durationHeld);
		} 
	}

private:
    std::string labelText = "Button";
    sf::Color labelColor = sf::Color::Black;

	sf::Font font = AssetManager::get().getFont("fonts/arial.ttf");
    unsigned int labelSize = 24;

	float hoverDarken = 0.2;

    std::function<void(UIButton&, const float&)> onClick;
	std::function<void(UIButton&, const float&)> onRelease; 
	std::function<void(UIButton&, const float&)> onHeld;
    std::function<void(UIButton&)> onPress;	//on press doesn't take a duration held cause its only trigerred when the button is just pressed ie. duration = 0

    bool pressed = false;
	float durationHeld = 0;

	UIButtonComponents shapes = buildShapes();

private:
	UIButtonComponents buildShapes() const {
        UIButtonComponents shapes;

        // Background
        sf::Color drawColor = e_fillcolor;
        if (pressed) {
            drawColor.r = static_cast<sf::Uint8>(drawColor.r * (1.f - hoverDarken * 1.8f));
            drawColor.g = static_cast<sf::Uint8>(drawColor.g * (1.f - hoverDarken * 1.8f));
            drawColor.b = static_cast<sf::Uint8>(drawColor.b * (1.f - hoverDarken * 1.8f));
        } else if (hovered && hoverDarken > 0.f) {
            drawColor.r = static_cast<sf::Uint8>(drawColor.r * (1.f - hoverDarken));
            drawColor.g = static_cast<sf::Uint8>(drawColor.g * (1.f - hoverDarken));
            drawColor.b = static_cast<sf::Uint8>(drawColor.b * (1.f - hoverDarken));
        }

        shapes.background.setSize(intr_size.getValue());
        shapes.background.setPosition(intr_position.getValue());
        shapes.background.setFillColor(drawColor);
        shapes.background.setOutlineColor(e_borderColor);
        shapes.background.setOutlineThickness(e_borderThickness);

        if (pressed) {
            shapes.background.setScale(0.99f, 0.99f);
            auto s = intr_size.getValue();
            shapes.background.setOrigin(s.x * 0.0051f, s.y * 0.0051f);
        } else {
            shapes.background.setScale(1.f, 1.f);
            shapes.background.setOrigin(0, 0);
        }

        // Label
        shapes.label.setFont(font);
        shapes.label.setCharacterSize(labelSize);
        shapes.label.setFillColor(labelColor);
        shapes.label.setString(labelText);
        auto lb = shapes.label.getLocalBounds();
        auto pos = intr_position.getValue();
        auto sz  = intr_size.getValue();
        shapes.label.setPosition(
            pos.x + (sz.x - lb.width) / 2.f,
            pos.y + (sz.y - lb.height) / 2.f - lb.top
        );

        return shapes;
    }
};
