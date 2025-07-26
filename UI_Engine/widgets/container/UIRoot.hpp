#pragma once

#include "core/UIElement.hpp"
#include <SFML/Graphics/RectangleShape.hpp>
#include <functional>

struct UIRootComponents {
    sf::RectangleShape background;
    std::optional<sf::RectangleShape> headerBar;
    std::optional<sf::Text> headerText;
    std::optional<sf::ConvexShape> debugTriangle;
	sf::RectangleShape toggleButton;
};

class UIRoot : public UIContainer {
public:
    UIRoot(){
		setFillColor(sf::Color(60, 60, 63, 200));
		setBorderColor(sf::Color(38, 38, 30, 220));
		setHeaderColor(sf::Color(90, 190, 90, 255));
		setHeaderHeight(30);
		headerTitle = "Root Menu";
	}

    // Standard setters
    UIRoot& setOffset(const sf::Vector2f& pos) {
        e_offset = pos;
        return *this;
    }
    UIRoot& setSize(const sf::Vector2f& size) {
        e_size = size;
		intr_size = e_size;
        return *this;
    }
	UIRoot& setPadding(const sf::Vector2f& pad) {
		e_padding = pad;
		return *this;
	}
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
	UIRoot& setSizeType(SizeType type) {
		sizeType = type;
		return *this;
	}
	UIRoot& setLayoutType(LayoutType type) {
        layoutType = type;
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

	// Container setters
	UIRoot& setHeaderTitle(std::string title){headerTitle = title; return *this;}
	UIRoot& setHeaderColor(const sf::Color& color){headerColor = color; return *this;}
	UIRoot& setHeaderHeight(float height){headerHeight = height; intr_headerHeight = height; return *this;}
	UIRoot& setChildrenVisible(bool vis){childrenVisible = vis; return *this;}

	// Container override functions
	void UpdateHeaderSize() override {headerSize = shapes.headerText->getLocalBounds().getSize();}

    void DrawSelf(sf::RenderTarget& target) override {
		shapes = buildShapes();
		target.draw(shapes.background);
		if (shapes.headerBar)   target.draw(*shapes.headerBar);
		if (shapes.headerText)  target.draw(*shapes.headerText);
		if (shapes.debugTriangle) target.draw(*shapes.debugTriangle);
		target.draw(shapes.toggleButton);
    }

    void HandleWidgetEvent(const UIEvent& event) override {
		if (headerHeight > 0.f) {
            sf::FloatRect headerbounds = shapes.headerBar->getGlobalBounds();
            sf::FloatRect togglebounds = shapes.toggleButton.getGlobalBounds();
            if (event.type == UIEventType::MouseDown) {
				if (auto* data = std::get_if<MouseEventData>(&event.data)){
					if(togglebounds.contains(data->pos)) {childrenVisible = !childrenVisible; return;}

					if(headerbounds.contains(data->pos)){
						dragging = true;
						dragOffset = data->pos - e_position; 
					}
				}
			}

			if (event.type == UIEventType::MouseUp) {
				dragging = false;
			}

			if (event.type == UIEventType::MouseMove){
				if(auto* data = std::get_if<MouseEventData>(&event.data)){
					if(dragging){
						setOffset(data->pos - dragOffset);
					}

					if(togglebounds.contains(data->pos)){
						toggle_hovered = true;
					}else{
						toggle_hovered = false;
					}
				}
			}

			if(!childrenVisible) return;
			for(auto& child:children){
				child->HandleEvent(event);
			}
		}
    }

private:
    // --- Dragging state ---
    bool dragging = false;
	bool toggle_hovered = false;
    sf::Vector2f dragOffset; // Mouse offset from top-left of root when drag starts

	UIRootComponents shapes = buildShapes();

private:
	UIRootComponents buildShapes() const {
		UIRootComponents shapes;

		sf::Vector2f toggleButtonSize = {25, 8 };

		// --- Header ---
		if (headerHeight > 0.f) {
			// Slightly darkened header bar
			sf::Color darker = headerColor;
			darker.r = static_cast<sf::Uint8>(darker.r * 0.7f);
			darker.g = static_cast<sf::Uint8>(darker.g * 0.7f);
			darker.b = static_cast<sf::Uint8>(darker.b * 0.7f);
			
			sf::RectangleShape headerRect({intr_size.getValue().x, intr_headerHeight.getValue()});
			headerRect.setPosition(intr_position.getValue().x, intr_position.getValue().y);
			headerRect.setFillColor(darker);
			headerRect.setOutlineColor(headerBorderColor);
			headerRect.setOutlineThickness(2.f);
			shapes.headerBar = headerRect;

			// Text color based on luminance
			float lum = 0.299f * headerColor.r + 0.587f * headerColor.g + 0.114f * headerColor.b;
			sf::Color textColor = (lum > 128.f) ? sf::Color::Black : sf::Color::White;

			sf::Text headerText;
			headerText.setFont(font);
			headerText.setCharacterSize(24);
			headerText.setFillColor(textColor);
			headerText.setString(headerTitle);
			headerText.setPosition(
				intr_position.getValue().x + toggleButtonSize.x + 10,//10 for little padding
				intr_position.getValue().y
			);
			shapes.headerText = headerText;
		}

		// --- Background ---
		shapes.background.setSize({intr_size.getValue().x, intr_size.getValue().y - intr_headerHeight.getValue()});
		shapes.background.setPosition({intr_position.getValue().x, intr_position.getValue().y + intr_headerHeight.getValue()});
		shapes.background.setFillColor(e_fillcolor);
		shapes.background.setOutlineColor(e_borderColor);
		shapes.background.setOutlineThickness(e_borderThickness);

		sf::RectangleShape toggleButton;
		toggleButton.setSize(sf::Vector2f(25, 8));
		toggleButton.setPosition(intr_position.getValue().x + 5, intr_position.getValue().y + intr_headerHeight.getValue()/2.f - toggleButtonSize.y/2.f);

		if(toggle_hovered) toggleButton.setFillColor({100,100,100,200});
		else toggleButton.setFillColor({50,50,50,200});

		shapes.toggleButton = toggleButton;

		return shapes;
	}	
};

