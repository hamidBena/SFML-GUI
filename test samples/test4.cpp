#include "UILibrary.hpp"
#include "SFML/Graphics.hpp"
#include <iostream>
#include <sstream>

int main() {
    // Create a window
    sf::RenderWindow window(sf::VideoMode(2000, 1200), "SFML Example");
    window.setFramerateLimit(100);

    sf::View view(sf::FloatRect(0, 0, 2000, 1200));  // <--- Add this
    window.setView(view);                            // <--- And this

    std::string synced_string;

    GUI UI;
    auto Menu1 = UI.CreateRoot();
    Menu1->setOffset({800, 100})
         .setPadding({10, 15})
         .setSize({500, 500})
         .setLayoutType(LayoutType::Static)
         .setSizeType(SizeType::Absolute)
         .setHeaderTitle("Main Menu");

    auto List1 = UI.CreateList();
    List1->setOffset({0, 30})
         .setSize({400, 400})
         .setPadding({10, 10})
         .setLayoutType(LayoutType::Relative)
         .setSizeType(SizeType::FitContent)
         .setHeaderTitle("List1")
         .setHeaderHeight(30.f)
         .setSpacing(20.f);

    auto InputField = UI.CreateTextField();
    InputField->setSizeType(SizeType::FitContent)
              .setPlaceholder("name here...")
              .setBoundValue(&synced_string);

	auto Button = UI.CreateButton();
	auto Label = UI.CreateLabel();
	Label->setText("hello!\nthis is a label\nwith support for multiline + dynamic sizing!");
	auto Slider = UI.CreateSlider();

	auto CheckBox = UI.CreateUICheckBox();

    List1->AddChild(InputField);
	List1->AddChild(CheckBox);
	List1->AddChild(Button);
	List1->AddChild(Slider);
	List1->AddChild(Label);
    Menu1->AddChild(List1);
    
    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
                
            if (event.type == sf::Event::Resized) {
                view.setSize(static_cast<float>(event.size.width), static_cast<float>(event.size.height));
                view.setCenter(view.getSize() / 2.f);
                window.setView(view);
				Menu1->markChildrenDirty();
            }

            UI.ProcessEvent(event);
        }

        float dt = clock.restart().asSeconds();
        UI.Update(dt);

        window.clear({80, 80, 80});
        UI.draw(window);
        window.display();
    }

    return 0;
}
