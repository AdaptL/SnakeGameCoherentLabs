#pragma once
#include "Grid.h"
#include "Position.h"

class Apple {
public:
    Apple(Grid* grid);

    void ClearApple();
    void SpawnApple();
    Position GetPosition();

private:
    Grid* grid_;
    Position applePosition_;
};
