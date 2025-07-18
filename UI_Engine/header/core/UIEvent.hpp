#pragma once
#include <SFML/Graphics.hpp>

enum class UIEventType {
    MouseMove,
    MouseDown,
    MouseUp,
    Click,
    Hover,
    Leave,
    KeyDown,
    KeyUp,
	TextEntered
};

struct UIEvent {
    UIEventType type;
    sf::Vector2f mousePos;
    int mouseButton = 0; // 0=left, 1=right.
    int key = 0;         // Key code for keyboard events
    char textChar = 0;   // Character for text input events
};
