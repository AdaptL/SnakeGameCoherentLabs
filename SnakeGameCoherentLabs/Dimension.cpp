#include "Dimension.h"

Dimension::Dimension(int width, int height) : width_(width), height_(height) {}

int Dimension::GetWidth() const { return width_; }
int Dimension::GetHeight() const { return height_; }

void Dimension::SetWidth(int width) { width_ = width; }
void Dimension::SetHeight(int height) { height_ = height; }
