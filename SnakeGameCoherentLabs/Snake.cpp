#include "Snake.h"
#include <iostream>
Snake::Snake(Grid* grid, Position startingPos, Direction startingDir)
    : grid_(grid), direction_(startingDir), startingPos_(startingPos) {
    bodyPositions_.push_front(startingPos);
    grid_->SetCellType(startingPos, Grid::SnakeHead);
}

bool Snake::HasCollided() {
    return hasCollided_;
}

bool Snake::Move(bool ateApple)
{
    Position newHeadPos = GetNextHeadPosition();

    if (!IsValidMove(newHeadPos)) {
        // Handle game over

        std::cout << "Collision! Game over" << std::endl;

        hasCollided_ = true;
        return false;
    }

    // Add the new head position to the body
    bodyPositions_.push_front(newHeadPos);
    occupiedPositions_.insert(newHeadPos);

    // If the snake didn't eat an apple, remove the last body segment
    if (!ateApple) {
        Position tailPos = bodyPositions_.back();
        bodyPositions_.pop_back();
        occupiedPositions_.erase(tailPos);
    }


    // Set the new cell type on the grid
    grid_->SetCellType(newHeadPos, Grid::SnakeHead);

    hasMoved_ = true;

    return true;
}

void Snake::ClearSnakeFromGrid()
{
    for (const auto& pos : bodyPositions_) {
        grid_->SetCellType(pos, Grid::Empty);
    }
}

void Snake::AddSnakeToGrid()
{
    for (auto it = bodyPositions_.begin() + 1; it != bodyPositions_.end(); ++it) {
        grid_->SetCellType(*it, Grid::SnakeBody);
    }


    if (!bodyPositions_.empty()) {
        grid_->SetCellType(bodyPositions_.front(), Grid::SnakeHead);
    }

}

void Snake::RestartSnake()
{
    hasCollided_ = false;
    hasMoved_ = true;

    bodyPositions_.clear();
    occupiedPositions_.clear();

    direction_ = Direction::Right;
    bodyPositions_.push_front(startingPos_);

    grid_->SetCellType(startingPos_, Grid::SnakeHead);

    IncreaseLength();
    IncreaseLength();
}

void Snake::UpdateDirection(Direction newDirection)
{
    if (hasMoved_)
    {
        switch (newDirection) {
        case Direction::Up:
            if (direction_ != Direction::Down) {
                direction_ = newDirection;
            }
            break;
        case Direction::Down:
            if (direction_ != Direction::Up) {
                direction_ = newDirection;
            }
            break;
        case Direction::Left:
            if (direction_ != Direction::Right) {
                direction_ = newDirection;
            }
            break;
        case Direction::Right:
            if (direction_ != Direction::Left) {
                direction_ = newDirection;
            }
            break;
        }
        hasMoved_ = false;
    }
}

void Snake::IncreaseLength()
{
    Position newHead = GetNextHeadPosition();
    bool isWithinGrid = (newHead.GetX() >= 0 || newHead.GetX() < grid_->GetGridWidth()) && (newHead.GetY() >= 0 || newHead.GetY() < grid_->GetGridHeight());
    //   auto isOccupied = occupied_positions_.find(newHead) != occupied_positions_.end();

       // Check if the new head position is within the grid and not occupied
    if (isWithinGrid) {
        // Add new head to the front of bodyPositions_
        bodyPositions_.push_front(newHead);
        // Also add the new position to the set of occupied positions
        occupiedPositions_.insert(newHead);
        // Don't remove the last body segment, causing the snake to grow in length
    }
}

Position Snake::GetNextHeadPosition()
{
    Position currentHeadPos = bodyPositions_.front();

    switch (direction_) {
    case Direction::Up:
        return Position(currentHeadPos.GetX(), currentHeadPos.GetY() - 1);
    case Direction::Down:
        return Position(currentHeadPos.GetX(), currentHeadPos.GetY() + 1);
    case Direction::Left:
        return Position(currentHeadPos.GetX() - 1, currentHeadPos.GetY());
    case Direction::Right:
        return Position(currentHeadPos.GetX() + 1, currentHeadPos.GetY());
    }
}

bool Snake::HasMoved()
{
    return hasMoved_;
}

bool Snake::IsValidMove(Position newPos)
{
    if (hasCollided_)
    {
        return true;
    }

    // Check if position is within the grid
    if (newPos.GetX() < 0 || newPos.GetX() >= grid_->GetGridWidth() || newPos.GetY() < 0 || newPos.GetY() >= grid_->GetGridHeight()) {
        return false;
    }

    // Check if position is occupied by the snake's body
    if (occupiedPositions_.find(newPos) != occupiedPositions_.end()) {
        return false;
    }

    return true;
}
