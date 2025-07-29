#include "UILibrary.hpp"
#include "SFML/Graphics.hpp"
#include <iostream>
#include <sstream>
#include <cmath>


int main() {
    // Create a window
    sf::RenderWindow window(sf::VideoMode(2000, 1200), "SFML Example", sf::Style::Default);
    window.setFramerateLimit(100);

    std::string synced_string;
	float loading_value = 0.f;

    GUI UI;
    auto Menu1 = UI.CreateRoot();
    Menu1->setOffset({200, 100});

	auto list1 = UI.CreateList();
	list1->setWidth(2);

    auto InputField = UI.CreateTextField();

	auto Button = UI.CreateButton();

	auto ChkBx = UI.CreateUICheckBox();
	ChkBx->setLabel("Toggle");

	Button->setOnHeld([&loading_value](UIElement& element, const float& duration){
		loading_value = duration;
	});

	auto Label = UI.CreateLabel();
	Label->setRoundingDecimals(2);

	auto Slider = UI.CreateSlider();
	Slider->setBoundValue(&loading_value)
	.setRange(0,10);
	
	//auto CheckBox = UI.CreateUICheckBox();
	
	auto PrgBar = UI.CreateUIProgressBar();
	PrgBar->setBoundValue(&loading_value)
	.setRange(0, 13);

	list1->AddChild(Slider);
	list1->AddChild(PrgBar);
	list1->AddChild(Button);
	list1->AddChild(Label);
	list1->AddChild(InputField);
	list1->AddChild(ChkBx);

	Menu1->AddChild(list1);

    sf::Clock clock;
	UI.Update(0);
	float layoutTime;
	std::string output;

	ChkBx->setOnToggle([&list1](auto& element, bool& check){
		list1->setHorizontal(check);
	});

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
                
            if (event.type == sf::Event::Resized) {
				sf::View view = window.getView();
				view.setSize(event.size.width, event.size.height); // resize the view to match window size
				view.setCenter(view.getSize().x / 2, view.getSize().y / 2); // keep center in bounds
				window.setView(view);
            }
            UI.HandleEvent(event, window);
        }

		Label->setLabel(output);

        float dt = clock.restart().asSeconds();
        UI.Update(dt);
		layoutTime = clock.getElapsedTime().asMicroseconds();
		output = "Time: " + std::to_string(layoutTime) + " micro s\n";

        window.clear({80, 80, 80});
        UI.draw(window);
        window.display();
    }

    return 0;
}