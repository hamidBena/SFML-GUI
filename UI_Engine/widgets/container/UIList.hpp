#pragma once

#include "core/UIElement.hpp"
#include <SFML/Graphics/RectangleShape.hpp>
#include <functional>

struct UIListComponents {
	sf::RectangleShape background;
	std::optional<sf::RectangleShape> headerBar;
	std::optional<sf::Text> headerText;
	std::optional<sf::ConvexShape> debugTriangle;
	sf::RectangleShape toggleButton;
};

class UIList : public UIContainer {
public:
    UIList() {
		setFillColor(sf::Color(60, 60, 63, 230));
		setBorderColor(sf::Color(38, 38, 30, 220));
		setHeaderColor(sf::Color(90, 190, 90, 255));
		setHeaderHeight(30);

		e_padding = {10,10};
		spacing = 10.f;
		headerTitle = "List Menu";
	}

	// Standard setters
    UIList& setOffset(const sf::Vector2f& pos) {
        e_offset = pos;
        return *this;
    }
    UIList& setSize(const sf::Vector2f& size) {
        e_size = size;
		intr_size = e_size;
        return *this;
    }
	UIList& setPadding(const sf::Vector2f& pad) {
		e_padding = pad;
		return *this;
	}
	UIList& setFillColor(const sf::Color& color) {
        e_fillcolor = color;
        return *this;
    }
	UIList& setBorderColor(const sf::Color& color) {
        e_borderColor = color;
        return *this;
    }
	UIList& setBorderThickness(float thickness) {
		e_borderThickness = thickness;
        return *this;
    }
	UIList& setSizeType(SizeType type) {
		sizeType = type;
		return *this;
	}
	UIList& setLayoutType(LayoutType type) {
        layoutType = type;
        return *this;
    }
	UIList& setEnable(bool en) {
		enabled = en;
		return *this;
	}
	UIList& setVisible(bool vis) {
		visible = vis;
		return *this;
	}

	// Container setters
	UIList& setHeaderTitle(std::string title){headerTitle = title; return *this;}
	UIList& setHeaderColor(const sf::Color& color){headerColor = color; return *this;}
	UIList& setHeaderHeight(float height){headerHeight = height; intr_headerHeight = height; return *this;}
	UIList& setChildrenVisible(bool vis){childrenVisible = vis; return *this;}

	// Container override functions
	void UpdateHeaderSize() override {headerSize = shapes.headerText->getLocalBounds().getSize();}

	// Widget specific setters
	UIList& setSpacing(float sp){
		spacing = sp;
		return *this;
	}
	UIList& setHorizontal(bool hr){
		horizontal = hr;
		return *this;
	}
	UIList& setWidth(unsigned int wd){
		rowWidth = wd;
		return *this;
	}

	// Widget specific getters
	float getSpacing(){return spacing;}
	bool getHorizontal(){return horizontal;}
	unsigned int getWidth(){return rowWidth;}

	//list layout logic
	void CalculateLayout() override {
		updateAccumulatedOffset();
		PositionPass();

		for (auto& child : children) {
			child->CalculateLayout();
		}
		EmplaceChildren();
		SizePass();
	}

	void EmplaceChildren(){
		sf::Vector2f cellSize = GetMaxCellSize();
		if(horizontal){
			float currentX = e_position.x + e_padding.x;
			int counter = 0;
			cellSize.y += spacing;

			for (auto& child : children) {
				child->Move({0, cellSize.y * counter});
				child->e_position.x = currentX;
				child->intr_position = child->e_position;

				if (counter >= rowWidth - 1) {
					currentX += cellSize.x + spacing;
					counter = 0;
				} else {
					counter++;
				}
			}
		}else{
			float currentY = e_position.y + headerHeight + e_padding.y;
			int counter = 0;
			cellSize.x += spacing;
			
			for(auto& child : children){
				child->Move({cellSize.x * counter,0});

				child->e_position.y = currentY;
				child->intr_position = child->e_position;

				if(counter >= rowWidth-1){
					currentY += cellSize.y + spacing;
					counter = 0;
				}else{
					counter ++;
				}
			}
		}
	}

	sf::Vector2f GetMaxCellSize(){
		sf::Vector2f maxSize = {0,0};
		for(auto& child:children){
			maxSize.x = std::max(maxSize.x, child->e_size.x);
			maxSize.y = std::max(maxSize.y, child->e_size.y);
		}
		return maxSize;
	}


public:
    void DrawSelf(sf::RenderTarget& target) override {
		shapes = buildShapes();
		target.draw(shapes.background);
		if (shapes.headerBar)     target.draw(*shapes.headerBar);
		if (shapes.headerText)    target.draw(*shapes.headerText);
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
    bool horizontal = false;
	bool toggle_hovered = false;
	float spacing = 5.f;
	unsigned int rowWidth = 1;

	UIListComponents shapes = buildShapes();
private:
	UIListComponents buildShapes() const {
		UIListComponents shapes;

		sf::Vector2f toggleButtonSize = {25, 8 };

		// --- Header ---
		if (headerHeight > 0.f) {
			// Slightly darkened header bar
			sf::Color darker = headerColor;
			darker.r = static_cast<sf::Uint8>(darker.r * 0.7f);
			darker.g = static_cast<sf::Uint8>(darker.g * 0.7f);
			darker.b = static_cast<sf::Uint8>(darker.b * 0.7f);

			sf::RectangleShape headerRect({intr_size.getValue().x, intr_headerHeight.getValue()});
			headerRect.setPosition(intr_position.getValue());
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
