#pragma once
#include <SDL.h>
#include <map>
#include <vector>
#include "Position.h"
#include "Dimension.h"
#include "GridCell.h"

class Grid : public Rectangle {
public:
    enum CellType {
        Empty,
        SnakeBody,
        SnakeHead,
        Apple
    };

public:
    Grid(Position pos = Position(), Dimension dim = Dimension(), int cellSize = 1);
    ~Grid();

    CellType GetCellTypeAtCoordinate(Position gridPos);
    int GetGridWidth() const;
    int GetGridHeight() const;
    void ClearGrid();
    bool AppleExists();
    void SetCellType(Position gridPos, CellType type);
    void Render(SDL_Renderer* renderer) override;
    Position GetGridPosition(Position pos) const;
    Position GetRelativePosition(Position gridPos) const;
    Position GetRandomEmptyPosition();

private:
    std::map<Position, std::pair<CellType, GridCell*>> cells_;
    std::map<Position, Position> gridToPos_;
    std::map<Position, Position> posToGrid_;
    int gridWidth;
    int gridHeight;
    int cellSize_;
};

