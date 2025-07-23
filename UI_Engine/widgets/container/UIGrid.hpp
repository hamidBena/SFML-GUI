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
    UIGrid(const std::string& name = defaultName()) : UIContainer(name) {
		e_fillcolor = sf::Color(70, 70, 73, 230);
		borderColor = sf::Color(58, 58, 60, 180);
		headerColor = sf::Color(90, 190, 90, 255);

		headerTitle = "Root grid";
	}

    // Builder setters
    UIGrid& setOffset(const sf::Vector2f& pos) {
        e_offset = pos;
		interpolated_position = pos;
		markLayoutDirty();
        return *this;
    }
    UIGrid& setSize(const sf::Vector2f& size) {
        e_size = size;
		intr_size.setValue(e_size);
		markLayoutDirty();
        return *this;
    }
    UIGrid& setFillColor(const sf::Color& color) {
        e_fillcolor = color;
        return *this;
    }
    UIGrid& setAnchor(LayoutAnchor anch) {
        anchor = anch;
        return *this;
    }
    UIGrid& setLayoutType(LayoutType type) {
        layoutType = type;
		markLayoutDirty();
        return *this;
    }
	UIGrid& setSizeType(SizeType type) {
		sizeType = type;
		markLayoutDirty();
		return *this;
	}
	UIGrid& setPadding(const sf::Vector2f& pad) {
		e_padding = pad;
		markLayoutDirty();
		return *this;
	}
	UIGrid& setHeaderTitle(const std::string& title) {
        headerTitle = title;
        return *this;
    }
    UIGrid& setHeaderColor(const sf::Color& color) {
        headerColor = color;
        return *this;
    }
    UIGrid& setHeaderHeight(float height) {
        headerHeight = height;
		markLayoutDirty();
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
	UIGrid& setWidth(unsigned int wd) {
		columns = wd;
		markLayoutDirty();
		return *this;
	}
	UIGrid& setSpacing(float sp) {
		spacing = sp;
		markLayoutDirty();
		return *this;
	}

	UIGrid& setOnTick(std::function<void(UIGrid&)> cb) {
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
		auto shapes = buildShapes();
		target.draw(shapes.background, states);
		if (shapes.headerBar)   target.draw(*shapes.headerBar, states);
		if (shapes.headerText)  target.draw(*shapes.headerText, states);
		if (shapes.debugTriangle) target.draw(*shapes.debugTriangle, states);
		target.draw(shapes.toggleButton, states);
    }

    void CalculateLayout() override {
		if(!layoutDirty ||!enabled) return;
		layoutDirty = false;

		auto shapes = buildShapes();
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

		sf::Vector2f cellSize(0, 0);

		// Step 1: Measure max cell size (if dynamic)
		for (const auto& child : children) {
			if (!child->enabled) continue;

			child->CalculateLayout();  // Ensure child has up-to-date size
			cellSize.x = std::max(cellSize.x, child->e_size.x);
			cellSize.y = std::max(cellSize.y, child->e_size.y);
		}

		// Step 2: Position children in grid
		int i = 0;
		for (const auto& child : children) {
			if (!child->enabled) continue;

			int row = i / columns;
			int col = i % columns;

			sf::Vector2f pos;
			pos.x = e_position.x + e_padding.x + col * (cellSize.x + spacing);
			pos.y = e_position.y + e_padding.y + row * (cellSize.y + spacing);

			child->e_position = pos;
			++i;
		}

		// Step 3: Size this container (if FitContent)
		if (sizeType == SizeType::FitContent) {
			sf::Vector2f newsize = shapes.headerText->getLocalBounds().getSize() + e_padding + sf::Vector2f(40,0);

			int rowCount = (children.size() + columns - 1) / columns;
			newsize.x = std::max(newsize.x, columns * cellSize.x + (columns - 1) * spacing + e_padding.x * 2.0f);
			newsize.y = std::max(newsize.y, rowCount * cellSize.y + (rowCount - 1) * spacing + e_padding.y * 2.0f);
			e_size = newsize;
		}

		intr_size.setValue(e_size);
    }

    UIElement* AddChild(std::shared_ptr<UIElement> child) override {
        children.push_back(child);
        child->parent = shared_from_this();
		markLayoutDirty();
        return child.get();
    }

    void HandleEvent(const UIEvent& event) override {
		if (!enabled) return;
		auto shapes = buildShapes();
        // dragging by header (now above the root)
        if (headerHeight > 0.f) {
            sf::FloatRect headerbounds = shapes.headerBar->getGlobalBounds();
            sf::FloatRect togglebounds = shapes.toggleButton.getGlobalBounds();
            if (event.type == UIEventType::MouseDown && event.mouseButton == 0) {
				if (togglebounds.contains(event.mousePos)){
					for(auto& child : children){
						child->enabled = !child->enabled;
					}
					markLayoutDirty();
					return;
				}
            }else if (event.type == UIEventType::MouseMove) {
				if(togglebounds.contains(event.mousePos)){
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
	unsigned int columns = 1;
	float spacing = 10;

    // --- Dragging state ---
	bool is_hovered = false;

    std::function<void(UIGrid&)> onTick;

	Interpolated<sf::Vector2f> intr_size;

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

		// --- Debug Triangle ---
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

