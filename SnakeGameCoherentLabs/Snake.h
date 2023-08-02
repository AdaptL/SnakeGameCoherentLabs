#pragma once
#include "Grid.h"
#include "Position.h"
#include <deque>
#include <unordered_set>

class Snake {
public:
    enum class Direction {
        Up,
        Down,
        Left,
        Right
    };

    Snake(Grid* grid, Position startingPos = Position(3, 3), Direction startingDir = Direction::Right);

    bool HasCollided();
    bool Move(bool ateApple = false);
    void ClearSnakeFromGrid();
    void AddSnakeToGrid();
    void RestartSnake();
    void UpdateDirection(Direction newDirection);
    void IncreaseLength();
    Position GetNextHeadPosition();
    bool HasMoved();

private:
    Grid* grid_;
    std::deque<Position> bodyPositions_;
    Direction direction_;
    std::unordered_set<Position, PositionHash> occupiedPositions_;
    bool hasMoved_ = false;
    bool hasCollided_ = false;
    Position startingPos_;

    bool IsValidMove(Position newPos);
};
