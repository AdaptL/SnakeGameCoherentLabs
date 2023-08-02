#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include "Component.h"
#include "Text.h"
#include "Button.h"
#include "Image.h"

class MenuUI : public Component {
public:
    MenuUI(TTF_Font* font, SDL_Renderer* renderer, const std::string& imagePath);
    void Render(SDL_Renderer* renderer) override;
    bool CheckClick(int mouseX, int mouseY);

private:
    Text title_;
    Text createdByText_;
    Button startButton_;
    Image snakeImage_;
    SDL_Renderer* renderer_;
};
