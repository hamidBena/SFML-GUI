#pragma once
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <variant>
#include <optional>

enum class UIEventType {
    MouseMove,
    MouseDown,
    MouseUp,
    KeyDown,
    KeyUp,
    TextEntered,
};

struct MouseEventData {
    sf::Vector2f pos;
    int button; // 0=left, 1=right
};

struct KeyEventData {
    int key;     // sf::Keyboard::Key
    bool ctrl;
    bool shift;
    bool alt;
};

struct TextEventData {
    char unicodeChar;
};

using UIEventPayload = std::variant<std::monostate ,MouseEventData, KeyEventData, TextEventData>;

struct UIEvent {
    UIEventType type;
    UIEventPayload data;
};
