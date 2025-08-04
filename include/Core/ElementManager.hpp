#pragma once

#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <optional>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "../include/core/UIElement.hpp"
#include "../include/utils/Interpolation.hpp"

#include "../include/widgets/container/UIRoot.hpp"
#include "../include/widgets/container/UIList.hpp"

#include "../include/widgets/UIButton.hpp"
#include "../include/widgets/UILabel.hpp"
#include "../include/widgets/UITextField.hpp"
#include "../include/widgets/UISlider.hpp"
#include "../include/widgets/UICheckBox.hpp"
#include "../include/widgets/UIProgressBar.hpp"

#include "../include/core/UIEvent.hpp"

class GUI {
public:
    std::shared_ptr<UIRoot> CreateRoot();
    std::shared_ptr<UIList> CreateList();

    std::shared_ptr<UIButton> CreateButton();
    std::shared_ptr<UILabel> CreateLabel();
    std::shared_ptr<UITextField> CreateTextField();
    std::shared_ptr<UISlider> CreateSlider();
    std::shared_ptr<UICheckBox> CreateUICheckBox();
    std::shared_ptr<UIProgressBar> CreateUIProgressBar();

    void draw(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default);
    void PassEvent(const UIEvent& event);
    void Update(const float dt);
    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window);

private:
    std::optional<UIEvent> mapSFEvent(const sf::Event& sfe, const sf::RenderWindow& window);
    std::vector<std::shared_ptr<UIRoot>> UIRoots;
};
