#include "UILibrary.hpp"
#include "SFML/Graphics.hpp"
#include <iostream>
#include <sstream>

int main() {
    // Create a window
    sf::RenderWindow window(sf::VideoMode(2000, 1200), "SFML Example", sf::Style::Default);
    window.setFramerateLimit(100);

    std::string synced_string;
	float loading_value = 0.f;

    GUI UI;
    auto Menu1 = UI.CreateRoot();
    Menu1->setOffset({200, 100});

    auto List1 = UI.CreateList();
    List1->setOffset({100, 30});

    auto InputField = UI.CreateTextField();
    InputField->setBoundValue(&synced_string);

	auto Button = UI.CreateButton();
	Button->setLabel("Increment")
	.setOnClick([&loading_value, &List1](){
		loading_value += 1.f;
		if(loading_value > 20.f){
			loading_value = 5.f;
		} 
		List1->setSpacing(loading_value);
	});

	auto swap_button = UI.CreateButton();
	swap_button->setLabel("Swap")
	.setFillColor({150,160,0})
	.setOnClick([&List1](){
		List1->setHorizontal(!List1->isHorizontal());
	});

	auto Label = UI.CreateLabel();
	Label->setText("hello!\nthis is a label\nwith support for multiline + dynamic sizing!");

	auto Slider = UI.CreateSlider();
	Slider->setBoundValue(&loading_value)
	.setRange(5.f, 20.f);

	auto CheckBox = UI.CreateUICheckBox();

	auto PrgBar = UI.CreateUIProgressBar();
	PrgBar->setBoundValue(&loading_value)
	.setRange(0.f, 20.f);

    List1->AddChild(InputField);
	List1->AddChild(CheckBox);
	List1->AddChild(swap_button);
	List1->AddChild(Button);
	List1->AddChild(Slider);
	List1->AddChild(PrgBar);
	List1->AddChild(Label);
    Menu1->AddChild(List1);
    
	auto UI_en = UI.CreateUICheckBox();
	UI_en->setLabel("Enable UI")
	.setOnToggle([&List1](bool en){
		List1->setEnable(en);
	});

	Menu1->AddChild(UI_en);


    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
                
            if (event.type == sf::Event::Resized) {
				//window.create(sf::VideoMode(event.size.width, event.size.height), "SFML Example");
				Menu1->markChildrenDirty();

				sf::View view = window.getView();
				view.setSize(event.size.width, event.size.height); // resize the view to match window size
				view.setCenter(view.getSize().x / 2, view.getSize().y / 2); // keep center in bounds
				window.setView(view);
            }
            UI.ProcessEvent(event);
        }

        float dt = clock.restart().asSeconds();
        UI.Update(dt);

        window.clear({80, 80, 80});
        UI.draw(window);
		sf::CircleShape circle;
		circle.setFillColor(sf::Color::Red);
		circle.setRadius(100);
		window.draw(circle);
        window.display();
    }

    return 0;
}