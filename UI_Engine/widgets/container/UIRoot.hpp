#pragma once

#include "core/UIElement.hpp"
#include <SFML/Graphics/RectangleShape.hpp>
#include <functional>

class UIRoot : public UIContainer {
public:
    UIRoot(const std::string& name = defaultName()) : UIContainer(name) {}

    // Builder setters
    UIRoot& setOffset(const sf::Vector2f& pos) {
        e_offset = pos;
		interpolated_position = pos;
		markLayoutDirty();
        return *this;
    }
    UIRoot& setSize(const sf::Vector2f& size) {
        e_size = size;
		markLayoutDirty();
        return *this;
    }
    UIRoot& setFillColor(const sf::Color& color) {
        e_fillcolor = color;
        return *this;
    }
    UIRoot& setAnchor(LayoutAnchor anch) {
        anchor = anch;
        return *this;
    }
    UIRoot& setLayoutType(LayoutType type) {
        layoutType = type;
		markLayoutDirty();
        return *this;
    }
	UIRoot& setSizeType(SizeType type) {
		sizeType = type;
		markLayoutDirty();
		return *this;
	}
	UIRoot& setPadding(const sf::Vector2f& pad) {
		e_padding = pad;
		markLayoutDirty();
		return *this;
	}
	UIRoot& setHeaderTitle(const std::string& title) {
        headerTitle = title;
        return *this;
    }
    UIRoot& setHeaderColor(const sf::Color& color) {
        headerColor = color;
        return *this;
    }
    UIRoot& setHeaderHeight(float height) {
        headerHeight = height;
		markLayoutDirty();
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
	UIRoot& setOnTick(std::function<void(UIRoot&)> cb) {
		 onTick = std::move(cb); return *this; }

	void Update(const float dt) override {
		if(!enabled) return;
		for (auto& child : children) {
			child->Update(dt);
		}
		if(onTick) onTick(*this);
	}

    void DrawSelf(sf::RenderTarget& target, sf::RenderStates states) override {
		if(!visible) return;
		//auto e_position = interpolated_position.getValue();	//testing interpolation

        // main background (root body)
        sf::RectangleShape rect(e_size);
        rect.setPosition(e_position);
        rect.setFillColor(e_fillcolor);
        rect.setOutlineColor(sf::Color::Black);
        rect.setOutlineThickness(2.f);
        target.draw(rect, states);

        // header bar (slightly darker)
        if (headerHeight > 0.f) {
            sf::Color headerBarColor = headerColor;
            headerBarColor.r = static_cast<sf::Uint8>(headerBarColor.r * 0.7f);
            headerBarColor.g = static_cast<sf::Uint8>(headerBarColor.g * 0.7f);
            headerBarColor.b = static_cast<sf::Uint8>(headerBarColor.b * 0.7f);
            sf::RectangleShape headerRect({e_size.x, headerHeight});
            headerRect.setPosition(e_position.x, e_position.y - headerHeight);
            headerRect.setFillColor(headerBarColor);
            headerRect.setOutlineColor(sf::Color::Black);
            headerRect.setOutlineThickness(2.f);
            target.draw(headerRect, states);

            sf::Text headerText;
            headerText.setString(headerTitle);
            headerText.setCharacterSize(24);
            headerText.setFillColor(sf::Color::White);
            headerText.setFont(font);
            headerText.setPosition(e_position.x + 10, (e_position.y - headerHeight) + (headerHeight - headerText.getLocalBounds().height) / 2.f - headerText.getLocalBounds().top);
            target.draw(headerText, states);
        }
		
		if (layoutDirty) {
			sf::ConvexShape triangle;
			triangle.setPointCount(3);
			triangle.setPoint(0, e_position);
			triangle.setPoint(1, e_position + sf::Vector2f(10, 0));
			triangle.setPoint(2, e_position + sf::Vector2f(0, 10));
			triangle.setFillColor(sf::Color::Red);
			target.draw(triangle, states);
		}
    }

    void CalculateLayout() override {
		if (!enabled) return;

		if(!layoutDirty) return;
		layoutDirty = false;

		// position calculations
		switch (layoutType) {
			case LayoutType::Static:
			case LayoutType::Anchor:
				e_position = e_offset;
				break;

			case LayoutType::Relative:
				if (auto parentPtr = parent.lock()) {
					e_position = parentPtr->e_position + parentPtr->e_padding + e_offset;
				} else {
					e_position = e_offset;
				}
				break;

			case LayoutType::Percent:
				if (auto parentPtr = parent.lock()) {
					sf::Vector2f parentSize = parentPtr->e_size - parentPtr->e_padding * 2.0f;
					e_position.x = parentPtr->e_position.x + parentPtr->e_padding.x + (parentSize.x * (e_offset.x / 100.f));
					e_position.y = parentPtr->e_position.y + parentPtr->e_padding.y + (parentSize.y * (e_offset.y / 100.f));
				}
				break;
		}

        // size calculations
		switch (sizeType) {
			case SizeType::FitContent: {
				sf::Vector2f maxSize(0, 0);
				for (const auto& child : children) {
					child->CalculateLayout();
					sf::Vector2f childBR = child->e_position + child->e_size - e_position;
					maxSize.x = std::max(maxSize.x, childBR.x);
					maxSize.y = std::max(maxSize.y, childBR.y);
				}
				e_size = maxSize + e_padding * 2.f;
				break;
			}
			case SizeType::FillParent: {
				if (auto parentPtr = parent.lock()) {
					e_size = parentPtr->e_size - parentPtr->e_padding / 0.5f - e_offset;
				}
				break;
			}
			case SizeType::Percent: {
				if (auto parentPtr = parent.lock()) {
					auto parentArea = parentPtr->e_size - parentPtr->e_padding / 0.5f;
					e_size.x = parentArea.x * (e_size.x / 100.f);
					e_size.y = parentArea.y * (e_size.y / 100.f);
				}
				break;
			}
			case SizeType::Absolute:
				break;
		}

        for (auto& child : children) {
            child->CalculateLayout();
        }
    }

    UIElement* AddChild(std::shared_ptr<UIElement> child) override {
        children.push_back(child);
        child->parent = shared_from_this();
		markLayoutDirty();
        return child.get();
    }

    void HandleEvent(const UIEvent& event) override {
		if (!enabled) return;

        // dragging by header (now above the root)
        if (headerHeight > 0.f) {
            sf::FloatRect headerRect(e_position.x, e_position.y - headerHeight, e_size.x, headerHeight);
            if (event.type == UIEventType::MouseDown && event.mouseButton == 0) {
                if (headerRect.contains(event.mousePos)) {
                    dragging = true;
                    dragOffset = event.mousePos - e_position;
                    return;
                }
            } else if (event.type == UIEventType::MouseUp && event.mouseButton == 0) {
                dragging = false;
            } else if (event.type == UIEventType::MouseMove && dragging) {
                setOffset(event.mousePos - dragOffset);
				markChildrenDirty();
                return;
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

    // --- Dragging state ---
    bool dragging = false;
    sf::Vector2f dragOffset; // Mouse offset from top-left of root when drag starts

    std::function<void(UIRoot&)> onTick;
};
