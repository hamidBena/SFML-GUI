#include "UILibrary.hpp"
#include "SFML/Graphics.hpp"
#include <iostream>
#include <sstream>

int main() {
    // Create a window
    sf::RenderWindow window(sf::VideoMode(2000, 1200), "SFML Example");
	window.setFramerateLimit(100);

	float counter = 10;
	std::string counter_string;

	GUI UI;
	auto Menu1 = UI.CreateRoot();
	Menu1->setOffset({800, 100})
		   .setPadding({10, 15})
		   .setSize({500, 500})
		   .setFillColor({250,250,50,50})
		   .setLayoutType(LayoutType::Static)
		   .setSizeType(SizeType::FitContent)
		   .setHeaderTitle("Main Menu");


	auto List1 = UI.CreateList();
	List1->setOffset({0, 30})
		   .setSize({400, 400})
		   .setPadding({10, 10})
		   .setFillColor({250,100,50,50})
		   .setLayoutType(LayoutType::Relative)
		   .setSizeType(SizeType::FitContent)
		   .setHeaderTitle("List1")
		   .setHeaderHeight(30.f)
		   .setHeaderColor(sf::Color(200, 200, 200, 150))
		   .setSpacing(50.f);

	auto InputField = UI.CreateTextField();
	InputField->setSizeType(SizeType::Absolute)
				.setPlaceholder("name here...")
				.setPlaceholderColor(sf::Color(100, 100, 100))
				.setBoundValue(&counter_string)
				.setOnChange([&counter](const std::string& value) {
					try{
						counter = std::stof(value);
					} catch (const std::invalid_argument&){}
				});

	auto Slider1 = UI.CreateSlider();
	Slider1->setSize({170, 40})
		   .setRange(0.f, 50.f)
		   .setStep(0.1f)
		   .setShowValue(false)
		   .setFillColor({100, 100, 50, 255})
		   .setBoundValue(&counter);

	// Add the elements to the Menu
	Menu1->AddChild(List1);	//size and position errors may appear when adding the parent before linking its children
	List1->AddChild(InputField);
	List1->AddChild(Slider1);

	

	auto List2 = UI.CreateList();
	List2->setOffset({0, 30})
		   .setSize({400, 400})
		   .setPadding({10, 10})
		   .setFillColor({200,100,250,50})
		   .setLayoutType(LayoutType::Relative)
		   .setSizeType(SizeType::FitContent)
		   .setHeaderTitle("List2")
		   .setHeaderHeight(30.f)
		   .setHeaderColor(sf::Color(200, 200, 200, 150))
		   .setSpacing(10.f);

	auto Button1 = UI.CreateButton();
	Button1->setFillColor({150,250,50,200})
		   .setLabel("Increment")
		   .setOnClick([&counter, &counter_string, &UI, &List2](){
			counter+= 5;
			counter_string = std::to_string(counter);
			List2->setSpacing(counter);
			UI.RefreshLayout();
		   });

	auto Button2 = UI.CreateButton();
	Button2->setFillColor({255,80,50,200})
		   .setLabel("Decrement")
		   .setOnClick([&counter, &counter_string, &UI, &List2](){
			counter-= 5;
			counter_string = std::to_string(counter);
			List2->setSpacing(counter);
			UI.RefreshLayout();
		   });

	auto Button3 = UI.CreateButton();
	Button3->setFillColor({250,250,50,200})
		   .setLabel("update")
		   .setOnClick([&counter, &List2, &UI](){
			List2->setSpacing(counter);
			UI.RefreshLayout();
		   });

	List2->AddChild(Button1);
	List2->AddChild(Button2);
	List2->AddChild(Button3);

	auto Menu2 = UI.CreateRoot();
	Menu2->setOffset({200, 100})
		   .setPadding({10, 15})
		   .setSize({500, 500})
		   .setFillColor({0,250,250,50})
		   .setLayoutType(LayoutType::Static)
		   .setSizeType(SizeType::FitContent)
		   .setHeaderTitle("second Menu");
	Menu2->AddChild(List2);

	InputField->setOnEnter([&Menu2, &UI](const std::string& value) {
		if(value == "On") Menu2->setEnable(true);
		else if(value == "Off") Menu2->setEnable(false);
		else if(value == "Toggle") {
			Menu2->setEnable(!Menu2->enabled);
		}
		UI.RefreshLayout();
	});

	UI.RefreshLayout();	//new function to ensure layout is correctly placed

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