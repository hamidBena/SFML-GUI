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
		   .setSpacing(20.f);

	auto InputField = UI.CreateTextField();
	InputField->setSizeType(SizeType::FitContent)
				.setPlaceholder("name here...")
				.setPlaceholderColor(sf::Color(100, 100, 100));

	auto InputField2 = UI.CreateTextField();
	InputField2->setSizeType(SizeType::FitContent)
				.setPlaceholder("name here... 2")
				.setPlaceholderColor(sf::Color(100, 100, 100));

	List1->AddChild(InputField);
	List1->AddChild(InputField2);

	Menu1->AddChild(List1);
	
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