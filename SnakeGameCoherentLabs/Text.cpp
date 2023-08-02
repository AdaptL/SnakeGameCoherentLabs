#include "Text.h"
#include <iostream>

Text::Text(Position pos, Dimension dim, const std::string& text, TTF_Font* font)
    : Rectangle(pos, dim), text_(text), font_(font) {
    color_ = { 0, 255, 0, 255 };
}

void Text::SetText(const std::string& text, SDL_Renderer* renderer) {
    text_ = text;
    UpdateTexture(renderer);
}

void Text::Render(SDL_Renderer* renderer) {
    if (!isTextureInitialized_) {
        UpdateTexture(renderer);
        isTextureInitialized_ = true;
    }

    SDL_Rect rect{ position_.GetX(), position_.GetY(), dimension_.GetWidth(), dimension_.GetHeight() };
    SDL_RenderCopy(renderer, texture_, nullptr, &rect);
}

Text::~Text() {
    SDL_DestroyTexture(texture_);
}

void Text::UpdateTexture(SDL_Renderer* renderer) {
    if (texture_) {
        SDL_DestroyTexture(texture_);
    }

    if (!font_) {
        std::cerr << "No font set for text object.\n";
        return;
    }

    SDL_Surface* surface = TTF_RenderText_Solid(font_, text_.c_str(), color_);
    if (!surface) {
        std::cerr << "Failed to create text surface.\n";
        return;
    }

    texture_ = SDL_CreateTextureFromSurface(renderer, surface);

    if (!texture_) {
        std::cerr << "Failed to create texture from surface.\n";
    }

    SDL_FreeSurface(surface);
}
