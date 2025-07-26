#pragma once

#include "core/UIElement.hpp"
#include <SFML/Graphics/RectangleShape.hpp>
#include <functional>

struct UIGridComponents {
    sf::RectangleShape background;
    std::optional<sf::RectangleShape> headerBar;
    std::optional<sf::Text> headerText;
    std::optional<sf::ConvexShape> debugTriangle;
	sf::RectangleShape toggleButton;
};

class UIGrid : public UIContainer {
public:
    UIGrid(){
		e_fillcolor = sf::Color(70, 70, 73, 230);
		e_borderColor = sf::Color(58, 58, 60, 180);
		headerColor = sf::Color(90, 190, 90, 255);

		headerTitle = "Root grid";
	}

	// Standard setters
    UIGrid& setOffset(const sf::Vector2f& pos) {
        e_offset = pos;
        return *this;
    }
    UIGrid& setSize(const sf::Vector2f& size) {
        e_size = size;
		intr_size = e_size;
        return *this;
    }
	UIGrid& setPadding(const sf::Vector2f& pad) {
		e_padding = pad;
		return *this;
	}
	UIGrid& setFillColor(const sf::Color& color) {
        e_fillcolor = color;
        return *this;
    }
	UIGrid& setBorderColor(const sf::Color& color) {
        e_borderColor = color;
        return *this;
    }
	UIGrid& setBorderThickness(float thickness) {
		e_borderThickness = thickness;
        return *this;
    }
	UIGrid& setSizeType(SizeType type) {
		sizeType = type;
		return *this;
	}
	UIGrid& setLayoutType(LayoutType type) {
        layoutType = type;
        return *this;
    }
	UIGrid& setEnable(bool en) {
		enabled = en;
		return *this;
	}
	UIGrid& setVisible(bool vis) {
		visible = vis;
		return *this;
	}

	// Container setters
	UIGrid& setHeaderTitle(std::string title){headerTitle = title; return *this;}
	UIGrid& setHeaderColor(const sf::Color& color){headerColor = color; return *this;}
	UIGrid& setHeaderHeight(float height){headerHeight = height; intr_headerHeight = height; return *this;}
	UIGrid& setChildrenVisible(bool vis){childrenVisible = vis; return *this;}

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

    void CalculateLayout() override {
		PositionPass();
		SizePass();
		for(auto& child:children) child->CalculateLayout();
	}

	void SizePass() override {
		switch(sizeType){
			case SizeType::Percent:
				if (auto parentPtr = parent.lock()) {
				auto parentArea = parentPtr->e_size - parentPtr->e_padding/0.5f;
				e_size.x = parentArea.x * (e_size.x / 100.f);
				e_size.y = parentArea.y * (e_size.y / 100.f);
				}
				break;
			
			case SizeType::FillParent:
				if (auto parentPtr = parent.lock()) { e_size = parentPtr->e_size - parentPtr->e_padding*2.f - e_offset; }
				break;

			case SizeType::FitContent: {
	
				sf::Vector2f maxSize = shapes.headerText->getLocalBounds().getSize() + e_padding + sf::Vector2f(headerHeight,0);
				for (const auto& child : children) {
					if(child->enabled){
						child->CalculateLayout();
						sf::Vector2f childBR = child->e_position + child->e_size - e_position;
						maxSize.x = std::max(maxSize.x, childBR.x);
						maxSize.y = std::max(maxSize.y, childBR.y);
					}
				}
				e_size = maxSize + e_padding * 2.f;
				break;
			}

			case SizeType::Absolute:
				break;
		}
	}

    void HandleWidgetEvent(const UIEvent& event) override {
		if (headerHeight > 0.f) {
            sf::FloatRect headerbounds = shapes.headerBar->getGlobalBounds();
            sf::FloatRect togglebounds = shapes.toggleButton.getGlobalBounds();
            if (event.type == UIEventType::MouseDown) {
				if (auto* data = std::get_if<MouseEventData>(&event.data)){
					if(togglebounds.contains(data->pos)) {childrenVisible = !childrenVisible; return;}
				}
			}
			if (event.type == UIEventType::MouseMove){
				if(auto* data = std::get_if<MouseEventData>(&event.data)){
					if(togglebounds.contains(data->pos)){
						toggle_hovered = true;
					}else{
						toggle_hovered = false;
					}
				}
			}

			for(auto& child:children){
				child->HandleEvent(event);
			}
		}
    }

private:
	int columns = 1;
	float spacing = 10;
	bool toggle_hovered = false;

	UIGridComponents shapes = buildShapes();

private:
	UIGridComponents buildShapes() const {
		UIGridComponents shapes;

		// --- Background ---
		shapes.background.setSize(intr_size.getValue());
		shapes.background.setPosition(e_position);
		shapes.background.setFillColor(e_fillcolor);
		shapes.background.setOutlineColor(sf::Color::Black);
		shapes.background.setOutlineThickness(2.f);

		// --- Header ---
		if (headerHeight > 0.f) {
			// Slightly darkened header bar
			sf::Color darker = headerColor;
			darker.r = static_cast<sf::Uint8>(darker.r * 0.7f);
			darker.g = static_cast<sf::Uint8>(darker.g * 0.7f);
			darker.b = static_cast<sf::Uint8>(darker.b * 0.7f);

			sf::RectangleShape headerRect({intr_size.getValue().x, headerHeight});
			headerRect.setPosition(e_position.x, e_position.y - headerHeight);
			headerRect.setFillColor(darker);
			headerRect.setOutlineColor(sf::Color::Black);
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
				e_position.x + headerHeight + 10.f,
				(e_position.y - headerHeight) + (headerHeight - headerText.getLocalBounds().height) / 2.f - headerText.getLocalBounds().top
			);
			shapes.headerText = headerText;
		}

		sf::RectangleShape toggleButton;
		toggleButton.setSize(sf::Vector2f(25, 8));
		toggleButton.setPosition(e_position + sf::Vector2f(5,-headerHeight+10));

		if(toggle_hovered) toggleButton.setFillColor({100,100,100,200});
		else toggleButton.setFillColor({50,50,50,200});

		shapes.toggleButton = toggleButton;

		return shapes;
	}	
};

