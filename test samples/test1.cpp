#include "UILibrary.hpp"
#include "SFML/Graphics.hpp"
#include <iostream>
#include <sstream>

int main() {
    // Create a window
    sf::RenderWindow window(sf::VideoMode(2000, 1200), "SFML gui");
	window.setFramerateLimit(100);
	float counter = 0;
	std::string counter_string;

	GUI UI;	//iniotialize the GUI manager

	//create a root container
	auto Menu1 = UI.CreateRoot();
	Menu1->setOffset({800, 100})
		   .setPadding({10, 15})
		   .setSize({500, 500})
		   .setFillColor({250,250,50,50})
		   .setLayoutType(LayoutType::Static)
		   .setSizeType(SizeType::FitContent)
		   .setHeaderTitle("Main Menu");

	//create UI elements using the GUI manager
	auto Button1 = UI.CreateButton();
	Button1->setOffset({0, 80})
		   .setSize({150, 50})
		   .setFillColor({150,250,50,200})
		   .setLayoutType(LayoutType::Relative)
		   .setSizeType(SizeType::Absolute)
		   .setTextSize(24)
		   .setLabel("Increment");

	auto Label1 = UI.CreateLabel();
	Label1->setOffset({0, 0})
		   .setSize({200, 50})
		   .setFillColor({250,250,250,100})
		   .setLayoutType(LayoutType::Relative)
		   .setSizeType(SizeType::FitContent)
		   .setTextSize(24)
		   .setTextColor(sf::Color::Black)
		   .setPadding({5, 2})
		   .setFont(AssetManager::get().getFont("assets/fonts/arial.ttf"))
		   .setText("Hello World!\nthis is a label");

	auto InputField = UI.CreateTextField();
	InputField->setOffset({0, 150})
				.setPlaceholder("name here...")
				.setPlaceholderColor(sf::Color(100, 100, 100))
				.setBoundValue(&counter_string)
				.setOnChange([&counter](const std::string& value) {
					try{
						counter = std::stof(value);
					} catch (const std::invalid_argument&){}
				});

	auto Slider1 = UI.CreateSlider();
	Slider1->setOffset({0, 250})
		   .setSize({250, 40})
		   .setFillColor(sf::Color::White)
		   .setBorder(2, sf::Color::Black)
		   .setLayoutType(LayoutType::Relative)
		   .setSizeType(SizeType::Absolute)
		   .setRange(0.f, 100.f)
		   .setStep(0.1f)
		   .setShowValue(false)
		   .setBoundValue(&counter)
		   .setOnChange([&counter_string](float value) {
		       counter_string = std::to_string(value);
		   });

	// example of using the bound value to update the counter
	Button1->setOnClick([&counter, &counter_string](){
		counter++;
		counter_string = std::to_string(counter);
		});

	// Add the elements to the Menu
	Menu1->AddChild(Button1);
	Menu1->AddChild(Label1);
	Menu1->AddChild(InputField);
	Menu1->AddChild(Slider1);

	sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
				
			UI.ProcessEvent(event);
		}

		float dt = clock.restart().asSeconds();
		UI.Update(dt);

        window.clear({150,150,150});
        UI.draw(window);
        window.display();
    }

    return 0;
}