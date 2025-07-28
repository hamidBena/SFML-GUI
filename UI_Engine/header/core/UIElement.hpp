#pragma once

#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <SFML/Graphics.hpp>
#include "utils/assetManager.hpp"
#include "utils/Interpolation.hpp"
#include "core/UIEvent.hpp"

// Layout enums
enum class LayoutType { Static, Relative, Percent };
enum class SizeType { Absolute, FitContent, FillParent, Percent };

class UIElement : public std::enable_shared_from_this<UIElement> {
public:
    inline static int ElementCount = 0;
	//layout
	sf::Vector2f e_position = {0, 0};			Interpolated<sf::Vector2f> intr_position;
    sf::Vector2f e_offset   = {0, 0};
    sf::Vector2f e_size     = {150, 50};		Interpolated<sf::Vector2f> intr_size;
	sf::Vector2f e_padding = {5, 5};
    sf::Color e_fillcolor = sf::Color::White;
    sf::Color e_borderColor = sf::Color::Black;
	float e_borderThickness = 2.f;

	sf::Vector2f accumulatedOffset = {0, 0};
	virtual void updateAccumulatedOffset(){accumulatedOffset = e_position + e_padding;}

	const sf::Vector2f& getPosition() const { return e_position; }
	const sf::Vector2f& getOffset() const 	{ return e_offset; }
	const sf::Vector2f& getSize() const 	{ return e_size; }
	const sf::Vector2f& getPadding() const 	{ return e_padding; }
	const sf::FloatRect getBounds() const 	{ return sf::FloatRect(e_position, e_size); }
	const sf::Color& getFillColor() const	{ return e_fillcolor; }
	const sf::Color& getBorderColor() const { return e_borderColor; }
	float getBorderThickness() const 		{ return e_borderThickness; }

	//state
	bool visible = true;
	bool enabled = true;

	//event states
	bool hovered = false;
	float hoverDuration; //for animations 
	const float getHoverDuation() const { return hoverDuration; }

	//layout config
    LayoutType layoutType = LayoutType::Relative;
	SizeType sizeType = SizeType::FitContent;

	//standard callbacks
	std::function<void(UIElement&, const float&)> onTick;
	std::function<void(UIElement&, const float&)> onHover;
	std::function<void(UIElement&)> onMouseEnter;
	std::function<void(UIElement&)> onMouseLeave;

	void setOnTick(std::function<void(UIElement&, const float&)> cb)    { onTick = std::move(cb); 		}
	void setOnHover(std::function<void(UIElement&, const float&)> cb)   { onHover = std::move(cb); 		}
	void setOnMouseEnter(std::function<void(UIElement&)> cb)			{ onMouseEnter = std::move(cb); }
	void setOnMouseLeave(std::function<void(UIElement&)> cb)			{ onMouseLeave = std::move(cb); }

	// hierarchy pointers
    std::weak_ptr<UIElement> parent;

	// override virtual functions
    virtual void UpdateWidget(const float dt) = 0;
    virtual void Render(sf::RenderTarget& target) = 0;
    virtual void DrawSelf(sf::RenderTarget& target) = 0;

	virtual void HandleWidgetEvent(const UIEvent& event) = 0;

    virtual void HandleEvent(const UIEvent& event){
		if(!enabled) return;

		if(event.type == UIEventType::MouseMove){
			if(auto* data = std::get_if<MouseEventData>(&event.data)){
				if(!hovered && getBounds().contains(data->pos)){
					hovered = true;
					if(onMouseEnter) onMouseEnter(*this);
				}else if(hovered && !getBounds().contains(data->pos)){
					hovered = false;
					if(onMouseLeave) onMouseLeave(*this);
				}
			}
		}
		HandleWidgetEvent(event);
	}

	virtual void Update(const float& dt){
		if(!enabled) return;

		if(onTick) onTick(*this, dt);

		if(hovered){
			hoverDuration += dt;
			if(onHover) onHover(*this, hoverDuration);
		}else{
			hoverDuration = 0;
		}

		UpdateWidget(dt);
	}

	//any override to this function must update the interpolated variables and update the parent's accumulated offset
	virtual void PositionPass(){
		switch(layoutType){
			case LayoutType::Relative:
				if (auto parentPtr = parent.lock()) { 
					parentPtr->updateAccumulatedOffset();
					e_position = parentPtr->accumulatedOffset + e_offset;
				}
				else { e_position = e_offset; }
				break;

			case LayoutType::Percent:
				if (auto parentPtr = parent.lock()) {
				sf::Vector2f parentSize = parentPtr->e_size - parentPtr->e_padding * 2.0f;
				e_position.x = parentPtr->e_position.x + parentPtr->e_padding.x + (e_size.x * (e_offset.x / 100.f));
				e_position.y = parentPtr->e_position.y + parentPtr->e_padding.y + (e_size.y * (e_offset.y / 100.f));
				}
				break;

			case LayoutType::Static:
				e_position = e_offset;
				break;
		}
		intr_position = e_position;
	}
	virtual void SizePass(){
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
			case SizeType::Absolute:
				break;
		}
		intr_size = e_size;
	}

	//any override to this function must update the accumulated offset and include some layoutDirty management
	virtual void CalculateLayout(){
		PositionPass();
		SizePass();
	}

	UIElement(){ ElementCount++; }
};

// Leaf type: cannot have children, only draws itself
class UILeaf : public UIElement {
public:
    void Render(sf::RenderTarget& target) override {
		if(!visible) return;

        DrawSelf(target);
    }
};

// Container type: can have children and manages layout
class UIContainer : public UIElement {
public:
    std::vector<std::shared_ptr<UIElement>> children;

    UIElement* AddChild(std::shared_ptr<UIElement> child) {
		children.push_back(child);
		child->parent = shared_from_this();
		return child.get();
	}

    void Render(sf::RenderTarget& target) override {
		if(visible) DrawSelf(target);

		if(childrenVisible){
			for (const auto& child : children) {
				child->Render(target);
			}
		}
	}

	void updateAccumulatedOffset() override {
		accumulatedOffset.x = e_position.x + e_padding.x;
		accumulatedOffset.y = e_position.y + e_padding.y + headerHeight*1.2;
	}

	//any override to this function must handle children update
	void UpdateWidget(const float dt) override {
		for (auto& child : children) {
			child->Update(dt);
		}
	}

	void CalculateLayout() override {
		PositionPass();
		if(sizeType == SizeType::FitContent){
			for (auto& child : children) {
				child->CalculateLayout();
			}
			SizePass();
		}else{
			SizePass();
			for (auto& child : children) {
				child->CalculateLayout();
			}
		}
	}

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

			case SizeType::FitContent: {
				UpdateHeaderSize();
				sf::Vector2f maxSize = headerSize + e_padding + sf::Vector2f(headerHeight,0);
				if(childrenVisible){
					for (const auto& child : children) {
						if(!child->enabled) continue;
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

		if(intr_size.end != e_size){
			intr_size = e_size;
		}
	}

	//this function should update the "headerSize" using the header's text from the child class
	//it serves to correctly calculate the fitcontent type when the menu is empty
	virtual void UpdateHeaderSize() = 0;

public:
	std::string headerTitle = "";
	float headerHeight = 30.f;		Interpolated<float> intr_headerHeight;
	sf::Color headerColor;
	sf::Color headerBorderColor;
	sf::Vector2f headerSize;

	sf::Font font = AssetManager::get().getFont("fonts/arial.ttf");
	bool childrenVisible = true;	//for menu functionality
};