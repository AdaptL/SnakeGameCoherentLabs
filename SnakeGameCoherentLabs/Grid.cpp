#include "Grid.h"
#include <stdexcept>
#include <iostream>

Grid::Grid(Position pos, Dimension dim, int cellSize)
	: Rectangle(pos, dim), cellSize_(cellSize)

{
    // Initialize the grid
    gridWidth = dim.GetWidth() / cellSize;
    gridHeight = dim.GetHeight() / cellSize;

    int cellWidth = dim.GetWidth() / gridWidth;
    int cellHeight = dim.GetHeight() / gridHeight;
    SDL_Color transparent = { 0, 0, 0, 0 };

    this->color_ = transparent;


    SDL_Color borderColor = { 0, 0, 0, 255 }; 
    int borderSize = 2; 
    for (int x = 0; x < gridWidth; ++x) {
        for (int y = 0; y < gridHeight; ++y) {
            Position cellPos(pos.GetX() + x * cellWidth, pos.GetY() + y * cellHeight);
            Position gridPos(x, y);  
            Dimension cellDim(cellWidth, cellHeight);
            GridCell* cell = new GridCell(cellPos, cellDim, { 0 , 0, 0 , 255 }, gridPos, borderColor, borderSize);
            cells_[cellPos] = std::make_pair(Empty, cell);
            posToGrid_[cellPos] = gridPos;
            gridToPos_[gridPos] = cellPos;
        }
    }
}

Grid::~Grid()
{
	for (auto& pair : cells_) {
		delete pair.second.second; // Delete Rectangle*
	}
}

Grid::CellType Grid::GetCellTypeAtCoordinate(Position gridPos)
{
    auto relativePos = GetRelativePosition(gridPos);
    return cells_[relativePos].first;
}

int Grid::GetGridWidth() const
{
    return gridWidth;
}

int Grid::GetGridHeight() const
{
    return gridHeight;
}

void Grid::ClearGrid()
{
    for (int i = 0; i < GetGridWidth(); ++i)
    {
        for (int j = 0; j < GetGridHeight(); ++j)
        {
            SetCellType(Position(i, j), CellType::Empty);
        }
    }
}

bool Grid::AppleExists()
{
    for (int i = 0; i < GetGridWidth(); ++i)
    {
        for (int j = 0; j < GetGridHeight(); ++j)
        {
            if (GetCellTypeAtCoordinate({ i,j }) == CellType::Apple)
            {
                return true;
            }
        }
    }
    return false;
}

void Grid::SetCellType(Position gridPos, CellType type)
{
    Position pos = GetRelativePosition(gridPos);
    cells_[pos].first = type;
    // Set the cell's color based on its type
    SDL_Color color;
    switch (type) {
        case SnakeBody:
            color = { 0, 255, 0, 255 };  // Green
            break;
        case SnakeHead:
            color = { 50, 50, 100, 255 };  // Blue
            break;
        case Apple:
            color = { 255, 0, 0, 255 };  // Red
            break;
        default:
            color = { 0, 0, 0, 0 };  // Black
            break;
    }
    if (cells_[pos].second != nullptr)
    {
        cells_[pos].second->SetColor(color);
    }
}

void Grid::Render(SDL_Renderer* renderer)
{
    // Render the grid
    for (auto& pair : cells_) {
        if (pair.second.second != nullptr)
        {
            pair.second.second->Render(renderer);
        }

    }
    /*todo extract*/

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);  // Green

// Draw a dashed border around the grid
    int dashLength = 10;  // Length of dashes
    int gapLength = 10;  // Length of gaps between dashes

    // Horizontal borders (top and bottom)
    for (int x = position_.GetX(); x < position_.GetX() + dimension_.GetWidth(); x += dashLength + gapLength) {
        SDL_RenderDrawLine(renderer, x, position_.GetY(), std::min(x + dashLength, position_.GetX() + dimension_.GetWidth()), position_.GetY());  // Top border
        SDL_RenderDrawLine(renderer, x, position_.GetY() + dimension_.GetHeight() - 1, std::min(x + dashLength, position_.GetX() + dimension_.GetWidth()), position_.GetY() + dimension_.GetHeight() - 1);  // Bottom border
    }

    // Vertical borders (left and right)
    for (int y = position_.GetY(); y < position_.GetY() + dimension_.GetHeight(); y += dashLength + gapLength) {
        SDL_RenderDrawLine(renderer, position_.GetX(), y, position_.GetX(), std::min(y + dashLength, position_.GetY() + dimension_.GetHeight()));  // Left border
        SDL_RenderDrawLine(renderer, position_.GetX() + dimension_.GetWidth() - 1, y, position_.GetX() + dimension_.GetWidth() - 1, std::min(y + dashLength, position_.GetY() + dimension_.GetHeight()));  // Right border
    }

}

Position Grid::GetGridPosition(Position pos) const
{
    auto it = posToGrid_.find(pos);
    if (it != posToGrid_.end()) {
        return it->second;
    }
    else {

        return Position(-1, -1);
    }
}

Position Grid::GetRelativePosition(Position gridPos) const
{
    auto it = gridToPos_.find(gridPos);
    if (it != gridToPos_.end()) {
        return it->second;
    }
    else {
        // Error handling: grid position not found
        return Position(-1, -1);
    }
}

Position Grid::GetRandomEmptyPosition()
{
    std::vector<Position> emptyPositions;

    for (const auto& pair : cells_) {
        if (pair.second.first == CellType::Empty) {
            emptyPositions.push_back(pair.first);
        }
    }

    if (emptyPositions.empty()) {
        // Error handling: there are no empty positions on the grid
        throw std::runtime_error("No empty positions on the grid.");
    }

    // Generate a random index
    int randomIndex = std::rand() % (emptyPositions.size() - 1);

    std::cout << randomIndex << " " << GetGridPosition(emptyPositions[randomIndex]).GetX() << "," << GetGridPosition(emptyPositions[randomIndex]).GetY() << std::endl;

    return GetGridPosition(emptyPositions[randomIndex]);
}
