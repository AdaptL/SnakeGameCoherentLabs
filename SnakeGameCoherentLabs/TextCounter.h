#pragma once
#include "Text.h"
#include <SDL.h>

class TextCounter : public Text {
public:
    TextCounter(Position pos = Position(), Dimension dim = Dimension(), TTF_Font* font = nullptr, SDL_Renderer* renderer = nullptr);

    void Increment();
    void Decrement();
    int GetCounter();
    void Reset();

private:
    int count_;
    std::string initialText_;
    SDL_Renderer* renderer_;
};
