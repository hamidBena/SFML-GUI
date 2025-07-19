#include "UILibrary.hpp"
#include "SFML/Graphics.hpp"
#include <iostream>
#include <sstream>

int main() {
    // Create a window
    sf::RenderWindow window(sf::VideoMode(2000, 1200), "SFML Example");
	window.setFramerateLimit(100);

	float counter;

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

	auto Slider1 = UI.CreateSlider();
	Slider1->setSize({170, 40})
		   .setRange(0.f, 50.f)
		   .setStep(0.1f)
		   .setShowValue(false)
		   .setFillColor({100, 100, 50, 255})
		   .setBoundValue(&counter);

	Menu1->AddChild(List1);
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
		   .setOnClick([&counter, &List2](){
			counter+= 5;
			List2->setSpacing(counter);
		   });

	auto Button2 = UI.CreateButton();
	Button2->setFillColor({255,80,50,200})
		   .setLabel("Decrement")
		   .setOnClick([&counter, &List2](){
			counter-= 5;
			List2->setSpacing(counter);
		   });

	auto Button3 = UI.CreateButton();
	Button3->setFillColor({250,250,50,200})
		   .setLabel("reset")
		   .setOnClick([&counter, &List2](){
			counter = 10;
			List2->setSpacing(counter);
		   });

	List2->AddChild(Button1);
	List2->AddChild(Button2);
	List2->AddChild(Button3);

	Slider1->setOnChange([&List2](float value){
		List2->setSpacing(value);
	});

	auto Menu2 = UI.CreateRoot();
	Menu2->setOffset({200, 100})
		   .setPadding({10, 15})
		   .setSize({500, 500})
		   .setFillColor({0,250,250,50})
		   .setLayoutType(LayoutType::Static)
		   .setSizeType(SizeType::FitContent)
		   .setHeaderTitle("second Menu");
	Menu2->AddChild(List2);

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