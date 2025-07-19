#pragma once
#include "core/UIElement.hpp"
#include <SFML/Graphics/Text.hpp>
#include <string>


class UILabel : public UILeaf {
public:
    UILabel(const std::string& name = defaultName()) : UILeaf(name) {
		sizeType = SizeType::FitContent;
	}

    // --- Standard builder-style setters ---
    UILabel& setOffset(const sf::Vector2f& pos) {
        e_offset = pos;
		markLayoutDirty();
        return *this;
    }
    UILabel& setSize(const sf::Vector2f& size) {
        e_size = size;
		markLayoutDirty();
        return *this;
    }
    UILabel& setFillColor(const sf::Color& color) {
        e_fillcolor = color;
        return *this;
    }
    UILabel& setAnchor(LayoutAnchor anch) {
        anchor = anch;
		markLayoutDirty();
        return *this;
    }
    UILabel& setLayoutType(LayoutType type) {
        layoutType = type;
		markLayoutDirty();
        return *this;
    }
    UILabel& setSizeType(SizeType type) {
        sizeType = type;
		markLayoutDirty();
        return *this;
    }
    UILabel& setPadding(const sf::Vector2f& pad) {
        e_padding = pad;
		markLayoutDirty();
        return *this;
    }
    UILabel& setBorder(float thickness, const sf::Color& color) {
        borderThickness = thickness;
        borderColor = color;
        return *this;
    }
    // --- Widget-specific setters ---
    UILabel& setText(const std::string& str) {
        text.setString(str);
        labelText = str;
        return *this;
    }
    UILabel& setFont(const sf::Font& f) {
        font = f;
        return *this;
    }
    UILabel& setTextSize(unsigned int size) {
        textSize = size;
        text.setCharacterSize(size);
        return *this;
    }
    UILabel& setTextColor(const sf::Color& color) {
        textColor = color;
        text.setFillColor(color);
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

	UILabel& setDecimal(int dec) {
		decimals = std::max(0, std::min(dec, 6));
		return *this;
	}

	UILabel& setOnTick(std::function<void(const float)> cb) {
		onTick = std::move(cb);
		return *this;
	}

	void Update(const float dt) override {
		if(!enabled) return;

		if(onTick) onTick(dt);
	}

	void DrawSelf(sf::RenderTarget& target, sf::RenderStates states) override {
		if (!visible) return;

		// Draw background (if alpha > 0)
		if (e_fillcolor.a > 0) {
			sf::RectangleShape rect(e_size);
			rect.setPosition(e_position);
			rect.setFillColor(e_fillcolor);
			rect.setOutlineColor(borderColor);
			rect.setOutlineThickness(borderThickness);
			target.draw(rect, states);
		}

		std::string displayText="";
		for(auto counter=0; counter<labelText.size(); counter++){
			if(labelText.at(counter) != '.'){
				displayText.push_back(labelText.at(counter));
			}else{
				if(decimals > 0){
					displayText.push_back(labelText.at(counter));
					for(auto i=0; i<decimals; i++){
						if(!std::isdigit(labelText.at(counter+1))) break;

						counter++;
						char ch = labelText.at(counter);
						displayText.push_back(ch);
					}
				}
				counter++;
				while(counter<labelText.size()){
					if(std::isdigit(labelText.at(counter)))	counter++;
					else {counter--; break;}
				}
			}
		}

		// === Draw text ===
		text.setFont(font);
		text.setCharacterSize(textSize);
		text.setFillColor(textColor);
		text.setPosition(e_position + e_padding);
		text.setString(displayText);

		target.draw(text, states);
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


        if (sizeType == SizeType::FillParent) {
            if (auto parentPtr = parent.lock()) {
                e_size = parentPtr->e_size - parentPtr->e_padding/0.5f - e_offset;
            }
        } else if (sizeType == SizeType::Percent) {
            if (auto parentPtr = parent.lock()) {
                auto parentArea = parentPtr->e_size - parentPtr->e_padding/0.5f;
                e_size.x = parentArea.x * (e_size.x / 100.f);
                e_size.y = parentArea.y * (e_size.y / 100.f);
            }
        } else if (sizeType == SizeType::FitContent) {
            e_size.x = text.getLocalBounds().width + 15;
            e_size.y = text.getLocalBounds().height + 20;
        }
    }

private:
    std::string labelText = "Label";
    sf::Text text;
    sf::Font font = AssetManager::get().getFont("fonts/arial.ttf");
    sf::Color textColor = sf::Color::Black;
    unsigned int textSize = 18;
	unsigned int decimals = 2;

	std::function<void(const float)> onTick;
};