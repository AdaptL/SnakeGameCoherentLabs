#include "GridCell.h"

GridCell::GridCell(Position position, Dimension dimension, SDL_Color color, Position gridPosition, SDL_Color borderColor, int borderSize)
    : Rectangle(position, dimension, borderColor), gridPos_(gridPosition) {

    inner_ = new Rectangle(Position(position.GetX() + borderSize, position.GetY() + borderSize),
        Dimension(dimension.GetWidth() - 2 * borderSize, dimension.GetHeight() - 2 * borderSize),
        color);
}

GridCell::~GridCell() {
    delete inner_;
}

Position GridCell::GetGridPosition() const { return gridPos_; }
void GridCell::SetGridPosition(Position position) { gridPos_ = position; }

void GridCell::SetColor(const SDL_Color& color) {
    inner_->SetColor(color);
}

void GridCell::Render(SDL_Renderer* renderer) {
    Rectangle::Render(renderer);
    inner_->Render(renderer);
}
