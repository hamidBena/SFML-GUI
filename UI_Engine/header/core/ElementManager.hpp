#pragma once

#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include "core/UIElement.hpp"
#include "utils/Interpolation.hpp"

#include "../widgets/container/UIRoot.hpp"
#include "../widgets/container/UIList.hpp"

#include "../widgets/UIButton.hpp"
#include "../widgets/UILabel.hpp"
#include "../widgets/UITextField.hpp"
#include "../widgets/UISlider.hpp"
#include "core/UIEvent.hpp"

class GUI {
public:
    std::shared_ptr<UIRoot> CreateRoot();
    std::shared_ptr<UIList> CreateList();

    std::shared_ptr<UIButton> CreateButton();
    std::shared_ptr<UILabel> CreateLabel();
    std::shared_ptr<UITextField> CreateTextField();
    std::shared_ptr<UISlider> CreateSlider();

    std::shared_ptr<UIElement> GetElementByName(const std::string& name);

    void RemoveElementByName(const std::string& name);

	void draw(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default);

    void HandleEvent(const UIEvent& event);

	void Update(const float dt);
	void ProcessEvent(const sf::Event& event);

	void RefreshLayout() {
		for (auto& root : UIRoots) {
			root->CalculateLayout();
		}
	}

	void AddRoot(std::shared_ptr<UIRoot> root) {
		UIRoots.push_back(std::move(root));
	}

	const std::vector<std::shared_ptr<UIRoot>>& GetRoots() const {
		return UIRoots;
	}


private:
    std::vector<std::shared_ptr<UIRoot>> UIRoots;

    std::shared_ptr<UIElement> FindElementRecursive(const std::shared_ptr<UIElement>& element, const std::string& name);
};
