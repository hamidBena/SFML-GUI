#pragma once
#include "core/UIElement.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <string>

class UITextField : public UILeaf {
public:
    UITextField(const std::string& name = defaultName()) : UILeaf(name) {}

	std::string *boundValue = nullptr;

    // --- Standard setters
    UITextField& setOffset(const sf::Vector2f& pos) { e_offset = pos; markLayoutDirty(); return *this; }
    UITextField& setSize(const sf::Vector2f& size) { e_size = size; markLayoutDirty(); return *this; }
    UITextField& setFillColor(const sf::Color& color) { e_fillcolor = color; return *this; }
    UITextField& setAnchor(LayoutAnchor anch) { anchor = anch; markLayoutDirty(); return *this; }
    UITextField& setLayoutType(LayoutType type) { layoutType = type; markLayoutDirty(); return *this; }
    UITextField& setSizeType(SizeType type) { sizeType = type; markLayoutDirty(); return *this; }
    UITextField& setPadding(const sf::Vector2f& pad) { e_padding = pad; markLayoutDirty(); return *this; }
    UITextField& setBorder(float thickness, const sf::Color& color) { borderThickness = thickness; borderColor = color; return *this; }
    UITextField& setFont(const sf::Font& f) { font = f; text.setFont(f); return *this; }
    UITextField& setStringSize(unsigned int size) { textSize = size; text.setCharacterSize(size); return *this; }
    UITextField& setStringColor(const sf::Color& color) { textColor = color; text.setFillColor(color); return *this; }
    UITextField& setString(const std::string& str) { value = str; if(boundValue) *boundValue = str; text.setString(str); return *this; }
	
	// --- Element specific
    UITextField& setPlaceholder(const std::string& str) {
        placeholder = str;
        return *this;
    }
	UITextField& setEnable(bool en) {
		enabled = en;
		return *this;
	}
	UITextField& setVisible(bool vis) {
		visible = vis;
		return *this;
	}
    UITextField& setPlaceholderColor(const sf::Color& color) {
        placeholderColor = color;
        return *this;
    }
	UITextField& clearText() {value = ""; if(boundValue) *boundValue = ""; text.setString("");
		return *this;}

	//lambda setters
    UITextField& setOnEnter(std::function<void(const std::string&)> cb) {
        onEnter = std::move(cb);
        return *this;
    }
    UITextField& setOnChange(std::function<void(const std::string&)> cb) {
        onChange = std::move(cb);
        return *this;
    }
    UITextField& setOnTick(std::function<void(UITextField&)> cb) {
        onTick = std::move(cb);
        return *this;
    }

	UITextField& setBoundValue(std::string* bound) {
		boundValue = bound;
		if (bound) {
			value = *bound;
			text.setString(value);
		}
		return *this;
	}

    // --- Drawing ---
    void DrawSelf(sf::RenderTarget& target, sf::RenderStates states) override {
		if(!visible) return;

		if (boundValue && value != *boundValue) {
			value = *boundValue;
			text.setString(value);
		}

        // Enhanced highlight: glow effect and stronger border when focused
        sf::Color border = focused ? sf::Color(60, 160, 255) : borderColor;
        float thickness = focused ? borderThickness + 2.5f : borderThickness;

        // Glow effect: draw a soft blue shadow if focused
        if (focused) {
            sf::RectangleShape glow(e_size + sf::Vector2f(8, 8));
            glow.setPosition(e_position - sf::Vector2f(4, 4));
            glow.setFillColor(sf::Color(60, 160, 255, 40));
            glow.setOutlineThickness(0);
            target.draw(glow, states);
        }

        // Slightly brighten background when focused
        sf::Color bg = e_fillcolor;
        if (focused) {
            bg.r = std::min(255, bg.r + 20);
            bg.g = std::min(255, bg.g + 20);
            bg.b = std::min(255, bg.b + 30);
        }

        sf::RectangleShape rect(e_size);
        rect.setPosition(e_position);
        rect.setFillColor(bg);
        rect.setOutlineColor(border);
        rect.setOutlineThickness(thickness);
        target.draw(rect, states);

		if (hasSelection()) {
			sf::Text selText(text);
			selText.setString(value.substr(0, selectionStart));
			float xStart = selText.getLocalBounds().width + e_position.x + 5;

			selText.setString(value.substr(selectionStart, selectionEnd - selectionStart));
			float width = selText.getLocalBounds().width;

			sf::RectangleShape highlight({width, float(textSize)});
			highlight.setPosition({xStart, e_position.y + 5});
			highlight.setFillColor(sf::Color(100, 100, 255, 70)); // semi-transparent blue
			target.draw(highlight, states);
		}

        // text placeholder
        std::string display = boundValue ? *boundValue : value;

        sf::Text caretText(text);
		caretText.setString(value.substr(0, cursorIndex));
		float caretX = caretText.getLocalBounds().width;

		if (focused && showCursor) {
			sf::RectangleShape cursor(sf::Vector2f(1, textSize));
			cursor.setPosition({e_position.x + 7 + caretX, e_position.y + textSize/2});
			cursor.setFillColor(sf::Color::Black);
			target.draw(cursor, states);
		}

        text.setFont(font);
        text.setCharacterSize(textSize);
        text.setFillColor(textColor);
        text.setString(display);
        text.setPosition(e_position.x + 5, e_position.y + (e_size.y - text.getLocalBounds().height) / 2.f - text.getLocalBounds().top);
        if (!value.empty() || (focused && showCursor)) {
            target.draw(text, states);
        } else if (!placeholder.empty()) {
            sf::Text ph;
            ph.setFont(font);
            ph.setCharacterSize(textSize);
            ph.setString(placeholder);
            sf::Color phColor = placeholderColor;
            phColor.a = 120; // semi-transparent
            ph.setFillColor(phColor);
            ph.setPosition(e_position.x + 5, e_position.y + (e_size.y - ph.getLocalBounds().height) / 2.f - ph.getLocalBounds().top);
            target.draw(ph, states);
        }

		if (layoutDirty) {
			sf::ConvexShape triangle;
			triangle.setPointCount(3);
			triangle.setPoint(0, e_position);
			triangle.setPoint(1, e_position + sf::Vector2f(10, 0));
			triangle.setPoint(2, e_position + sf::Vector2f(0, 10));
			triangle.setFillColor(sf::Color::Red);
			target.draw(triangle, states);
		}
    }

	void Update(const float dt) override {
		if(!enabled) return;

		if (onTick) onTick(*this);

		if (focused) {
			cursorTimer += dt;
			if (cursorTimer > 0.5f) {
				showCursor = !showCursor;
				cursorTimer = 0.f;
			}
		} else {
			showCursor = false;
		}

	}

    void CalculateLayout() override {
		if(!layoutDirty) return;
		layoutDirty = false;

        if(layoutType == LayoutType::Static) {
            e_position = e_offset;
        } else if(layoutType == LayoutType::Relative) {
            if (auto parentPtr = parent.lock()) {
                e_position = parentPtr->e_position + parentPtr->e_padding + e_offset;
            } else {
                e_position = e_offset;
            }
        } else if(layoutType == LayoutType::Percent) {
            if (auto parentPtr = parent.lock()) {
                sf::Vector2f parentSize = parentPtr->e_size - parentPtr->e_padding * 2.0f;
                e_position.x = parentPtr->e_position.x + parentPtr->e_padding.x + (parentSize.x * (e_offset.x / 100.f));
                e_position.y = parentPtr->e_position.y + parentPtr->e_padding.y + (parentSize.y * (e_offset.y / 100.f));
            }
        } else if(layoutType == LayoutType::Anchor) {
            e_position = e_offset;
        }

		// Calculate size based on sizeType
		if (sizeType == SizeType::FitContent) {
			sf::Text tempText(value.empty() ? placeholder : value, font, textSize);
			sf::FloatRect bounds = tempText.getLocalBounds();
			float width = bounds.width + e_padding.x * 2.f + 20.f; // +10 to account for cursor or buffer
			float height = bounds.height + e_padding.y * 2.f + 20.f;

			e_size = { width, height };
		} else if (sizeType == SizeType::FillParent) {
			// Fill parent logic (e.g., match parent's size)
			if (auto parentPtr = parent.lock()) {
				e_size = parentPtr->e_size-parentPtr->e_padding/0.5f - e_offset;
			}
		} else if (sizeType == SizeType::Percent) {
			// Percent size logic (e.g., based on parent's size)
			if (auto parentPtr = parent.lock()) {
				auto parentArea = parentPtr->e_size - parentPtr->e_padding/0.5f;
				e_size.x = parentArea.x * (e_size.x / 100.f);
				e_size.y = parentArea.y * (e_size.y / 100.f);
			}
		}
    }

    void HandleEvent(const UIEvent& event) override {
        if (!enabled) return; // Ignore events if not enabled

        bool changed = false;
        if (event.type == UIEventType::MouseDown) {
            focused = contains(event.mousePos);
        }

		if(!focused) return;

        if (event.type == UIEventType::TextEntered && event.textChar >= 32 && event.textChar < 127) {
			pushUndoState();
			redoStack.clear();
			if (hasSelection()) {
				value.erase(selectionStart, selectionEnd - selectionStart);
				cursorIndex = selectionStart;
				selectionEnd = selectionStart;
			}
            value.insert(cursorIndex, 1, event.textChar);
			cursorIndex++;
            text.setString(value);
            changed = true;
        } else if (event.type == UIEventType::KeyDown) {
            if (event.key == sf::Keyboard::BackSpace) {
				pushUndoState();
				redoStack.clear();
				if(hasSelection()){
					value.erase(selectionStart, selectionEnd - selectionStart);
					cursorIndex = selectionStart;
					selectionEnd = selectionStart;
				}else{ 
					if (!value.empty() && cursorIndex>0) { value.erase(cursorIndex - 1, 1); cursorIndex--; changed = true; } 
				}

                text.setString(value);
            } else if (event.key == sf::Keyboard::Enter) {
				if(event.shift){
					value.insert(cursorIndex, "\n");
					cursorIndex++;
					changed = true;
				}else{
					focused = false;
					if (onEnter){
						onEnter(value);
					}
				}
            } else if (event.key == sf::Keyboard::Left && cursorIndex > 0) {
				cursorIndex--;
			} else if (event.key == sf::Keyboard::Right && cursorIndex < value.size()) {
				cursorIndex++;
			} else if (event.key == sf::Keyboard::A && event.ctrl) {
				// Select all
				selectionStart = 0;
				selectionEnd = value.size();
				selecting = true;
			} else if (event.key == sf::Keyboard::C && event.ctrl) {	//ctrl c copy
				std::string selected;
				if(hasSelection())
					selected = value.substr(selectionStart, selectionEnd - selectionStart);
				else
					selected = value;
				sf::Clipboard::setString(selected);

			}else if (event.key == sf::Keyboard::V && event.ctrl) {	//ctrl v paste
				pushUndoState();
				redoStack.clear();
				std::string paste = sf::Clipboard::getString().toAnsiString();
				value.insert(cursorIndex, paste);
				cursorIndex += paste.size();
				changed = true;
			}else if (event.key == sf::Keyboard::X && event.ctrl) {	//ctrl x cut
				pushUndoState();
				redoStack.clear();

				std::string selected;
				if(hasSelection()){
					selected = value.substr(selectionStart, selectionEnd - selectionStart);
					value.erase(selectionStart, selectionEnd - selectionStart);
				}else{
					selected = value;
					value.clear();
				}
				sf::Clipboard::setString(selected);
				cursorIndex = value.size();
				changed = true;
			}else if (event.key == sf::Keyboard::Z && event.ctrl) {
				if (!undoStack.empty()) {
					redoStack.push_back(value);
					value = undoStack.back();
					undoStack.pop_back();
					changed = true;
					cursorIndex = value.size();
				}
			}else if (event.key == sf::Keyboard::Y && event.ctrl) {
				if (!redoStack.empty()) {
					undoStack.push_back(value);
					value = redoStack.back();
					redoStack.pop_back();
					changed = true;
					cursorIndex = value.size();
				}
			}
        }

        if (changed){
			markLayoutDirty();
			if(onChange) onChange(value);
		}
        if (boundValue) *boundValue = value; // Update bound value if set
    }

private:
    bool contains(const sf::Vector2f& pt)  {
        return pt.x >= e_position.x && pt.x <= e_position.x + e_size.x &&
               pt.y >= e_position.y && pt.y <= e_position.y + e_size.y;
    }
	bool hasSelection()  {
		return selectionEnd > selectionStart;
	}
	void pushUndoState() {
		if (undoStack.empty() || undoStack.back() != value) {
			undoStack.push_back(value);
			if (undoStack.size() > 100) undoStack.erase(undoStack.begin()); // limit history size
		}
	}
    std::string value;
    sf::Text text;
    sf::Font font = AssetManager::get().getFont("fonts/arial.ttf");
    sf::Color textColor = sf::Color::Black;
    unsigned int textSize = 18;
    bool focused = false;
    bool showCursor = false;
    float cursorTimer = 0.f;
	size_t cursorIndex = 0;
	size_t selectionStart = 0;
	size_t selectionEnd = 0;
	bool selecting = false;
    std::function<void(const std::string&)> onEnter;
    std::function<void(const std::string&)> onChange;
    std::function<void(UITextField&)> onTick;
    std::string placeholder;
    sf::Color placeholderColor = sf::Color(120, 120, 120, 120);

	std::vector<std::string> undoStack;
	std::vector<std::string> redoStack;
	float lastEditTime = 0.f;
};