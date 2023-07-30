#include <iostream>
#include "SDL.h"
#include "SDL_ttf.h"
#include <vector>
#include <functional>
#include <map>

class Position {
public:
    Position(int x = 0, int y = 0) : x_(x), y_(y) {}

    int GetX() const { return x_; }
    int GetY() const { return y_; }
    void SetX(int x) { x_ = x; }
    void SetY(int y) { y_ = y; }

    bool operator<(const Position& other) const {
        if (x_ < other.x_) return true;
        if (x_ > other.x_) return false;
        if (y_ < other.y_) return true;
        return false;
    }

    Position operator*(int factor) const {
        return Position(x_ * factor, y_ * factor);
    }



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
    Rectangle(Position pos = Position(), Dimension dim = Dimension(), SDL_Color color = SDL_Color{0, 0, 0, 255} )
        : position_(pos), dimension_(dim), color_{ color } {}

    Position GetPosition() const { return position_; }
    Dimension GetDimension() const { return dimension_; }
    SDL_Color GetColor() const { return color_; }

    void SetPosition(const Position& pos) { position_ = pos; }
    void SetDimension(const Dimension& dim) { dimension_ = dim; }
    virtual void SetColor(const SDL_Color& color) { color_ = color; }

    void Render(SDL_Renderer* renderer) override {
        SDL_Rect rect{ position_.GetX(), position_.GetY(), dimension_.GetWidth(), dimension_.GetHeight() };
        SDL_SetRenderDrawColor(renderer, color_.r, color_.g, color_.b, color_.a);
        SDL_RenderFillRect(renderer, &rect);
    }

protected:
    Position position_;
    Dimension dimension_;
    SDL_Color color_;
};

class GridCell : public Rectangle {
public:
    GridCell(Position pos, Dimension dim, SDL_Color color, Position gridPos, SDL_Color borderColor, int borderSize)
        : Rectangle(pos, dim, borderColor), gridPos_(gridPos) {
        inner_ = new Rectangle(Position(pos.GetX() + borderSize, pos.GetY() + borderSize),
            Dimension(dim.GetWidth() - 2 * borderSize, dim.GetHeight() - 2 * borderSize),
            color);
    }

    ~GridCell() {
        delete inner_;
    }

    Position GetGridPosition() const { return gridPos_; }
    void SetGridPosition(Position pos) { gridPos_ = pos; }

    void SetColor(const SDL_Color& color) override {
        inner_->SetColor(color);
    }

    void Render(SDL_Renderer* renderer) override {
        Rectangle::Render(renderer);
        inner_->Render(renderer);
    }

private:
    Position gridPos_;
    Rectangle* inner_;
};





class Grid : public Rectangle {
public:
    enum CellType {
        Empty,
        SnakeBody,
        SnakeHead,
        Apple
    };

public:
    ~Grid() {
        for (auto& pair : cells_) {
            delete pair.second.second; // Delete Rectangle*
        }
    }


    Grid(Position pos = Position(), Dimension dim = Dimension(), int cellSize = 1)
        : Rectangle(pos, dim), cellSize_(cellSize) {
        /* for (int x = 0; x < gridWidth; ++x) {
         for (int y = 0; y < gridHeight; ++y) {
             Position cellPos(x * cellSize, y * cellSize);
             Dimension cellDim(cellSize, cellSize);
             cells_[cellPos] = std::make_pair(Empty, Rectangle(cellPos, cellDim));
         }
     }*/
        // Initialize the grid
        gridWidth = dim.GetWidth() / cellSize;
        gridHeight = dim.GetHeight() / cellSize;


    

        int cellWidth = dim.GetWidth() / gridWidth;
        int cellHeight = dim.GetHeight() / gridHeight;
        SDL_Color transparent = { 0, 0, 0, 0 };

        this->color_ = transparent;

       /* for (int x = 0; x < gridWidth; ++x) {
            for (int y = 0; y < gridHeight; ++y) {
                


                Position cellPos(pos.GetX() + x * cellWidth, pos.GetY() + y * cellHeight);
                Position gridPos(x, y);  // The grid position
                Dimension cellDim(cellWidth, cellHeight);
                GridCell* cell = new GridCell(cellPos, cellDim, { 255 , 0, 0 , 255 }, gridPos);
                cells_[cellPos] = std::make_pair(Empty, cell);
                posToGrid_[cellPos] = gridPos;
                gridToPos_[gridPos] = cellPos;
            }
        }*/
        SDL_Color borderColor = { 0, 0, 0, 255 };  // Black
        int borderSize = 2;  // Adjust as desired
        for (int x = 0; x < gridWidth; ++x) {
            for (int y = 0; y < gridHeight; ++y) {
                Position cellPos(pos.GetX() + x * cellWidth, pos.GetY() + y * cellHeight);
                Position gridPos(x, y);  // The grid position
                Dimension cellDim(cellWidth, cellHeight);
                GridCell* cell = new GridCell(cellPos, cellDim, { 0 , 0, 0 , 255 }, gridPos, borderColor, borderSize);
                cells_[cellPos] = std::make_pair(Empty, cell);
                posToGrid_[cellPos] = gridPos;
                gridToPos_[gridPos] = cellPos;
            }
        }


    }
    void AddApple(Position gridPos) {

        SetCellType(gridPos, Grid::Apple);
    }

    int GetGridWidth() const
    {
        return gridWidth;
    }

    int GetGridHeight() const
    {
        return gridHeight;
    }



    void SetCellType(Position gridPos, CellType type) {
        Position pos = GetRelativePosition(gridPos);
        cells_[pos].first = type;
        // Set the cell's color based on its type
        SDL_Color color;
        switch (type) {
        case SnakeBody:
            color = { 0, 255, 0, 255 };  // Green
            break;
        case SnakeHead:
            color = { 0, 0, 255, 255 };  // Blue
            break;
        case Apple:
            color = { 255, 0, 0, 255 };  // Red
            break;
        default:
            color = { 0, 0, 0, 0 };  // Black
            break;
        }
        cells_[pos].second->SetColor(color);
    }

    void SetCell(Position pos, std::pair<CellType, Rectangle*> cellData) {
        cells_[pos] = cellData;
    }


    void Render(SDL_Renderer* renderer) override {
        // Render the grid
        for (auto& pair : cells_) {
            pair.second.second->Render(renderer);
        }
    }
    Position GetGridPosition(Position pos) const {
        auto it = posToGrid_.find(pos);
        if (it != posToGrid_.end()) {
            return it->second;
        }
        else {
            // Error handling: position not found in the grid
            return Position(-1, -1); 
        }
    }

    Position GetRelativePosition(Position gridPos) const {
        auto it = gridToPos_.find(gridPos);
        if (it != gridToPos_.end()) {
            return it->second;
        }
        else {
            // Error handling: grid position not found
            return Position(-1, -1); 
        }
    }


private:
    std::map<Position, std::pair<CellType, Rectangle*>> cells_ = std::map<Position, std::pair<CellType, Rectangle*>>();
    std::map<Position, Position> gridToPos_ = std::map<Position, Position>();
    std::map<Position, Position> posToGrid_ = std::map<Position, Position>();
    int gridWidth = 0;
    int gridHeight = 0;
    int cellSize_ = 0;
};


class Button : public Rectangle {
public:
    using Callback = std::function<void()>;

    Button(Position pos = Position(), Dimension dim = Dimension(), Callback callback = nullptr)
        : Rectangle(pos, dim), callback_(callback) {}

    void OnClick() {

        std::cout << "You clicked!" << std::endl;
        if (callback_) {
            callback_();
        }
    }

    void CheckClick(int mouseX, int mouseY) {
        if (mouseX >= position_.GetX() && mouseX <= position_.GetX() + dimension_.GetWidth() &&
            mouseY >= position_.GetY() && mouseY <= position_.GetY() + dimension_.GetHeight()) {
            OnClick();
        }
    }

private:
    Callback callback_;
};



class Text : public Rectangle {
public:
    Text(Position pos = Position(), Dimension dim = Dimension(), const std::string& text = "", TTF_Font* font = nullptr)
        : Rectangle(pos, dim), text_(text), font_(font) {
        color_ = { 150, 150, 150, 150 };  // Default color is white
        //UpdateTexture();  // TODO: Check if texture creation is successful
    }

    void SetText(const std::string& text, SDL_Renderer* renderer) {
        text_ = text;
        UpdateTexture(renderer);  // TODO: Check if texture creation is successful
    }

    void Render(SDL_Renderer* renderer) override {
        if (!isTextureInitialized)
        {
            UpdateTexture(renderer);
            isTextureInitialized = true;
        }

        SDL_Rect rect{ position_.GetX(), position_.GetY(), dimension_.GetWidth(), dimension_.GetHeight() };
        SDL_RenderCopy(renderer, texture_, nullptr, &rect);
    }

    ~Text() {
        SDL_DestroyTexture(texture_);
        // TODO: Handle the lifetime of font_
    }

private:
    void UpdateTexture(SDL_Renderer* renderer) {
        if (texture_) {
            SDL_DestroyTexture(texture_);
        }

        if (!font_) {
            std::cerr << "No font set for text object.\n";
            // TODO: Handle the case where font_ is null
            return;
        }

        SDL_Surface* surface = TTF_RenderText_Solid(font_, text_.c_str(), color_);
        if (!surface) {
            std::cerr << "Failed to create text surface.\n";
            // TODO: Handle this error
            return;
        }

        texture_ = SDL_CreateTextureFromSurface(renderer, surface);

        if (!texture_) {
            std::cerr << "Failed to create texture from surface.\n";
            // TODO: Handle this error
        }

        SDL_FreeSurface(surface);
    }

    std::string text_;
    TTF_Font* font_;
    SDL_Texture* texture_ = nullptr;
    bool isTextureInitialized = false;
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
    Window(const std::string& title = "Snake Game")
        : window_(nullptr), renderer_(nullptr), currentScreen_(nullptr) {
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
           // currentScreen_->PrintScreenName();
            if (currentScreen_) {
                currentScreen_->Render(renderer_);
            }
            SDL_RenderPresent(renderer_);
        }
    }

    static int GetWidth() 
    {
        return width;
    }

    static int GetHeight() 
    {
        return height;
    }

private:
    SDL_Window* window_;
    SDL_Renderer* renderer_;
    Screen* currentScreen_;
    static const int width = 800;
    static const int height = 600;
};



class GameScreen : public Screen {
public:
    // Implement the methods from Screen
    void Enter() override {
        // Create a blue rectangle in the top right corner of the screen
        /*int topRightX = window_->GetWidth() - 100;
        Rectangle* rectangle = new Rectangle(Position(topRightX, 0), Dimension(100, 100));
        rectangle->SetColor({ 0, 0, 255, 255 });
        AddComponent(rectangle);*/
        /*
        if (TTF_Init() == -1) {
            std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
            // Handle the error...
        }


        TTF_Font* font = TTF_OpenFont("E:\\RpgUnityxCourse\\QT CORE\\QT Beginners\\freedom-font\\Freedom-10eM.ttf", 50);
        if (!font) {
            std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
            // Handle the error...
        }


        Text *text = new Text(Position(100, 100), Dimension(100,100), "Hello world!", font);

        AddComponent(text);*/

       /* int topRightX = window_->GetWidth() - 100;
        Rectangle* rectangle = new Rectangle(Position(topRightX, 0), Dimension(100, 100));
        rectangle->SetColor({ 0, 0, 255, 255 });
        AddComponent(rectangle);*/


        Grid* grid = new Grid(Position(0, 100), Dimension(800, 500), 50);

        Position applePos(1, 1);
        Position applePos1(0, 1);
        Position applePos2(1, 0);
        Position applePos3(0, 0);

        grid->AddApple(applePos);
        grid->AddApple(applePos1);
        grid->AddApple(applePos2);
        grid->AddApple(applePos3);


        AddComponent(grid);



                // Create the snake
        /*for (int i = 0; i < 3; ++i) {
            Position snakePos(centerGridX + i, centerGridY);
            SDL_Color snakeColor = (i == 2) ? SDL_Color{ 0, 255, 0, 255 } : SDL_Color{ 0, 0, 255, 255 };  // Head is green, body is blue
            Rectangle snakeCell(snakePos * 10, Dimension(10, 10), snakeColor);
            grid->SetCell(snakePos, std::make_pair((i == 2) ? Grid::CellType::SnakeHead : Grid::CellType::SnakeBody, snakeCell));
        }*/

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

    void Enter() override {
        // Create a red rectangle in the center of the screen
        int centerX = window_->GetWidth() / 2 - 50;
        int centerY = window_->GetHeight() / 2 - 50;
        Rectangle* rectangle = new Rectangle(Position(centerX, centerY), Dimension(100, 100));
        rectangle->SetColor({ 255, 0, 0, 255 });
        AddComponent(rectangle);

        startButton_ = new Button(Position(100, 100), Dimension(200, 200));

        startButton_->SetColor({ 255, 0, 0, 255 });

        AddComponent(startButton_);
    }

    void Leave() override {}
    void HandleEvents(const SDL_Event& evt) override 
    {
        if (evt.type == SDL_MOUSEBUTTONDOWN) {
            if (evt.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                startButton_->CheckClick(mouseX, mouseY);
                
            }

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


private:
    Button *startButton_;
};









int main(int argc, char* argv[])
{
    Window window;
    MenuScreen menuScreen;


    window.SetScreen(&menuScreen);
    window.MainLoop();


    return 0;
}
