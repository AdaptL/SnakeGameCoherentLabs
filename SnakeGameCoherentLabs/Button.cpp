#include "Button.h"
#include <iostream>

Button::Button(Position pos, Dimension dim, Callback callback, TTF_Font* font, const std::string& text)
    : Rectangle(pos, dim), callback_(callback) {
    Position textPos = Position(pos.GetX() + (dim.GetWidth() / 4), pos.GetY() + (dim.GetHeight() / 4));
    Dimension textDim = Dimension(dim.GetWidth() / 2, dim.GetHeight() / 2);
    text_ = Text(textPos, textDim, text, font);
    color_ = { 0, 255, 0, 255 };
    text_.SetColor({ 0, 255, 0, 255 });
}

void Button::OnClick() {
    std::cout << "You clicked!" << std::endl;
    if (callback_) {
        callback_();
    }
}

bool Button::CheckClick(int mouseX, int mouseY) {
    if (mouseX >= position_.GetX() && mouseX <= position_.GetX() + dimension_.GetWidth() &&
        mouseY >= position_.GetY() && mouseY <= position_.GetY() + dimension_.GetHeight()) {
        OnClick();
        return true;
    }
    return false;
}

void Button::Render(SDL_Renderer* renderer) {
    SDL_Rect rect{ position_.GetX(), position_.GetY(), dimension_.GetWidth(), dimension_.GetHeight() };

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &rect);

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderDrawRect(renderer, &rect);

    text_.Render(renderer);
}
