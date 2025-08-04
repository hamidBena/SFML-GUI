#pragma once

#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <SFML/Graphics.hpp>
#include "utils/assetManager.hpp"
#include "utils/Interpolation.hpp"
#include "core/UIEvent.hpp"

enum class LayoutType { Static, Relative };
enum class SizeType { Absolute, FitContent };

class UIElement : public std::enable_shared_from_this<UIElement> {
public:
    inline static int ElementCount = 0;

    sf::Vector2f e_position = {0, 0};
    sf::Vector2f e_offset   = {0, 0};
    sf::Vector2f e_size     = {150, 50};
    sf::Vector2f e_padding  = {5, 5};

    Interpolated<sf::Vector2f> intr_position;
    Interpolated<sf::Vector2f> intr_size;

    sf::Color e_fillcolor = sf::Color::White;
    sf::Color e_borderColor = sf::Color::Black;
    float e_borderThickness = 2.f;

    sf::Vector2f accumulatedOffset = {0, 0};
    virtual void updateAccumulatedOffset();

    const sf::Vector2f& getPosition() const;
    const sf::Vector2f& getOffset() const;
    const sf::Vector2f& getSize() const;
    const sf::Vector2f& getPadding() const;
    const sf::FloatRect getBounds() const;
    const sf::Color& getFillColor() const;
    const sf::Color& getBorderColor() const;
    float getBorderThickness() const;

    bool visible = true;
    bool enabled = true;
    bool hovered = false;
    float hoverDuration = 0;
    const float getHoverDuation() const;

    LayoutType layoutType = LayoutType::Relative;
    SizeType sizeType = SizeType::FitContent;

    std::function<void(UIElement&, const float&)> onTick;
    std::function<void(UIElement&, const float&)> onHover;
    std::function<void(UIElement&)> onMouseEnter;
    std::function<void(UIElement&)> onMouseLeave;

    void setOnTick(std::function<void(UIElement&, const float&)> cb);
    void setOnHover(std::function<void(UIElement&, const float&)> cb);
    void setOnMouseEnter(std::function<void(UIElement&)> cb);
    void setOnMouseLeave(std::function<void(UIElement&)> cb);

    std::weak_ptr<UIElement> parent;

    virtual void UpdateWidget(const float dt) = 0;
    virtual void Render(sf::RenderTarget& target) = 0;
    virtual void DrawSelf(sf::RenderTarget& target) = 0;
    virtual void HandleWidgetEvent(const UIEvent& event) = 0;

    virtual void HandleEvent(const UIEvent& event);
    virtual void Update(const float& dt);

    virtual void PositionPass();
    virtual void SizePass();
    virtual void CalculateLayout();
    virtual void Move(const sf::Vector2f& offset);

    UIElement();
};

// Leaf widget, draws itself only
class UILeaf : public UIElement {
public:
    void Render(sf::RenderTarget& target) override;
};

// Container widget, holds children
class UIContainer : public UIElement {
public:
    std::vector<std::shared_ptr<UIElement>> children;

    UIElement* AddChild(std::shared_ptr<UIElement> child);

    void Render(sf::RenderTarget& target) override;
    void updateAccumulatedOffset() override;
    void UpdateWidget(const float dt) override;
    void CalculateLayout() override;
    void SizePass() override;
    void Move(const sf::Vector2f& offset) override final;

    virtual void UpdateHeaderSize() = 0;

public:
    std::string headerTitle = "";
    float headerHeight = 30.f;
    Interpolated<float> intr_headerHeight;
    sf::Color headerColor;
    sf::Color headerBorderColor;
    sf::Vector2f headerSize;

    sf::Font font = AssetManager::get().getFont("fonts/arial.ttf");
    bool childrenVisible = true;
};
