#pragma once

class Dimension {
public:
    Dimension(int width = 0, int height = 0);

    int GetWidth() const;
    int GetHeight() const;
    void SetWidth(int width);
    void SetHeight(int height);

private:
    int width_;
    int height_;
};
