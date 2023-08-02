#include "Apple.h"

Apple::Apple(Grid* grid) : grid_(grid) {
}

void Apple::ClearApple() {
    grid_->SetCellType(applePosition_, Grid::CellType::Empty);
}

void Apple::SpawnApple() {
    applePosition_ = grid_->GetRandomEmptyPosition();
    grid_->SetCellType(applePosition_, Grid::CellType::Apple);
}

Position Apple::GetPosition() {
    return applePosition_;
}
