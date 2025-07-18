#pragma once

#include <vector>
#include <memory>
#include <string>
#include <SFML/Graphics.hpp>
#include "utils/assetManager.hpp"
#include "utils/Interpolation.hpp"
#include "core/UIEvent.hpp"

// Layout enums
enum class LayoutAnchor { TopLeft, TopRight, BottomLeft, BottomRight, Center };
enum class LayoutType { Static, Relative, Percent, Anchor };
enum class SizeType { Absolute, FitContent, FillParent, Percent };

class UIElement : public std::enable_shared_from_this<UIElement> {
public:
    inline static int ElementCount = 0;

	sf::Vector2f e_position = {0, 0};
    sf::Vector2f e_offset   = {0, 0};
    sf::Vector2f e_size     = {150, 50};
    sf::Color e_fillcolor = sf::Color::White;
    std::string id;

	bool visible = true;
	bool enabled = true;

    LayoutAnchor anchor = LayoutAnchor::TopLeft;
    LayoutType layoutType = LayoutType::Relative;
	SizeType sizeType = SizeType::Absolute;

    std::weak_ptr<UIElement> parent;

    sf::Vector2f e_padding = {0, 0};

    float borderThickness = 2.f;
    sf::Color borderColor = sf::Color::Black;

    UIElement(const std::string& id);

	sf::Vector2f getSize(){return e_size;}

    virtual UIElement* AddChild(std::shared_ptr<UIElement> child) { return nullptr; }
    virtual void CalculateLayout() = 0;
    virtual void Update(const float dt) = 0;

    virtual void Render(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) = 0;	// for drawing children
    virtual void DrawSelf(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) = 0;	// for drawing itself
    virtual void HandleEvent(const UIEvent& event) {};	// default empty event handler but may be overridden by derived classes
    virtual ~UIElement() {}

protected:
    static std::string defaultName() {
        return std::to_string(UIElement::ElementCount);
    }

	Interpolated<sf::Vector2f> interpolated_position;
};

// Container type: can have children and manages layout
class UIContainer : public UIElement {
public:
    std::vector<std::shared_ptr<UIElement>> children;
    float spacing = 0.f;

    UIContainer(const std::string& id);
    UIElement* AddChild(std::shared_ptr<UIElement> child) override;
    void Render(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) override;
    void HandleEvent(const UIEvent& event) override;
};

// Leaf type: cannot have children, only draws itself
class UILeaf : public UIElement {
public:
    UILeaf(const std::string& id) : UIElement(id) {}
    // Leaves cannot have children
    UIElement* AddChild(std::shared_ptr<UIElement> child) override { return nullptr; }

    void Render(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) override {
		
        DrawSelf(target, states);
    }
	
    void HandleEvent(const UIEvent& event) override {};
};