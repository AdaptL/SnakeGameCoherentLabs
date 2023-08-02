#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <string>
#include "Rectangle.h"

class Image : public Rectangle {
public:
    Image(Position position = Position(), Dimension dimension = Dimension(),
        const std::string& imagePath = "", SDL_Renderer* renderer = nullptr, float size = 1);
    ~Image();

    void Render(SDL_Renderer* renderer) override;

private:
    std::string imagePath_;
    SDL_Renderer* renderer_;
    SDL_Texture* texture_;
    float size_;
};
