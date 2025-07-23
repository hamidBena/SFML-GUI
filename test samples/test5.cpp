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

    auto Grid1 = UI.CreateGrid();
    Grid1->setOffset({0, 30})
	.setWidth(2);

    auto InputField = UI.CreateTextField();
    InputField->setBoundValue(&synced_string);

	auto Button = UI.CreateButton();
	Button->setLabel("Increment")
	.setOnClick([&loading_value, &Grid1](){
		loading_value += 1.f;
		if(loading_value > 20.f){
			loading_value = 5.f;
		} 
		Grid1->setSpacing(loading_value);
	});

	auto Label = UI.CreateLabel();
	Label->setText("hello!\nthis is a label");

	auto Slider = UI.CreateSlider();
	Slider->setBoundValue(&loading_value)
	.setRange(5.f, 20.f);

	auto CheckBox = UI.CreateUICheckBox();

	auto PrgBar = UI.CreateUIProgressBar();
	PrgBar->setBoundValue(&loading_value)
	.setRange(0.f, 20.f);

    Grid1->AddChild(InputField);
	Grid1->AddChild(CheckBox);
	Grid1->AddChild(Button);
	Grid1->AddChild(Label);
	Grid1->AddChild(Slider);
	Grid1->AddChild(PrgBar);
    Menu1->AddChild(Grid1);


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