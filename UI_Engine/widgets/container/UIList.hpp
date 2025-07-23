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
    UIList(const std::string& name = defaultName()) : UIContainer(name) {
		e_fillcolor = sf::Color(75, 75, 70, 230);
		borderColor = sf::Color(58, 58, 60, 180);
		headerColor = sf::Color(100, 200, 90, 255);

		spacing = 10.f;
		headerTitle = "List Menu";
	}

    // Builder setters
    UIList& setOffset(const sf::Vector2f& pos) {
        e_offset = pos;
		markLayoutDirty();
        return *this;
    }
    UIList& setSize(const sf::Vector2f& size) {
        e_size = size;
		intr_size.setValue(e_size);
		markLayoutDirty();
        return *this;
    }
    UIList& setFillColor(const sf::Color& color) {
        e_fillcolor = color;
        return *this;
    }
    UIList& setAnchor(LayoutAnchor anch) {
        anchor = anch;
        return *this;
    }
    UIList& setLayoutType(LayoutType type) {
        layoutType = type;
		markLayoutDirty();
        return *this;
    }
	UIList& setSizeType(SizeType type) {
		sizeType = type;
		markLayoutDirty();
		return *this;
	}
	UIList& setPadding(const sf::Vector2f& pad) {
		e_padding = pad;
		markLayoutDirty();
		return *this;
	}
	UIList& setHeaderTitle(const std::string& title) {
        headerTitle = title;
        return *this;
    }
    UIList& setHeaderColor(const sf::Color& color) {
        headerColor = color;
        return *this;
    }
    UIList& setHeaderHeight(float height) {
        headerHeight = height;
		markLayoutDirty();
        return *this;
    }

	UIList& setEnable(bool en) {
		enabled = en;
		markLayoutDirty();
		return *this;
	}

	UIList& setVisible(bool vis) {
		visible = vis;
		return *this;
	}

	UIList& setSpacing(float space) {
		spacing = space;
		markLayoutDirty();
		return *this;
	}

	UIList& setHorizontal(bool hor) {
		is_horizontal = hor;
		markLayoutDirty();
		return *this;
	}

	bool isHorizontal() {
		return is_horizontal;
	}

	UIList& setOnTick(std::function<void(UIList&, const float)> cb) { onTick = std::move(cb); return *this; }

	void Update(const float dt) override {
		if(!enabled) return;
		for (auto& child : children) {
			child->Update(dt);
		}
		if(onTick) onTick(*this, dt);
	}

    void DrawSelf(sf::RenderTarget& target, sf::RenderStates states) override {
		if(!visible || !enabled) return;

		UIListComponents shapes = buildShapes();
		target.draw(shapes.background, states);
		if (shapes.headerBar)     target.draw(*shapes.headerBar, states);
		if (shapes.headerText)    target.draw(*shapes.headerText, states);
		if (shapes.debugTriangle) target.draw(*shapes.debugTriangle, states);
		target.draw(shapes.toggleButton, states);
    }

    void CalculateLayout() override {
		if(!enabled) return;

		if(!layoutDirty) return;
		layoutDirty = false;

		auto shapes = buildShapes();

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

		sf::Vector2f current_p;
        if (sizeType == SizeType::FitContent) {
			current_p = e_position + e_padding;
			for (auto& child : children) {
				child->CalculateLayout();
				if(is_horizontal) child->e_position.x = current_p.x;
				else child->e_position.y = current_p.y;
				current_p += child->e_size + sf::Vector2f(spacing, spacing);
			}
			
            sf::Vector2f maxSize = shapes.headerText->getLocalBounds().getSize() + e_padding + sf::Vector2f(20,0);
            for (const auto& child : children) {
				if(child->enabled){
					sf::Vector2f childBR = child->e_position + child->e_size - e_position;
					maxSize.x = std::max(maxSize.x, childBR.x);
					maxSize.y = std::max(maxSize.y, childBR.y);
				}
            }
            e_size = maxSize + e_padding * 2.f;
        } else if (sizeType == SizeType::FillParent) {
            if (auto parentPtr = parent.lock()) {
                e_size = parentPtr->e_size-parentPtr->e_padding*2.f - e_offset;
            }
        } else if (sizeType == SizeType::Percent) {
            if (auto parentPtr = parent.lock()) {
                auto parentArea = parentPtr->e_size - parentPtr->e_padding*2.f;
                e_size.x = parentArea.x * (e_size.x / 100.f);
                e_size.y = parentArea.y * (e_size.y / 100.f);
            }
        }

		intr_size.setValue(e_size);

		current_p = e_position + e_padding;
        for (auto& child : children) {
            child->CalculateLayout();

			if(is_horizontal) child->e_position.x = current_p.x;
			else child->e_position.y = current_p.y;
			current_p += child->e_size + sf::Vector2f(spacing, spacing);
        }
    }

    void HandleEvent(const UIEvent& event) override {
		if (!enabled) return;
		auto shapes = buildShapes();
        if (headerHeight > 0.f) {
            sf::FloatRect togglebounds = shapes.toggleButton.getGlobalBounds();
            if (event.type == UIEventType::MouseDown && event.mouseButton == 0) {
                if (togglebounds.contains(event.mousePos)) {
					for(auto& child : children){
						child->enabled = !child->enabled;
					}
					markLayoutDirty();
                    return;
                }
            }
			else if (event.type == UIEventType::MouseMove) {
				if (togglebounds.contains(event.mousePos)) {
					is_hovered = true;
                }else{
					is_hovered = false;
				}
			}
        }
        for (auto& child : children) {
            child->HandleEvent(event);
        }
    }

private:
    std::string headerTitle = "";
    sf::Color headerColor = sf::Color(60, 60, 60);
    float headerHeight = 30.f;
    sf::Font font = AssetManager::get().getFont("fonts/arial.ttf");

    bool is_horizontal = false;
	bool is_hovered = false;

	float spacing = 5.f;

    std::function<void(UIList&, const float dt)> onTick;
	Interpolated<sf::Vector2f> intr_size;

private:
	UIListComponents buildShapes() const {
		UIListComponents shapes;

		// --- Background ---
		shapes.background.setSize(intr_size.getValue());
		shapes.background.setPosition(e_position);
		shapes.background.setFillColor(e_fillcolor);
		shapes.background.setOutlineColor(sf::Color::Black);
		shapes.background.setOutlineThickness(2.f);

		// --- Header ---
		if (headerHeight > 0.f) {
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

			// compute perceived luminance for text color
			float lum = 0.299f * headerColor.r + 0.587f * headerColor.g + 0.114f * headerColor.b;
			sf::Color textColor = (lum > 128.f) ? sf::Color::Black : sf::Color::White;

			sf::Text headerText;
			headerText.setFont(font);
			headerText.setString(headerTitle);
			headerText.setCharacterSize(24);
			headerText.setFillColor(textColor);
			headerText.setPosition(
				e_position.x + headerHeight + 10.f,
				(e_position.y - headerHeight) +
				(headerHeight - headerText.getLocalBounds().height) / 2.f -
				headerText.getLocalBounds().top
			);
			shapes.headerText = headerText;
		}

		// --- Debug marker (if layout dirty) ---
		if (layoutDirty) {
			sf::ConvexShape triangle;
			triangle.setPointCount(3);
			triangle.setPoint(0, e_position);
			triangle.setPoint(1, e_position + sf::Vector2f(10, 0));
			triangle.setPoint(2, e_position + sf::Vector2f(0, 10));
			triangle.setFillColor(sf::Color::Red);
			shapes.debugTriangle = triangle;
		}

		sf::RectangleShape toggleButton;
		toggleButton.setSize(sf::Vector2f(25, 8));
		toggleButton.setPosition(e_position + sf::Vector2f(5,-headerHeight+10));

		if(is_hovered) toggleButton.setFillColor({100,100,100,200});
		else toggleButton.setFillColor({50,50,50,200});

		shapes.toggleButton = toggleButton;

		return shapes;
	}

};
