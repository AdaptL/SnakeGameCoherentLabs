#include "Position.h"

Position::Position(int x, int y) : x_(x), y_(y) {}

int Position::GetX() const { return x_; }
int Position::GetY() const { return y_; }

void Position::SetX(int x) { x_ = x; }
void Position::SetY(int y) { y_ = y; }

bool Position::operator<(const Position& other) const {
    if (x_ < other.x_) return true;
    if (x_ > other.x_) return false;
    if (y_ < other.y_) return true;
    return false;
}

Position Position::operator*(int factor) const {
    return Position(x_ * factor, y_ * factor);
}

Position Position::operator-(const Position& other) const {
    return Position(x_ - other.x_, y_ - other.y_);
}

bool Position::operator==(const Position& other) const {
    return x_ == other.x_ && y_ == other.y_;
}
