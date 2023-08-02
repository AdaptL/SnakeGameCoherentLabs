#pragma once
#include <SDL.h>
#include "Position.h"
#include "Dimension.h"
#include "Component.h"

class Rectangle : public Component {
public:
    Rectangle(Position position = Position(), Dimension dimension = Dimension(), SDL_Color color = { 0, 0, 0, 255 });

    Position GetPosition() const;
    Dimension GetDimension() const;
    SDL_Color GetColor() const;

    void SetPosition(const Position& position);
    void SetDimension(const Dimension& dimension);
    virtual void SetColor(const SDL_Color& color);

    void Render(SDL_Renderer* renderer) override;

protected:
    Position position_;
    Dimension dimension_;
    SDL_Color color_;
};


