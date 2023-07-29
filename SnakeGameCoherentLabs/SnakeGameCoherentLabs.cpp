#include <iostream>
#include "SDL.h"
#include <vector>


class Position {
public:
    Position(int x = 0, int y = 0) : x_(x), y_(y) {}

    int GetX() const { return x_; }
    int GetY() const { return y_; }
    void SetX(int x) { x_ = x; }
    void SetY(int y) { y_ = y; }

private:
    int x_;
    int y_;
};

class Dimension {
public:
    Dimension(int width = 0, int height = 0) : width_(width), height_(height) {}

    int GetWidth() const { return width_; }
    int GetHeight() const { return height_; }
    void SetWidth(int width) { width_ = width; }
    void SetHeight(int height) { height_ = height; }

private:
    int width_;
    int height_;
};

class Component {
public:
    virtual ~Component() = default;

    virtual void Render(SDL_Renderer* renderer) = 0;
};

class Rectangle : public Component {
public:
    Rectangle(Position pos = Position(), Dimension dim = Dimension())
        : position_(pos), dimension_(dim), color_{ 0, 0, 0, 255 } {}

    Position GetPosition() const { return position_; }
    Dimension GetDimension() const { return dimension_; }
    SDL_Color GetColor() const { return color_; }

    void SetPosition(const Position& pos) { position_ = pos; }
    void SetDimension(const Dimension& dim) { dimension_ = dim; }
    void SetColor(const SDL_Color& color) { color_ = color; }

    void Render(SDL_Renderer* renderer) override {
        SDL_Rect rect{ position_.GetX(), position_.GetY(), dimension_.GetWidth(), dimension_.GetHeight() };
        SDL_SetRenderDrawColor(renderer, color_.r, color_.g, color_.b, color_.a);
        SDL_RenderFillRect(renderer, &rect);
    }

private:
    Position position_;
    Dimension dimension_;
    SDL_Color color_;
};




class Window;

class Screen {
public:
    ~Screen() {
        for (auto component : components_) {
            delete component;
        }
        components_.clear();
    }

    // Called when entering the screen
    virtual void Enter() = 0;

    // Called when leaving the screen
    virtual void Leave() = 0;

    // Handle SDL events
    virtual void HandleEvents(const SDL_Event& evt) = 0;

    virtual void PrintScreenName() = 0;

    // Render the screen's content
    virtual void Render(SDL_Renderer* renderer) {
        for (auto component : components_) {
            component->Render(renderer);
        }
    }

    void SetWindow(Window* window) {
        window_ = window;
    }

    void AddComponent(Component* component) {
        components_.push_back(component);
    }

protected:
    Window* window_ = nullptr;
    std::vector<Component*> components_;
};

class Window {
public:
    Window(const std::string& title = "Snake Game",
        int width = 800,
        int height = 600)
        : window_(nullptr), renderer_(nullptr), currentScreen_(nullptr), width(width), height(height) {
        // Initialize SDL
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            // Handle error...
        }

        // Create the window
        window_ = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED, width, height, 0);
        if (!window_) {
            // Handle error...
        }

        // Create the renderer
        renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer_) {
            // Handle error...
        }
    }

    ~Window() {
        SDL_DestroyRenderer(renderer_);
        SDL_DestroyWindow(window_);
        SDL_Quit();
    }

    void SetScreen(Screen* screen) {
        if (currentScreen_) {
            currentScreen_->Leave();
        }
        currentScreen_ = screen;
        if (currentScreen_) {
            currentScreen_->SetWindow(this);
            currentScreen_->Enter();
        }
    }

    void MainLoop() {
        bool running = true;
        while (running) {
            //Clear screen
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
            currentScreen_->PrintScreenName();
            if (currentScreen_) {
                currentScreen_->Render(renderer_);
            }
            SDL_RenderPresent(renderer_);
        }
    }

    int GetWidth()
    {
        return width;
    }

    int GetHeight()
    {
        return height;
    }

private:
    SDL_Window* window_;
    SDL_Renderer* renderer_;
    Screen* currentScreen_;
    int width;
    int height;
};



class GameScreen : public Screen {
public:
    // Implement the methods from Screen
    void Enter() override {
        // Create a blue rectangle in the top right corner of the screen
        int topRightX = window_->GetWidth() - 100;
        Rectangle* rectangle = new Rectangle(Position(topRightX, 0), Dimension(100, 100));
        rectangle->SetColor({ 0, 0, 255, 255 });
        AddComponent(rectangle);
    }
    void Leave() override {}
    void HandleEvents(const SDL_Event& evt) override {}
    void PrintScreenName() override
    {
        std::cout << "GameScreen" << std::endl;
    }


};


class MenuScreen : public Screen {
public:
    // Implement the methods from Screen
    void Enter() override {
        // Create a red rectangle in the center of the screen
        int centerX = window_->GetWidth() / 2 - 50;
        int centerY = window_->GetHeight() / 2 - 50;
        Rectangle* rectangle = new Rectangle(Position(centerX, centerY), Dimension(100, 100));
        rectangle->SetColor({ 255, 0, 0, 255 });
        AddComponent(rectangle);
    }

    void Leave() override {}
    void HandleEvents(const SDL_Event& evt) override 
    {
        if (evt.type == SDL_MOUSEBUTTONDOWN) {
            // Here you would typically check if the mouse button down event occurred within
            // the boundaries of your Button. If it did, you would switch to the GameScreen.
            // Since the Button class isn't implemented yet, I'm leaving this as a comment.
            // if (startButton->Contains(evt.button.x, evt.button.y)) {
            //     Window* window = ...;  // Obtain a pointer to the Window object
            //     window->SetScreen(new GameScreen());
            // }
        }
        else if (evt.type == SDL_KEYDOWN) {
            if (evt.key.keysym.sym == SDLK_RETURN) {
                if (window_) {
                    window_->SetScreen(new GameScreen());
                }
            }
        }
    }
    void PrintScreenName() override
    {
        std::cout << "MenuScreen" << std::endl;
    }
};









int main(int argc, char* argv[])
{
    Window window;
    MenuScreen menuScreen;


    window.SetScreen(&menuScreen);
    window.MainLoop();


    return 0;
}
