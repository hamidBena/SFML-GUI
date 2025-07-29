#pragma once

#include "core/UIElement.hpp"
#include <SFML/Graphics/RectangleShape.hpp>
#include <functional>

struct UIRootComponents {
    sf::RectangleShape background;
};

class UIRoot : public UIContainer {
public:
    UIRoot(){
		setFillColor(sf::Color(200, 0, 0, 50));
		setBorderColor(sf::Color::Transparent);

		headerTitle = "Root Menu";
	}

    // Standard setters
	UIRoot& setFillColor(const sf::Color& color) {
        e_fillcolor = color;
        return *this;
    }
	UIRoot& setBorderColor(const sf::Color& color) {
        e_borderColor = color;
        return *this;
    }
	UIRoot& setBorderThickness(float thickness) {
		e_borderThickness = thickness;
        return *this;
    }
	UIRoot& setEnable(bool en) {
		enabled = en;
		return *this;
	}
	UIRoot& setVisible(bool vis) {
		visible = vis;
		return *this;
	}

    void DrawSelf(sf::RenderTarget& target) override {
		e_size = sf::Vector2f(target.getSize());
		intr_size = e_size;
		shapes = buildShapes();
		target.draw(shapes.background);
    }


private:
    // --- Dragging state ---
    bool dragging = false;
	bool toggle_hovered = false;
    sf::Vector2f dragOffset; // Mouse offset from top-left of root when drag starts

	bool fitWindowOverride = false;

	UIRootComponents shapes = buildShapes();

private:
	UIRootComponents buildShapes() const {
		UIRootComponents shapes;

		// --- Background ---
		shapes.background.setSize({intr_size.getValue().x, intr_size.getValue().y});
		shapes.background.setPosition({0,0});
		shapes.background.setFillColor(e_fillcolor);
		shapes.background.setOutlineColor(e_borderColor);
		shapes.background.setOutlineThickness(e_borderThickness);

		return shapes;
	}	
};

