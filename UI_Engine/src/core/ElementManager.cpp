#include "core/ElementManager.hpp"
#include "core/UIEvent.hpp"

std::shared_ptr<UIRoot> GUI::CreateRoot() {
    std::shared_ptr<UIRoot> root;
    root = std::make_shared<UIRoot>();
    UIRoots.push_back(root);
    return root;
}

std::shared_ptr<UIList> GUI::CreateList() {
    std::shared_ptr<UIList> list;
    list = std::make_shared<UIList>();
    return list;
}

std::shared_ptr<UIGrid> GUI::CreateGrid() {
    std::shared_ptr<UIGrid> Grid;
    Grid = std::make_shared<UIGrid>();
    return Grid;
}


std::shared_ptr<UIButton> GUI::CreateButton() {
	auto button = std::make_shared<UIButton>();
	return button;
}

std::shared_ptr<UILabel> GUI::CreateLabel() {
	auto label = std::make_shared<UILabel>();
	return label;
}

std::shared_ptr<UITextField> GUI::CreateTextField() {
	auto TextField = std::make_shared<UITextField>();
	return TextField;
}

std::shared_ptr<UISlider> GUI::CreateSlider() {
	auto Slider = std::make_shared<UISlider>();
	return Slider;
}

std::shared_ptr<UICheckBox> GUI::CreateUICheckBox() {
	auto Chkbx = std::make_shared<UICheckBox>();
	return Chkbx;
}

std::shared_ptr<UIProgressBar> GUI::CreateUIProgressBar() {
	auto PrgsBr = std::make_shared<UIProgressBar>();
	return PrgsBr;
}

void GUI::RemoveElementByName(const std::string& name) {
    UIRoots.erase(
        std::remove_if(UIRoots.begin(), UIRoots.end(),
            [&name](const std::shared_ptr<UIRoot>& element) {
                return element->id == name;
            }),
        UIRoots.end()
    );
}

void GUI::draw(sf::RenderTarget& target, sf::RenderStates states){
	sf::View oldView = target.getView();
	sf::Vector2u winSize = target.getSize();
	sf::View uiView = sf::View(sf::FloatRect(0, 0, static_cast<float>(winSize.x), static_cast<float>(winSize.y)));

	target.setView(uiView);

	for(auto& root : UIRoots) root->Render(target, states);
	// Restore the previous view (world-space)
    target.setView(oldView);
}

void GUI::HandleEvent(const UIEvent& event) {
    for (auto& root : UIRoots) {
        root->HandleEvent(event);
    }
}

void GUI::Update(const float dt) {
	for (auto& root : UIRoots) {
		while(root->layoutDirty) root->CalculateLayout();
		root->Update(dt);
	}
}

void GUI::ProcessEvent(const sf::Event& event) {
    if (event.type == sf::Event::MouseMoved) {
        UIEvent uievt{UIEventType::MouseMove, sf::Vector2f(event.mouseMove.x, event.mouseMove.y)};
        HandleEvent(uievt);
    } else if (event.type == sf::Event::MouseButtonPressed) {
        UIEvent uievt{UIEventType::MouseDown, sf::Vector2f(event.mouseButton.x, event.mouseButton.y), event.mouseButton.button};
        HandleEvent(uievt);
    } else if (event.type == sf::Event::MouseButtonReleased) {
        UIEvent uievt{UIEventType::MouseUp, sf::Vector2f(event.mouseButton.x, event.mouseButton.y), event.mouseButton.button};
        HandleEvent(uievt);
    } else if (event.type == sf::Event::KeyPressed) {
        UIEvent uievt{UIEventType::KeyDown, sf::Vector2f(0, 0), 0, event.key.code};
		uievt.ctrl  = event.key.control;
		uievt.shift = event.key.shift;
		uievt.alt   = event.key.alt;
        HandleEvent(uievt);
    } else if (event.type == sf::Event::KeyReleased) {
        UIEvent uievt{UIEventType::KeyUp, sf::Vector2f(0, 0), 0, event.key.code};
		uievt.ctrl  = event.key.control;
		uievt.shift = event.key.shift;
		uievt.alt   = event.key.alt;
        HandleEvent(uievt);
    } else if (event.type == sf::Event::TextEntered) {
        if (event.text.unicode >= 32 && event.text.unicode < 127) {
            UIEvent uievt{UIEventType::TextEntered, sf::Vector2f(0, 0), 0, 0, static_cast<char>(event.text.unicode)};
            HandleEvent(uievt);
        }
    }
}