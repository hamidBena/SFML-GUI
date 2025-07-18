#include "core/UIElement.hpp"

UIElement::UIElement(const std::string& id)
    : id(id) {
    ElementCount++;
}

UIContainer::UIContainer(const std::string& id)
    : UIElement(id) {}

void UIContainer::Render(sf::RenderTarget& target, sf::RenderStates states) {
	if(!enabled) return;

	sf::View oldView = target.getView();
    // Set to default view (screen-space)	
	target.setView(target.getDefaultView());

    DrawSelf(target, states);
    for (const auto& child : children) {
        child->Render(target, states);
    }

	// Restore the previous view (world-space)
    target.setView(oldView);
}

UIElement* UIContainer::AddChild(std::shared_ptr<UIElement> child) {
    children.push_back(child);
    child->parent = shared_from_this();
    return child.get();
}

void UIContainer::HandleEvent(const UIEvent& event) {
    for (auto& child : children) {
        child->HandleEvent(event);
    }
}