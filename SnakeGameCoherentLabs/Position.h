#pragma once
#include <cstddef>
#include <functional>

class Position {
public:
    Position(int x = 0, int y = 0);

    int GetX() const;
    int GetY() const;
    void SetX(int x);
    void SetY(int y);

    bool operator<(const Position& other) const;
    Position operator*(int factor) const;
    Position operator-(const Position& other) const;
    bool operator==(const Position& other) const;

private:
    int x_;
    int y_;
};

struct PositionHash {
    std::size_t operator() (const Position& pos) const {
        return std::hash<int>()(pos.GetX()) ^ std::hash<int>()(pos.GetY());
    }
};