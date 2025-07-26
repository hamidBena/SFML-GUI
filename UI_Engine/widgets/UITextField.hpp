#pragma once
#include "core/UIElement.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <string>

struct UITextFieldComponents {
    sf::RectangleShape background;
    std::optional<sf::RectangleShape> selectionHighlight;
    std::optional<sf::RectangleShape> cursor;
    sf::Text text;
};

class UITextField : public UILeaf {
public:
    UITextField(){
		e_fillcolor = sf::Color(70, 120, 70, 205);
		e_borderColor = sf::Color(20, 20, 20, 255);
		textColor = sf::Color(249, 249, 251, 255);

		placeholder = "Input here...";
		placeholderColor = sf::Color(242, 242, 247, 170);
	}

	// Standard setters
    UITextField& setOffset(const sf::Vector2f& pos) {
        e_offset = pos;
        return *this;
    }
    UITextField& setSize(const sf::Vector2f& size) {
        e_size = size;
		intr_size = e_size;
        return *this;
    }
	UITextField& setPadding(const sf::Vector2f& pad) {
		e_padding = pad;
		return *this;
	}
	UITextField& setFillColor(const sf::Color& color) {
        e_fillcolor = color;
        return *this;
    }
	UITextField& setBorderColor(const sf::Color& color) {
        e_borderColor = color;
        return *this;
    }
	UITextField& setBorderThickness(float thickness) {
		e_borderThickness = thickness;
        return *this;
    }
	UITextField& setSizeType(SizeType type) {
		sizeType = type;
		return *this;
	}
	UITextField& setLayoutType(LayoutType type) {
        layoutType = type;
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

	//Widget Specific setters
	UITextField& setPlaceholder(std::string ph){placeholder = ph; return *this;}
	UITextField& clearText(){value = ""; return*this;}
	UITextField& setBoundValue(std::string* bound){boundValue = bound; return *this;}

	//Widget specific callback setters
	void setOnChange(std::function<void(UITextField&)> cb) { onChange = std::move(cb); }
	void setOnEnter(std::function<void(UITextField&)> cb) { onEnter = std::move(cb); }

    // --- Drawing ---
    void DrawSelf(sf::RenderTarget& target) override {
		shapes = buildShapes();

        target.draw(shapes.background);
        if (shapes.selectionHighlight) target.draw(*shapes.selectionHighlight);
        if (shapes.cursor) target.draw(*shapes.cursor);
        target.draw(shapes.text);
    }

	void UpdateWidget(const float dt) override {
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

    void HandleWidgetEvent(const UIEvent& event) override {
        bool changed = false;
		sf::FloatRect TextFieldBounds = shapes.background.getGlobalBounds();
		if (event.type == UIEventType::MouseDown) {
			if(auto* data = std::get_if<MouseEventData>(&event.data)){
				focused = TextFieldBounds.contains(data->pos);
				selectionEnd = selectionStart;
			}
		}

		if(!focused) return;

        if (event.type == UIEventType::TextEntered) {
			if(auto* data = std::get_if<TextEventData>(&event.data)){
				if(data->unicodeChar<32 || data->unicodeChar >= 127) return;
				pushUndoState();
				redoStack.clear();
				if (hasSelection()) {
					value.erase(selectionStart, selectionEnd - selectionStart);
					cursorIndex = selectionStart;
					selectionEnd = selectionStart;
				}
				value.insert(cursorIndex, 1, data->unicodeChar);
				cursorIndex++;
				changed = true;
			}
        } else if (event.type == UIEventType::KeyDown) {
			if(auto* data = std::get_if<KeyEventData>(&event.data)){
				if(data->key == sf::Keyboard::Backspace){
					pushUndoState();
					redoStack.clear();
					if(hasSelection()){
						value.erase(selectionStart, selectionEnd - selectionStart);
						cursorIndex = selectionStart;
						selectionEnd = selectionStart;
					}else{ 
						if (!value.empty() && cursorIndex>0) { value.erase(cursorIndex - 1, 1); cursorIndex--; changed = true; } 
					}
				}
				if (data->key == sf::Keyboard::Enter) {
						if(data->shift){
							value.insert(cursorIndex, "\n");
							cursorIndex++;
							changed = true;
						}else{
							focused = false;
							if (onEnter){
								onEnter(*this);
							}
						}
						selectionStart = selectionEnd;
					}
				if (data->key == sf::Keyboard::Left && cursorIndex > 0) {
					cursorIndex--;
					selectionEnd = selectionStart;
				}
				if (data->key == sf::Keyboard::Right && cursorIndex < value.size()) {
					cursorIndex++;
					selectionEnd = selectionStart;
				}
				if (data->key == sf::Keyboard::A && data->ctrl) {
					// Select all
					selectionStart = 0;
					selectionEnd = value.size();
				}
				if (data->key == sf::Keyboard::C && data->ctrl) {	//ctrl c copy
					std::string selected;
					if(hasSelection())
						selected = value.substr(selectionStart, selectionEnd - selectionStart);
					else
						selected = value;
					sf::Clipboard::setString(selected);
				}
				if (data->key == sf::Keyboard::V && data->ctrl) {	//ctrl v paste
					pushUndoState();
					redoStack.clear();
					std::string paste = sf::Clipboard::getString().toAnsiString();
					value.insert(cursorIndex, paste);
					cursorIndex += paste.size();
					changed = true;
				}
				if (data->key == sf::Keyboard::X && data->ctrl) {	//ctrl x cut
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
				}
				if (data->key == sf::Keyboard::Z && data->ctrl) {
					if (!undoStack.empty()) {
						redoStack.push_back(value);
						value = undoStack.back();
						undoStack.pop_back();
						changed = true;
						cursorIndex = value.size();
					}
				}
				if (data->key == sf::Keyboard::Y && data->ctrl) {
					if (!redoStack.empty()) {
						undoStack.push_back(value);
						value = redoStack.back();
						redoStack.pop_back();
						changed = true;
						cursorIndex = value.size();
					}
				}
			}
        }

        if (changed){
			if(onChange) onChange(*this);
			if(boundValue) *boundValue = value;
		}
    }


	void SizePass() override {
		switch(sizeType){
			case SizeType::Percent:
				if (auto parentPtr = parent.lock()) {
				auto parentArea = parentPtr->e_size - parentPtr->e_padding/0.5f;
				e_size.x = parentArea.x * (e_size.x / 100.f);
				e_size.y = parentArea.y * (e_size.y / 100.f);
				}
				break;
			
			case SizeType::FillParent:
				if (auto parentPtr = parent.lock()) { e_size = parentPtr->e_size - parentPtr->e_padding*2.f - e_offset; }
				break;

			case SizeType::FitContent: {
				e_size = shapes.text.getLocalBounds().getSize() + e_padding * 2.f;
				break;
			}

			case SizeType::Absolute:
				break;
		}
		intr_size = e_size;
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
	std::string* boundValue = nullptr;

    sf::Font font = AssetManager::get().getFont("fonts/arial.ttf");
	sf::Color textColor;
	int textSize = 18;

    bool focused = false;
    bool showCursor = false;
	bool selecting = false;

    float cursorTimer = 0.f;
	size_t cursorIndex = 0;
	size_t selectionStart = 0;
	size_t selectionEnd = 0;
	

    std::function<void(UITextField&)> onEnter;
    std::function<void(UITextField&)> onChange;

    std::string placeholder;
    sf::Color placeholderColor;

	std::vector<std::string> undoStack;
	std::vector<std::string> redoStack;

	UITextFieldComponents shapes = buildShapes();

private:
    UITextFieldComponents buildShapes() {
        UITextFieldComponents shapes;

        auto pos  = intr_position.getValue();
        auto size = intr_size.getValue();

        // Border highlight and fill
        sf::Color border = focused ? sf::Color(60, 160, 60) : e_borderColor;
        float thickness = focused ? e_borderThickness + 0.7f : e_borderThickness;

        sf::Color bg = e_fillcolor;
        if (focused) {
            bg.r = std::min(255, bg.r + 20);
            bg.g = std::min(255, bg.g + 20);
            bg.b = std::min(255, bg.b + 30);
        }

        sf::RectangleShape rect(size);
        rect.setPosition(pos);
        rect.setFillColor(bg);
        rect.setOutlineColor(border);
        rect.setOutlineThickness(thickness);
        shapes.background = rect;

        // Selection highlight
        if (hasSelection()) {
            sf::Text selText;
			selText.setFont(font);
			selText.setCharacterSize(textSize);
			selText.setString(value.substr(0, selectionStart));
            float xStart = selText.getLocalBounds().width + pos.x + 5;

            selText.setString(value.substr(selectionStart, selectionEnd - selectionStart));
            float width = selText.getLocalBounds().width;

            sf::RectangleShape highlight({width, float(textSize)});
            highlight.setPosition({xStart, pos.y+e_padding.y/2.f});
            highlight.setFillColor(sf::Color(250, 100, 200, 200));
            shapes.selectionHighlight = highlight;
        }

        // Actual text
        sf::Text txt;
        txt.setFont(font);
        txt.setCharacterSize(textSize);
        txt.setFillColor(textColor);
        txt.setString(value);

        // Placeholder
        if ((value.empty() || value == "|") && !(focused && showCursor) && !placeholder.empty()) {
			if(value == "|") txt.setString(placeholder + "|");
			else txt.setString(placeholder);
            txt.setFillColor(placeholderColor);
        }
		txt.setPosition(pos.x + 5, pos.y + (size.y - txt.getLocalBounds().height) / 2.f - txt.getLocalBounds().top);
		shapes.text = txt;

        return shapes;
    }
};