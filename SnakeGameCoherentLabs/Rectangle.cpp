#include "Rectangle.h"

Rectangle::Rectangle(Position position, Dimension dimension, SDL_Color color)
    : position_(position), dimension_(dimension), color_(color) {}

Position Rectangle::GetPosition() const { return position_; }
Dimension Rectangle::GetDimension() const { return dimension_; }
SDL_Color Rectangle::GetColor() const { return color_; }

void Rectangle::SetPosition(const Position& position) { position_ = position; }
void Rectangle::SetDimension(const Dimension& dimension) { dimension_ = dimension; }
void Rectangle::SetColor(const SDL_Color& color) { color_ = color; }

void Rectangle::Render(SDL_Renderer* renderer) {
    SDL_Rect rect{ position_.GetX(), position_.GetY(), dimension_.GetWidth(), dimension_.GetHeight() };
    SDL_SetRenderDrawColor(renderer, color_.r, color_.g, color_.b, color_.a);
    SDL_RenderFillRect(renderer, &rect);
}
