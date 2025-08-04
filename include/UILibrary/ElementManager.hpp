#pragma once

#include <vector>
#include <memory>
#include <string>
#include <algorithm>
//#include "UILibrary/core/UIElement.hpp"
//#include "utils/Interpolation.hpp"
//
//#include "../widgets/container/UIRoot.hpp"
//#include "../widgets/container/UIDragable.hpp"
//#include "../widgets/container/UIList.hpp"
//
//#include "../widgets/UIButton.hpp"
//#include "../widgets/UILabel.hpp"
//#include "../widgets/UITextField.hpp"
//#include "../widgets/UISlider.hpp"
//#include "../widgets/UICheckBox.hpp"
//#include "UILibrary/widgets/UIProgressBar.hpp"
//#include "../include/UILibrary/"

class GUI {
public:
	// container creation
    std::shared_ptr<UIRoot> CreateRoot();
    std::shared_ptr<UIList> CreateList();
    std::shared_ptr<UIDraggable> CreateDraggableMenu();
	//widget creation
    std::shared_ptr<UIButton> CreateButton();
    std::shared_ptr<UILabel> CreateLabel();
    std::shared_ptr<UITextField> CreateTextField();
    std::shared_ptr<UISlider> CreateSlider();
    std::shared_ptr<UICheckBox> CreateUICheckBox();
    std::shared_ptr<UIProgressBar> CreateUIProgressBar();

	//UI control entry points
	void draw(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default){
		sf::View oldView = target.getView();
		sf::Vector2u winSize = target.getSize();
		sf::View uiView = sf::View(sf::FloatRect(0, 0, static_cast<float>(winSize.x), static_cast<float>(winSize.y)));

		target.setView(uiView);

		for(auto& container : UIContainers){
			container->CalculateLayout();
			container->Render(target);
		}
		
		target.setView(oldView);
	}
    void PassEvent(const UIEvent& event){
		for (auto& container : UIContainers) {
			container->HandleEvent(event);
		}
	}
	void Update(const float dt) {
		for (auto& container : UIContainers) {
			container->Update(dt);
		}
	}

	void HandleEvent(const sf::Event& event, const sf::RenderWindow& window){
		auto mappedEvent = mapSFEvent(event, window);
		if(mappedEvent) PassEvent(*mappedEvent);
	}
	
private:
	std::optional<UIEvent> mapSFEvent(const sf::Event& sfe, const sf::RenderWindow& window) {
		switch (sfe.type) {
			case sf::Event::MouseMoved:
				return UIEvent{ UIEventType::MouseMove, MouseEventData{ sf::Vector2f(sf::Mouse::getPosition(window)), 0 } };

			case sf::Event::MouseButtonPressed:
				return UIEvent{ UIEventType::MouseDown, MouseEventData{ sf::Vector2f(sf::Mouse::getPosition(window)), sfe.mouseButton.button } };

			case sf::Event::MouseButtonReleased:
				return UIEvent{ UIEventType::MouseUp, MouseEventData{ sf::Vector2f(sf::Mouse::getPosition(window)), sfe.mouseButton.button } };

			case sf::Event::TextEntered:
				return UIEvent{ UIEventType::TextEntered, TextEventData{ static_cast<char>(sfe.text.unicode) } };

			case sf::Event::KeyPressed:
				return UIEvent{ UIEventType::KeyDown, KeyEventData{
					sfe.key.code,
					sfe.key.control,
					sfe.key.shift,
					sfe.key.alt
				}};
			case sf::Event::KeyReleased:
				return UIEvent{ UIEventType::KeyUp, KeyEventData{
					sfe.key.code,
					sfe.key.control,
					sfe.key.shift,
					sfe.key.alt
				}};

			default:
				return std::nullopt;
		}
	}

    std::vector<std::shared_ptr<UIContainer>> UIContainers;
};
