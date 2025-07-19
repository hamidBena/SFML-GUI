#pragma once

#include "core/UIElement.hpp"
#include <SFML/Graphics/RectangleShape.hpp>
#include <functional>

class UIList : public UIContainer {
public:
    UIList(const std::string& name = defaultName()) : UIContainer(name) {}

    // Builder setters
    UIList& setOffset(const sf::Vector2f& pos) {
        e_offset = pos;
		markLayoutDirty();
        return *this;
    }
    UIList& setSize(const sf::Vector2f& size) {
        e_size = size;
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

	UIList& setOnTick(std::function<void(UIList&, const float)> cb) { onTick = std::move(cb); return *this; }

	void Update(const float dt) override {
		if(!enabled) return;
		for (auto& child : children) {
			child->Update(dt);
		}
		if(onTick) onTick(*this, dt);
	}

    void DrawSelf(sf::RenderTarget& target, sf::RenderStates states) override {
		if(!visible) return;

        // main background
        sf::RectangleShape rect(e_size);
        rect.setPosition(e_position);
        rect.setFillColor(e_fillcolor);
        rect.setOutlineColor(sf::Color::Black);
        rect.setOutlineThickness(2.f);
        target.draw(rect, states);

        // header bar
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
		if(!layoutDirty) return;
		layoutDirty = false;

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

		interpolated_position = e_position;

		float currentY;
        if (sizeType == SizeType::FitContent) {
			currentY = e_position.y + e_padding.y;
			for (auto& child : children) {
				child->CalculateLayout();
				child->e_position.y = currentY;
				currentY += child->e_size.y + spacing; // Stack children vertically with spacing
			}

            sf::Vector2f maxSize(0, 0);
            for (const auto& child : children) {
                sf::Vector2f childBR = child->e_position + child->e_size - e_position; // bottom-right relative to root
                maxSize.x = std::max(maxSize.x, childBR.x);
                maxSize.y = std::max(maxSize.y, childBR.y);
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

		currentY = e_position.y + e_padding.y;
        for (auto& child : children) {
            child->CalculateLayout();

			child->e_position.y = currentY;
			currentY += child->e_size.y + spacing;
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

    std::function<void(UIList&, const float dt)> onTick;
};
