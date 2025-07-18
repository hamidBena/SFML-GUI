#include "UILibrary.hpp"
#include "SFML/Graphics.hpp"
#include <iostream>
#include <sstream>

//test for the GUI draw space (such that when the view moves around the GUI and its even handling such as dragging and moving stays in screen space)

int main() {
    // Create a window
    sf::RenderWindow window(sf::VideoMode(2000, 1200), "SFML Example");
    window.setFramerateLimit(100);

    // === [1] Player and View Setup ===
    sf::RectangleShape player(sf::Vector2f(100, 100));
    player.setFillColor(sf::Color::Blue);
    player.setOrigin(50, 50); // Center origin
    sf::Vector2f playerPos(0.f, 0.f);
    sf::View gameView;

	sf::RectangleShape world(sf::Vector2f(1000, 1000));
    world.setFillColor({100,100,100,150});
    world.setOrigin(500, 500); // Center origin
	world.setPosition({0,0});

    gameView.setSize(window.getSize().x, window.getSize().y);
    gameView.setCenter(playerPos);

    float moveSpeed = 400.f;

    std::string input="";

    // === [UI Setup] ===
    GUI UI;
    auto Menu1 = UI.CreateRoot();
    Menu1->setOffset({800, 100})
         .setPadding({10, 15})
         .setFillColor({250,250,50,50})
         .setSizeType(SizeType::FitContent)
         .setHeaderTitle("Main Menu");

	auto textfield1 = UI.CreateTextField();
	textfield1->setPlaceholder("coordinates: x,y")
	.setBoundValue(&input);

	auto clearButton = UI.CreateButton();
	clearButton->setLabel("Clear")
	.setFillColor({250,100,50,200})
	.setOffset({textfield1->getSize().x+10, 40})
	.setSize({95,30})
	.setOnClick([&textfield1](){
		textfield1->clearText();
		textfield1->setPlaceholder("coordinates: x,y");
	});

	auto teleportButton = UI.CreateButton();
	teleportButton->setLabel("teleport")
	.setFillColor({100,250,50,200})
	.setOffset({textfield1->getSize().x+10, 0})
	.setSize({95,30})
	.setOnClick([&playerPos, &input, &textfield1]() {
		try {
			size_t commaPos = input.find(',');
			if (commaPos == std::string::npos){
				textfield1->setPlaceholder("invalid format. x,y");
				textfield1->clearText();
				return;
			}

			std::string xStr = input.substr(0, commaPos);
			std::string yStr = input.substr(commaPos + 1);

			float x = std::stof(xStr);
			float y = std::stof(yStr);

			playerPos = sf::Vector2f(x, y);

			textfield1->clearText();
		} catch (const std::exception& e) {
			std::cerr << "Invalid input: " << e.what() << "\n";
		}
	});

	auto player_pos_label = UI.CreateLabel();
	player_pos_label->setOffset({0, 100})
	.setDecimal(-1)
	.setOnTick([&playerPos, &player_pos_label](float dt){
		player_pos_label->setText("player pos: " + std::to_string(playerPos.x) + ", " + std::to_string(playerPos.y) + " baby");
		
	});

	Menu1->AddChild(player_pos_label);
	Menu1->AddChild(textfield1);
	Menu1->AddChild(clearButton);
	Menu1->AddChild(teleportButton);
    UI.RefreshLayout();

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

        // === [2] Movement Input Handling ===
        sf::Vector2f move(0.f, 0.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) move.y -= 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) move.y += 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) move.x -= 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) move.x += 1.f;

        if (move != sf::Vector2f(0,0))
            playerPos += move * moveSpeed * dt;

        player.setPosition(playerPos);
        gameView.setCenter(playerPos);

        // === [3] Drawing ===
        window.clear({150,150,150});

		window.setView(gameView);
		window.draw(world);
        window.draw(player);

        UI.draw(window);

        window.display();
    }

    return 0;
}
