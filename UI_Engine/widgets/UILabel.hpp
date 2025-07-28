#pragma once
#include "core/UIElement.hpp"
#include <SFML/Graphics/Text.hpp>
#include <string>

struct UILabelComponents {
    std::optional<sf::RectangleShape> background;
    sf::Text text;
	sf::Vector2f textArea;
};

class UILabel : public UILeaf {
public:
    UILabel() {
		e_fillcolor = sf::Color(60, 100, 63, 0);
		textColor = sf::Color(230, 230, 230, 255);
		e_borderColor = sf::Color(20, 20, 20, 255);
		sizeType = SizeType::FitContent;
		e_padding = {5, 5};
	}
	// Standard setters
    UILabel& setOffset(const sf::Vector2f& pos) {
        e_offset = pos;
        return *this;
    }
    UILabel& setSize(const sf::Vector2f& size) {
        e_size = size;
		intr_size = e_size;
        return *this;
    }
	UILabel& setPadding(const sf::Vector2f& pad) {
		e_padding = pad;
		return *this;
	}
	UILabel& setFillColor(const sf::Color& color) {
        e_fillcolor = color;
        return *this;
    }
	UILabel& setBorderColor(const sf::Color& color) {
        e_borderColor = color;
        return *this;
    }
	UILabel& setBorderThickness(float thickness) {
		e_borderThickness = thickness;
        return *this;
    }
	UILabel& setSizeType(SizeType type) {
		sizeType = type;
		return *this;
	}
	UILabel& setLayoutType(LayoutType type) {
        layoutType = type;
        return *this;
    }
	UILabel& setEnable(bool en) {
		enabled = en;
		return *this;
	}
	UILabel& setVisible(bool vis) {
		visible = vis;
		return *this;
	}

	// Widget Specific setters
	UILabel& setLabel(std::string lbl){labelText = lbl; return *this;}
	UILabel& setLabelColor(sf::Color color){textColor = color; return *this;}
	UILabel& setLabelSize(unsigned int size){textSize = size; return *this;}
	UILabel& setRoundingDecimals(unsigned int dec){decimals = dec; return *this;}

	//size pass override cause the label supports "fit content type"
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

			case SizeType::FitContent:	//fit content is either widget specific or not supported
				e_size = shapes.textArea + e_padding;
			case SizeType::Absolute:
				break;
		}
		intr_size = e_size;
	}


	void DrawSelf(sf::RenderTarget& target) override {
		buildShapes(shapes);

		if (shapes.background) target.draw(*shapes.background);
        target.draw(shapes.text);
	}

	void HandleWidgetEvent(const UIEvent& event) override {}	//no event handling needed for a label

	void UpdateWidget(const float dt) override {}	//will be added soon

private:
    std::string labelText = "Label";
    sf::Font font = AssetManager::get().getFont("fonts/arial.ttf");
    sf::Color textColor = sf::Color::Black;
    unsigned int textSize = 18;
	unsigned int decimals = 2;

	UILabelComponents shapes;

private:
	void buildShapes(UILabelComponents& shapes) const {
        auto pos = intr_position.getValue();
        auto size = intr_size.getValue();

        // Background
        if (e_fillcolor.a > 0) {
            sf::RectangleShape bg(size);
            bg.setPosition(pos);
            bg.setFillColor(e_fillcolor);
            bg.setOutlineColor(e_borderColor);
            bg.setOutlineThickness(e_borderThickness);
            shapes.background = bg;
        }

        // Format text with decimal filtering
        std::string displayText = "";
        for (size_t i = 0; i < labelText.size(); ++i) {
            if (labelText[i] != '.') {
                displayText.push_back(labelText[i]);
            } else {
                if (decimals > 0) {
                    displayText.push_back('.');
                    for (int j = 0; j < decimals; ++j) {
                        if (i + 1 >= labelText.size()) break;
                        if (!std::isdigit(labelText[i + 1])) break;
                        ++i;
                        displayText.push_back(labelText[i]);
                    }
                }
                ++i;
                while (i < labelText.size()) {
                    if (std::isdigit(labelText[i])) ++i;
                    else { --i; break; }
                }
            }
        }

        // Text
        shapes.text.setFont(font);
        shapes.text.setCharacterSize(textSize);
        shapes.text.setFillColor(textColor);
        shapes.text.setString(displayText);
        shapes.text.setPosition(pos + e_padding);
		shapes.textArea = shapes.text.getLocalBounds().getSize();
    }
};