#pragma once
#include <functional>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <memory>

// soon to be implemented instead of the element's lambda functions

class EventBus {
public:
    template<typename EventT>
    using Handler = std::function<void(const EventT&)>;

    template<typename EventT>
    void Subscribe(const Handler<EventT>& handler) {
        auto& vec = handlers[typeid(EventT)];
        vec.push_back([handler](const void* evt) {
            handler(*static_cast<const EventT*>(evt));
        });
    }

    template<typename EventT>
    void Publish(const EventT& event) const {
        auto it = handlers.find(typeid(EventT));
        if (it != handlers.end()) {
            for (const auto& fn : it->second) {
                fn(&event);
            }
        }
    }

    void Clear() { handlers.clear(); }

private:
    using HandlerFn = std::function<void(const void*)>;
    mutable std::unordered_map<std::type_index, std::vector<HandlerFn>> handlers;
};

// Example:
// struct ButtonClickedEvent { std::string buttonName; };
// Usage:
// EventBus bus;
// bus.Subscribe<ButtonClickedEvent>([](const ButtonClickedEvent& evt) { ... });
// bus.Publish(ButtonClickedEvent{"myButton"});
