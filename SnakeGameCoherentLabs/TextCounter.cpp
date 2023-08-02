#include "TextCounter.h"
#include <string>

TextCounter::TextCounter(Position pos, Dimension dim, TTF_Font* font, SDL_Renderer* renderer)
    : Text(pos, dim, "0", font), renderer_(renderer), count_(0) {}

void TextCounter::Increment() {
    ++count_;
    SetText(initialText_ + std::to_string(count_), renderer_);
}

void TextCounter::Decrement() {
    if (count_ > 0) {
        --count_;
        SetText(initialText_ + std::to_string(count_), renderer_);
    }
}

int TextCounter::GetCounter() {
    return count_;
}

void TextCounter::Reset() {
    count_ = 0;
    SetText(initialText_ + std::to_string(count_), renderer_);
    Render(renderer_);
}
