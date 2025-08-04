#include "../include/core/ElementManager.hpp"

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

void GUI::draw(sf::RenderTarget& target, sf::RenderStates states) {
    sf::View oldView = target.getView();
    sf::Vector2u winSize = target.getSize();
    sf::View uiView = sf::View(sf::FloatRect(0, 0, static_cast<float>(winSize.x), static_cast<float>(winSize.y)));

    target.setView(uiView);

    for (auto& root : UIRoots) root->Render(target);

    target.setView(oldView);
}

void GUI::PassEvent(const UIEvent& event) {
    for (auto& root : UIRoots) {
        root->HandleEvent(event);
    }
}

void GUI::Update(const float dt) {
    for (auto& root : UIRoots) {
        root->CalculateLayout();
        root->Update(dt);
    }
}

void GUI::HandleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    auto mappedEvent = mapSFEvent(event, window);
    if (mappedEvent) PassEvent(*mappedEvent);
}

std::optional<UIEvent> GUI::mapSFEvent(const sf::Event& sfe, const sf::RenderWindow& window) {
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
