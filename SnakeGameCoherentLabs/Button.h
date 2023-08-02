#pragma once
#include "Rectangle.h"
#include "Text.h"
#include <functional>
#include <string>

class Button : public Rectangle {
public:
    using Callback = std::function<void()>;

    Button(Position pos = Position(), Dimension dim = Dimension(), Callback callback = nullptr, TTF_Font* font = nullptr, const std::string& text = "");

    void OnClick();
    bool CheckClick(int mouseX, int mouseY);
    void Render(SDL_Renderer* renderer) override;

private:
    Callback callback_;
    Text text_;
};
