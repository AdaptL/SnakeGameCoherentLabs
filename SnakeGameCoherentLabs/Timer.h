#pragma once
#include "Text.h"
#include <SDL.h>

class Timer : public Text {
public:
    Timer(Position pos = Position(), Dimension dim = Dimension(), TTF_Font* font = nullptr, SDL_Renderer* renderer = nullptr);

    void Update();
    void Reset();

private:
    std::string GetFormattedTime() const;
    Uint32 lastUpdateTime_;
    int seconds_;
    int minutes_;
    SDL_Renderer* renderer_;
};
