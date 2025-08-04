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