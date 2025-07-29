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
Menu1->setOffset({500, 200})
.setHeaderHeight(0);

// === First Column ===
auto list1 = UI.CreateList();
list1->setWidth(1);

auto loadingSlider = UI.CreateSlider();
loadingSlider->setBoundValue(&loading_value).setRange(0, 10);

auto loadingProgress = UI.CreateUIProgressBar();
loadingProgress->setBoundValue(&loading_value).setRange(0, 10);

auto button = UI.CreateButton();
button->setLabel("Hold to Load");
button->setOnHeld([&loading_value](UIElement& e, const float& duration) {
    loading_value = duration;
});

auto loadingInput = UI.CreateLabel();

list1->AddChild(loadingSlider);
list1->AddChild(loadingProgress);
list1->AddChild(button);
list1->AddChild(loadingInput);

// === Second Column ===
auto list2 = UI.CreateList();
list2->setWidth(1);

float temperature = 36.5f;

auto tempSlider = UI.CreateSlider();
tempSlider->setBoundValue(&temperature).setRange(34, 42);

auto tempBar = UI.CreateUIProgressBar();
tempBar->setBoundValue(&temperature).setRange(34, 42);

auto chkDevMode = UI.CreateUICheckBox();
chkDevMode->setLabel("Enable Dev Mode");

auto textInput2 = UI.CreateTextField();
textInput2->setPlaceholder("Comment");

auto btnLog = UI.CreateButton();
btnLog->setLabel("Log Temp");

list2->AddChild(tempSlider);
list2->AddChild(tempBar);
list2->AddChild(chkDevMode);
list2->AddChild(textInput2);
list2->AddChild(btnLog);

// === Horizontal Layout (2-column) ===
auto horizontalLayout = UI.CreateList();
horizontalLayout->setWidth(2);
horizontalLayout->AddChild(list1);
horizontalLayout->AddChild(list2);

Menu1->AddChild(horizontalLayout);

    sf::Clock clock;
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