#pragma once
#include <SDL.h>

class Component {
public:
    virtual ~Component() = default;

    virtual void Render(SDL_Renderer* renderer) = 0;
};
