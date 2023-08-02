#include "Window.h"

Window::Window(const std::string& title)
    : window_(nullptr), renderer_(nullptr), currentScreen_(nullptr) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        // Handle error
    }

    window_ = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, width, height, 0);
    if (!window_) {
        // Handle error
    }

    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer_) {
        // Handle error
    }
}

Window::~Window() {
    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
}

void Window::SetScreen(Screen* screen) {
    if (currentScreen_) {
        currentScreen_->Leave();
    }
    currentScreen_ = screen;
    if (currentScreen_) {
        currentScreen_->SetWindow(this);
        currentScreen_->Enter();
    }
}

SDL_Renderer* Window::GetRenderer() {
    return renderer_;
}

void Window::MainLoop() {
    bool running = true;
    while (running) {
        SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
        SDL_RenderClear(renderer_);

        SDL_Event evt;
        while (SDL_PollEvent(&evt)) {
            if (evt.type == SDL_QUIT) {
                running = false;
            }
            if (currentScreen_) {
                currentScreen_->HandleEvents(evt);
            }
        }

        if (currentScreen_) {
            currentScreen_->Update();
            currentScreen_->Render(renderer_);
        }
        SDL_RenderPresent(renderer_);
    }
}

int Window::GetWidth() {
    return width;
}

int Window::GetHeight() {
    return height;
}
