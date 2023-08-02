#pragma once
#include <SDL.h>
#include "Position.h"
#include "Rectangle.h"

class GridCell : public Rectangle {
public:
    GridCell(Position position, Dimension dimension, SDL_Color color, Position gridPosition, SDL_Color borderColor, int borderSize);
    ~GridCell();

    Position GetGridPosition() const;
    void SetGridPosition(Position position);

    void SetColor(const SDL_Color& color) override;
    void Render(SDL_Renderer* renderer) override;

private:
    Position gridPos_;
    Rectangle* inner_;
};
