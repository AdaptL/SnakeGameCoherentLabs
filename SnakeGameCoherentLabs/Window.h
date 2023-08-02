#pragma once
#include <SDL.h>
#include <string>
#include "Screen.h"
class Screen;
class Window {
public:
    Window(const std::string& title = "Snake Game");
    ~Window();

    void SetScreen(Screen* screen);
    SDL_Renderer* GetRenderer();
    void MainLoop();
    static int GetWidth();
    static int GetHeight();

private:
    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    Screen *currentScreen_ = nullptr;
    static const int width = 800;
    static const int height = 600;
};
