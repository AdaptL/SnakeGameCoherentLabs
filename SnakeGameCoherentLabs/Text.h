#pragma once
#include "Rectangle.h"
#include <string>
#include <SDL_ttf.h>

class Text : public Rectangle {
public:
    Text(Position pos = Position(), Dimension dim = Dimension(), const std::string& text = "", TTF_Font* font = nullptr);

    void SetText(const std::string& text, SDL_Renderer* renderer);
    void Render(SDL_Renderer* renderer) override;
    ~Text();

protected:
    void UpdateTexture(SDL_Renderer* renderer);

    std::string text_;
    TTF_Font* font_;
    SDL_Texture* texture_ = nullptr;
    bool isTextureInitialized_ = false;
};
