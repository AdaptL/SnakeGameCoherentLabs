#pragma once
#include "Component.h"
#include "Window.h"
#include <SDL.h>
#include <vector>

class Window;

class Screen {
public:
    virtual ~Screen();

    virtual void Enter() = 0;
    virtual void Leave() = 0;
    virtual void Update() = 0;
    virtual void HandleEvents(const SDL_Event& evt) = 0;
    virtual void PrintScreenName() = 0;
    virtual void Render(SDL_Renderer* renderer);

    void SetWindow(Window* window);
    void AddComponent(Component* component);

protected:
    Window* window_ = nullptr;
    std::vector<Component*> components_;
};
