#include "Screen.h"

Screen::~Screen() {
    for (auto component : components_) {
        delete component;
    }
    components_.clear();
}

void Screen::Render(SDL_Renderer* renderer) {
    for (auto component : components_) {
        component->Render(renderer);
    }
}

void Screen::SetWindow(Window* window) {
    window_ = window;
}

void Screen::AddComponent(Component* component) {
    components_.push_back(component);
}
