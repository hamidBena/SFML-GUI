#include "../include/core/UIElement.hpp"

UIElement::UIElement() { ElementCount++; }

void UIElement::updateAccumulatedOffset() {
    accumulatedOffset = e_position + e_padding;
}

const sf::Vector2f& UIElement::getPosition() const { return e_position; }
const sf::Vector2f& UIElement::getOffset() const { return e_offset; }
const sf::Vector2f& UIElement::getSize() const { return e_size; }
const sf::Vector2f& UIElement::getPadding() const { return e_padding; }
const sf::FloatRect UIElement::getBounds() const { return sf::FloatRect(e_position, e_size); }
const sf::Color& UIElement::getFillColor() const { return e_fillcolor; }
const sf::Color& UIElement::getBorderColor() const { return e_borderColor; }
float UIElement::getBorderThickness() const { return e_borderThickness; }
const float UIElement::getHoverDuation() const { return hoverDuration; }

void UIElement::setOnTick(std::function<void(UIElement&, const float&)> cb) { onTick = std::move(cb); }
void UIElement::setOnHover(std::function<void(UIElement&, const float&)> cb) { onHover = std::move(cb); }
void UIElement::setOnMouseEnter(std::function<void(UIElement&)> cb) { onMouseEnter = std::move(cb); }
void UIElement::setOnMouseLeave(std::function<void(UIElement&)> cb) { onMouseLeave = std::move(cb); }

void UIElement::HandleEvent(const UIEvent& event) {
    if (!enabled) return;

    if (event.type == UIEventType::MouseMove) {
        if (auto* data = std::get_if<MouseEventData>(&event.data)) {
            if (!hovered && getBounds().contains(data->pos)) {
                hovered = true;
                if (onMouseEnter) onMouseEnter(*this);
            } else if (hovered && !getBounds().contains(data->pos)) {
                hovered = false;
                if (onMouseLeave) onMouseLeave(*this);
            }
        }
    }
    HandleWidgetEvent(event);
}

void UIElement::Update(const float& dt) {
    if (!enabled) return;

    if (onTick) onTick(*this, dt);

    if (hovered) {
        hoverDuration += dt;
        if (onHover) onHover(*this, hoverDuration);
    } else {
        hoverDuration = 0;
    }

    UpdateWidget(dt);
}

void UIElement::PositionPass() {
    switch (layoutType) {
        case LayoutType::Relative:
            if (auto parentPtr = parent.lock()) {
                parentPtr->updateAccumulatedOffset();
                e_position = parentPtr->accumulatedOffset + e_offset;
            } else {
                e_position = e_offset;
            }
            break;
        case LayoutType::Static:
            e_position = e_offset;
            break;
    }
    intr_position = e_position;
}

void UIElement::SizePass() {
    intr_size = e_size;
}

void UIElement::CalculateLayout() {
    PositionPass();
    SizePass();
}

void UIElement::Move(const sf::Vector2f& offset) {
    e_position += offset;
    intr_position = e_position;
}

void UILeaf::Render(sf::RenderTarget& target) {
    if (!visible) return;
    DrawSelf(target);
}

UIElement* UIContainer::AddChild(std::shared_ptr<UIElement> child) {
    children.push_back(child);
    child->parent = shared_from_this();
    return child.get();
}

void UIContainer::Render(sf::RenderTarget& target) {
    if (visible) DrawSelf(target);
    if (childrenVisible) {
        for (const auto& child : children)
            child->Render(target);
    }
}

void UIContainer::updateAccumulatedOffset() {
    accumulatedOffset.x = e_position.x + e_padding.x;
    accumulatedOffset.y = e_position.y + e_padding.y + headerHeight * 1.2f;
}

void UIContainer::UpdateWidget(const float dt) {
    for (auto& child : children)
        child->Update(dt);
}

void UIContainer::CalculateLayout() {
    PositionPass();
    if (sizeType == SizeType::FitContent) {
        for (auto& child : children)
            child->CalculateLayout();
        SizePass();
    } else {
        SizePass();
        for (auto& child : children)
            child->CalculateLayout();
    }
}

void UIContainer::SizePass() {
    switch (sizeType) {
        case SizeType::FitContent: {
            UpdateHeaderSize();
            sf::Vector2f maxSize = headerSize + e_padding + sf::Vector2f(headerHeight, 0);
            if (childrenVisible) {
                for (const auto& child : children) {
                    if (!child->enabled) continue;
                    child->SizePass();
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
    if (intr_size.end != e_size) {
        intr_size = e_size;
    }
}

void UIContainer::Move(const sf::Vector2f& offset) {
    e_position += offset;
    intr_position = e_position;
    for (auto& child : children)
        child->Move(offset);
}
