#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <functional>
#include <map>
#include <deque>
#include <unordered_set>
#include <cstdlib> 
#include <string>
#include <ostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <ostream>
#include <filesystem>
#include "SDL_image.h"

static const std::string  FONT_PATH      = "..\\resources\\Fonts\\super-legend-boy-font\\SuperLegendBoy-4w8Y.ttf";
static const std::string  IMAGE_PATH     = "..\\resources\\images\\snake_image.png";



static const std::string FONT_PATH_EXE = (std::filesystem::current_path() / "resources" / "Fonts" / "super-legend-boy-font" / "SuperLegendBoy-4w8Y.ttf").string();
static const std::string IMAGE_PATH_EXE = (std::filesystem::current_path() / "resources" / "Images" / "snake_image.png").string();


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

    Position operator-(const Position& other) const {
        return Position(x_ - other.x_, y_ - other.y_);
    }

    bool operator==(const Position& other) const {
        return x_ == other.x_ && y_ == other.y_;
    }

private:
    int x_;
    int y_;
};

struct PositionHash {
    std::size_t operator() (const Position& pos) const {
        return std::hash<int>()(pos.GetX()) ^ std::hash<int>()(pos.GetY());
    }
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

class Image : public Rectangle {
public:
    Image(Position pos = Position(), Dimension dim = Dimension(), const std::string& imagePath = "", SDL_Renderer* renderer = nullptr, float size = 1)
        : Rectangle(pos, dim), imagePath_(imagePath), renderer_(renderer), size_(size) {

        SDL_Surface* loadedSurface = IMG_Load(imagePath_.c_str());
        if (loadedSurface == nullptr) {

            loadedSurface = IMG_Load(IMAGE_PATH_EXE.c_str());
            if (loadedSurface == nullptr)
            {
                std::cerr << "Failed to load image. SDL_image Error: " << IMG_GetError() << std::endl;
                return;
            }
            
            
        }

        texture_ = SDL_CreateTextureFromSurface(renderer_, loadedSurface);
        if (texture_ == nullptr) {
            std::cerr << "Failed to create texture from loaded image. SDL Error: " << SDL_GetError() << std::endl;
        }

        SDL_FreeSurface(loadedSurface);
    }

    void Render(SDL_Renderer* renderer) override {

        dimension_.SetWidth(static_cast<int>(dimension_.GetWidth() * size_));
        dimension_.SetHeight(static_cast<int>(dimension_.GetHeight() * size_));

        SDL_Rect rect{ position_.GetX(), position_.GetY(), dimension_.GetWidth(), dimension_.GetHeight() };
        SDL_RenderCopy(renderer, texture_, nullptr, &rect);
    }

    ~Image() {
        // Deallocate texture
        SDL_DestroyTexture(texture_);
        texture_ = nullptr;
    }

private:
    std::string imagePath_;
    SDL_Renderer* renderer_;
    SDL_Texture* texture_ = nullptr;
    float size_;
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

        // Initialize the grid
        gridWidth = dim.GetWidth() / cellSize;
        gridHeight = dim.GetHeight() / cellSize;

        int cellWidth = dim.GetWidth() / gridWidth;
        int cellHeight = dim.GetHeight() / gridHeight;
        SDL_Color transparent = { 0, 0, 0, 0 };

        this->color_ = transparent;


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


    CellType GetCellTypeAtCoordinate(Position gridPos)
    {
        auto relativePos = GetRelativePosition(gridPos);
        return cells_[relativePos].first;
    }

    int GetGridWidth() const
    {
        return gridWidth;
    }

    int GetGridHeight() const
    {
        return gridHeight;
    }

    void ClearGrid()
    {
        for (int i = 0; i < GetGridWidth(); ++i)
        {
            for (int j = 0; j < GetGridHeight(); ++j)
            {
                SetCellType(Position(i, j), CellType::Empty);
            }
        }
    }
    /*to fix a bug*/
    bool AppleExists()
    {
        for (int i = 0; i < GetGridWidth(); ++i)
        {
            for (int j = 0; j < GetGridHeight(); ++j)
            {
                if (GetCellTypeAtCoordinate({ i,j }) == CellType::Apple)
                {
                    return true;
                }
            }
        }
        return false;
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
            color = { 50, 50, 100, 255 };  // Blue
            break;
        case Apple:
            color = { 255, 0, 0, 255 };  // Red
            break;
        default:
            color = { 0, 0, 0, 0 };  // Black
            break;
        }
        if (cells_[pos].second != nullptr)
        {
            cells_[pos].second->SetColor(color);
        }

       
    }


    void Render(SDL_Renderer* renderer) override {
        // Render the grid
        for (auto& pair : cells_) {
            if (pair.second.second != nullptr)
            {
                pair.second.second->Render(renderer);
            }
            
        }
        /*todo extract*/

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);  // Green

    // Draw a dashed border around the grid
        int dashLength = 10;  // Length of dashes
        int gapLength = 10;  // Length of gaps between dashes

        // Horizontal borders (top and bottom)
        for (int x = position_.GetX(); x < position_.GetX() + dimension_.GetWidth(); x += dashLength + gapLength) {
            SDL_RenderDrawLine(renderer, x, position_.GetY(), std::min(x + dashLength, position_.GetX() + dimension_.GetWidth()), position_.GetY());  // Top border
            SDL_RenderDrawLine(renderer, x, position_.GetY() + dimension_.GetHeight() - 1, std::min(x + dashLength, position_.GetX() + dimension_.GetWidth()), position_.GetY() + dimension_.GetHeight() - 1);  // Bottom border
        }

        // Vertical borders (left and right)
        for (int y = position_.GetY(); y < position_.GetY() + dimension_.GetHeight(); y += dashLength + gapLength) {
            SDL_RenderDrawLine(renderer, position_.GetX(), y, position_.GetX(), std::min(y + dashLength, position_.GetY() + dimension_.GetHeight()));  // Left border
            SDL_RenderDrawLine(renderer, position_.GetX() + dimension_.GetWidth() - 1, y, position_.GetX() + dimension_.GetWidth() - 1, std::min(y + dashLength, position_.GetY() + dimension_.GetHeight()));  // Right border
        }
    }
    Position GetGridPosition(Position pos) const {
        auto it = posToGrid_.find(pos);
        if (it != posToGrid_.end()) {
            return it->second;
        }
        else {

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
    Position GetRandomEmptyPosition() {
        std::vector<Position> emptyPositions;

        for (const auto& pair : cells_) {
            if (pair.second.first == CellType::Empty) {
                emptyPositions.push_back(pair.first);
            }
        }

        if (emptyPositions.empty()) {
            // Error handling: there are no empty positions on the grid
            throw std::runtime_error("No empty positions on the grid.");
        }

        // Generate a random index
        int randomIndex = std::rand() % (emptyPositions.size() - 1);

        std::cout << randomIndex << " " << GetGridPosition(emptyPositions[randomIndex]).GetX() << "," << GetGridPosition(emptyPositions[randomIndex]).GetY() << std::endl;
             
        return GetGridPosition(emptyPositions[randomIndex]);
    }


private:
    std::map<Position, std::pair<CellType, Rectangle*>> cells_ = std::map<Position, std::pair<CellType, Rectangle*>>();
    std::map<Position, Position> gridToPos_ = std::map<Position, Position>();
    std::map<Position, Position> posToGrid_ = std::map<Position, Position>();
    int gridWidth = 0;
    int gridHeight = 0;
    int cellSize_ = 0;
};

class Snake {
public:
    enum Direction {
        Up,
        Down,
        Left,
        Right
    };

public:
    Snake(Grid* grid, Position startingPos = Position(3,3), Direction startingDir = Right)
        : grid_(grid), direction_(startingDir) {
        // Starting length is 1
        startingPos_ = startingPos;

        bodyPositions_.push_front(startingPos);

        // Initially set the head on the grid
        grid_->SetCellType(startingPos, Grid::SnakeHead);
    }

    bool hasCollided()
    {
        return hasCollided_;
    }

    bool Move(bool ateApple = false) {
        // Calculate the new head position
        Position newHeadPos = GetNextHeadPosition();

        // Check if the new position is valid (inside the grid and not colliding with the snake's body)
        if (!IsValidMove(newHeadPos)) {
            // Handle game over

            std::cout << "Collision! Game over" << std::endl;

            hasCollided_ = true;
            return false;
        }

        // Add the new head position to the body
        bodyPositions_.push_front(newHeadPos);
        occupied_positions_.insert(newHeadPos);

        // If the snake didn't eat an apple, remove the last body segment
        if (!ateApple) {
            Position tailPos = bodyPositions_.back();
            bodyPositions_.pop_back();
            occupied_positions_.erase(tailPos);
        }
    

        // Set the new cell type on the grid
        grid_->SetCellType(newHeadPos, Grid::SnakeHead);

        hasMoved_ = true;

        return true;
    }

    void ClearSnakeFromGrid() {

        for (const auto& pos : bodyPositions_) {
            grid_->SetCellType(pos, Grid::Empty);
        }
    }

    void AddSnakeToGrid() {
        // Set all body positions to SnakeBody

        for (auto it = bodyPositions_.begin() + 1; it != bodyPositions_.end(); ++it) {
            grid_->SetCellType(*it, Grid::SnakeBody);
        }

        // Set the head position to SnakeHead
        if (!bodyPositions_.empty()) {
            grid_->SetCellType(bodyPositions_.front(), Grid::SnakeHead);
        }
        
        
    }

    void RestartSnake()
    {
        hasCollided_ = false;
        hasMoved_ = true;

        bodyPositions_.clear();
        occupied_positions_.clear();

        direction_ = Direction::Right;
        bodyPositions_.push_front(startingPos_);

        grid_->SetCellType(startingPos_, Grid::SnakeHead);

        IncreaseLength();
        IncreaseLength();

        
    }

    void UpdateDirection(Direction newDirection) {

        if (hasMoved_)
        {
            switch (newDirection) {
            case Up:
                if (direction_ != Down) {
                    direction_ = newDirection;
                }
                break;
            case Down:
                if (direction_ != Up) {
                    direction_ = newDirection;
                }
                break;
            case Left:
                if (direction_ != Right) {
                    direction_ = newDirection;
                }
                break;
            case Right:
                if (direction_ != Left) {
                    direction_ = newDirection;
                }
                break;
            }
            hasMoved_ = false;
        }

        
    }

    void IncreaseLength() {
        // Calculate new head position
        Position newHead = GetNextHeadPosition();
        bool isWithinGrid = (newHead.GetX() >= 0 || newHead.GetX() < grid_->GetGridWidth()) && (newHead.GetY() >= 0 || newHead.GetY() < grid_->GetGridHeight());
     //   auto isOccupied = occupied_positions_.find(newHead) != occupied_positions_.end();

        // Check if the new head position is within the grid and not occupied
        if (isWithinGrid) {
            // Add new head to the front of bodyPositions_
            bodyPositions_.push_front(newHead);
            // Also add the new position to the set of occupied positions
            occupied_positions_.insert(newHead);
            // Don't remove the last body segment, causing the snake to grow in length
        }
        // If the new head position is not within the grid or is occupied, do nothing
    }

    Position GetNextHeadPosition() {
       

        Position currentHeadPos = bodyPositions_.front();

        switch (direction_) {
        case Up:
            return Position(currentHeadPos.GetX(), currentHeadPos.GetY() - 1);
        case Down:
            return Position(currentHeadPos.GetX(), currentHeadPos.GetY() + 1);
        case Left:
            return Position(currentHeadPos.GetX() - 1, currentHeadPos.GetY());
        case Right:
            return Position(currentHeadPos.GetX() + 1, currentHeadPos.GetY());
        }
    }

    bool hasMoved()
    {
        return hasMoved_;
    }

private:
    Grid* grid_; // Pointer to the grid the snake is on
    std::deque<Position> bodyPositions_; // Positions of the snake's body segments
    Direction direction_; // Current direction of movement
    std::unordered_set<Position, PositionHash> occupied_positions_;
    bool hasMoved_ = false;
    bool hasCollided_ = false;
    Position startingPos_;


    bool IsValidMove(Position newPos) {

        if (hasCollided_)
        {
            return true;
        }

        // Check if position is within the grid
        if (newPos.GetX() < 0 || newPos.GetX() >= grid_->GetGridWidth() || newPos.GetY() < 0 || newPos.GetY() >= grid_->GetGridHeight()) {
            return false;
        }

        // Check if position is occupied by the snake's body
        if (occupied_positions_.find(newPos) != occupied_positions_.end()) {
            return false;
        }

        return true;
    }

};

class Apple {
public:
    Apple(Grid* grid) : grid_(grid) {
    }

    void ClearApple() {
        grid_->SetCellType(applePosition_, Grid::CellType::Empty);
    }

    void SpawnApple() {


        applePosition_ = grid_->GetRandomEmptyPosition();
        grid_->SetCellType(applePosition_, Grid::CellType::Apple);
    }

    Position GetPosition() {
        return applePosition_;
    }

private:
    Grid* grid_;
    Position applePosition_;
};

class Text : public Rectangle {
public:
    Text(Position pos = Position(), Dimension dim = Dimension(), const std::string& text = "", TTF_Font* font = nullptr)
        : Rectangle(pos, dim), text_(text), font_(font) {
        color_ = { 0, 255, 0, 255 };  // Default color is white
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

protected:
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



class Button : public Rectangle {
public:
    using Callback = std::function<void()>;

    Button(Position pos = Position(), Dimension dim = Dimension(), Callback callback = nullptr, TTF_Font* font = nullptr, const std::string& text = "")
        : Rectangle(pos, dim), callback_(callback) {


        Position textPos = Position(pos.GetX() + (dim.GetWidth()/4), pos.GetY() + (dim.GetHeight() / 4));
        Dimension textDim = Dimension(dim.GetWidth() /2 , dim.GetHeight() / 2);
        text_ = Text(textPos, textDim, text, font);
        color_ = { 0, 255, 0, 255 };  // Green border color
        text_.SetColor({ 0, 255, 0, 255 });  // Green text color
    }


    void OnClick() {
        std::cout << "You clicked!" << std::endl;
        if (callback_) {
            callback_();
        }
    }

    bool CheckClick(int mouseX, int mouseY) {
        if (mouseX >= position_.GetX() && mouseX <= position_.GetX() + dimension_.GetWidth() &&
            mouseY >= position_.GetY() && mouseY <= position_.GetY() + dimension_.GetHeight()) {
            OnClick();
            return true;
        }
    }

    void Render(SDL_Renderer* renderer) override {
        SDL_Rect rect{ position_.GetX(), position_.GetY(), dimension_.GetWidth(), dimension_.GetHeight() };

        // Render black filled rectangle
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);

        // Render green border
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderDrawRect(renderer, &rect);

        // Render text
        text_.Render(renderer);
    }

private:
    Callback callback_;
    Text text_;
};







class TextCounter : public Text {
public:
    TextCounter(Position pos = Position(), Dimension dim = Dimension(), TTF_Font* font = nullptr, SDL_Renderer* renderer = nullptr)
        : Text(pos, dim, "0", font), renderer_(renderer), count_(0) {}

    void Increment() {
        ++count_;
        SetText(initialText_ + std::to_string(count_), renderer_);
    }

    void Decrement() {
        if (count_ > 0) {
            --count_;
            SetText(initialText_ + std::to_string(count_), renderer_);
        }
    }

    int GetCounter()
    {
        return count_;
    }

    void Reset()
    {
        count_ = 0;
        SetText(initialText_ + std::to_string(count_), renderer_);
        Render(renderer_);
    }

private:
    int count_;
    std::string initialText_;
    SDL_Renderer* renderer_;
};


class Timer : public Text {
public:
    Timer(Position pos = Position(), Dimension dim = Dimension(), TTF_Font* font = nullptr, SDL_Renderer* renderer = nullptr)
        : Text(pos, dim, "00:00", font), renderer_(renderer), lastUpdateTime_(SDL_GetTicks()), seconds_(0), minutes_(0) {}

    void Update() {
        Uint32 current_time = SDL_GetTicks();
        if (current_time > lastUpdateTime_ + 1000) { // 1000 milliseconds = 1 second
            ++seconds_;
            if (seconds_ >= 60) {
                ++minutes_;
                seconds_ = 0;
            }
            SetText(GetFormattedTime(), renderer_);
            lastUpdateTime_ = current_time;
        }
    }

    void Reset()
    {
        seconds_ = 0;
        minutes_ = 0;
        Render(renderer_);
    }

private:
    std::string GetFormattedTime() const {
        std::ostringstream oss;
        oss << std::setw(2) << std::setfill('0') << minutes_ << ":" << std::setw(2) << std::setfill('0') << seconds_;
        return oss.str();
    }

    Uint32 lastUpdateTime_;
    int seconds_;
    int minutes_;
    SDL_Renderer* renderer_;
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

    virtual void Update() = 0;

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

    SDL_Renderer* GetRenderer()
    {
        return renderer_;
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

            currentScreen_->Update();



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

class Stats : public Component {
public:
    Stats(SDL_Renderer* renderer, TTF_Font* font) : renderer_(renderer), font_(font) {

        counterText_ = new Text(Position(35, 25), Dimension(250, 50), "Apples eaten :", font);
        appleCounter_ = new TextCounter(Position(295, 35), Dimension(40, 40), font, renderer);

        timerText_ = new Text(Position(400, 25), Dimension(250, 50), "Time passed :", font);
        gameTimer_ = new Timer(Position(660, 27), Dimension(100, 50), font, renderer);

        timerText_->SetColor({ 50, 50, 100, 255 });
        gameTimer_->SetColor({ 50, 50, 100, 255 });


        LoadHighscore();

        oldHighScore_ = highscore_;
    }

    ~Stats() {
        delete counterText_;
        delete appleCounter_;
        delete timerText_;
        delete gameTimer_;
    }

    void IncreaseAppleCount() {
        appleCounter_->Increment();
    }

    void UpdateTimer() {
        gameTimer_->Update();
    }

    void ResetTimer()
    {
        gameTimer_->Reset();
    }

    void ResetAppleCounter()
    {
        appleCounter_->Reset();
    }


    int GetAppleCount()
    {
        return appleCounter_->GetCounter();
    }

    void CheckHighscore() {
        if (appleCounter_->GetCounter() > highscore_) {
            highscore_ = appleCounter_->GetCounter();
            SaveHighscore();
        }
    }

    void Render(SDL_Renderer* renderer) override {
        counterText_->Render(renderer);
        appleCounter_->Render(renderer);
        timerText_->Render(renderer);
        gameTimer_->Render(renderer);
    }

    static int GetHighscore() {
        std::ifstream highscoreFile("highscore.txt");
        int highscore;
        if (highscoreFile.is_open()) {
            highscoreFile >> highscore;
        }
        else {
            // File not found, so highscore is 0
            highscore = 0;
        }
        highscoreFile.close();
        return highscore;
    }

    static bool NewHighScore(int newHighScore)
    {
        if (newHighScore >= GetHighscore() && newHighScore > oldHighScore_)
        {
            oldHighScore_ = newHighScore;
            return true;
        }
        return false;
    }

private:
    void LoadHighscore() {
        std::ifstream file("highscore.txt");

        if (file.is_open()) {
            file >> highscore_;

        }
        else {
            highscore_ = 0;
            SaveHighscore();  // Creates the file if it doesn't exist
        }
    }

    void SaveHighscore() {
        std::ofstream file("highscore.txt");
        if (file.is_open()) {
            file << highscore_;
        }
    }

    int highscore_;
    static int oldHighScore_;
    Text* counterText_;
    TextCounter* appleCounter_;
    Text* timerText_;
    Timer* gameTimer_;
    SDL_Renderer* renderer_;
    TTF_Font* font_;
};
int Stats::oldHighScore_ = 0;

class GameOver : public Component {
public:
    GameOver(TTF_Font* font, SDL_Renderer* renderer)
        : gameOverText_(Position(110, 200), Dimension(600, 75), "Game Over", font),
        highScoreText_(Position(260, 290), Dimension(300, 40), "", font),
        restartButton_(Position(300, 350), Dimension(200, 50), nullptr, font, "Restart"),
        background_(Position(2, 102), Dimension(796, 496), { 0, 0, 0, 255 }),
        font_(font),
        renderer_(renderer) {}

    void Render(SDL_Renderer* renderer) override {
        if (gameOver_) {
            background_.Render(renderer);
            gameOverText_.Render(renderer);
            if (Stats::NewHighScore(GetHighScore())) {
                highScoreText_.SetText("New high score: " + std::to_string(Stats::GetHighscore()), renderer);
                highScoreText_.Render(renderer);
            }
            else
            {
                highScoreText_.SetText("Score: " + std::to_string(GetHighScore()), renderer);
                highScoreText_.Render(renderer);
            }
            restartButton_.Render(renderer);
        }
    }

    void CheckClick(int mouseX, int mouseY) {
        if (gameOver_) {
            restartButton_.CheckClick(mouseX, mouseY);
        }
    }

    void StartGameOver() {
        gameOver_ = true;
    }

    void StopGameOver() {
        gameOver_ = false;
    }

    void SetHighScore(int highScore)
    {
        highScore_ = highScore;
    }

    int GetHighScore()
    {
        return highScore_;
    }
private:
    bool gameOver_ = false;
    int  highScore_ = 0;
    Text gameOverText_;
    Text highScoreText_;
    Button restartButton_;
    Rectangle background_;
    TTF_Font* font_;
    SDL_Renderer* renderer_;
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
        

        /*getcurrenthighest score*/

        if (TTF_Init() == -1) {
            std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
      
        }



        font = TTF_OpenFont(FONT_PATH.c_str(), 45);
       
        if (!font) {

  
            font = TTF_OpenFont(FONT_PATH_EXE.c_str(), 45);
            if (!font)
            {
                std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
                return;
            }
        }

        stats = new Stats(window_->GetRenderer(), font);


        AddComponent(stats);

        


       /* Text* counterText = new Text(Position(35, 25), Dimension(250, 50), "Apples eaten :", font);

        appleCounter = new TextCounter(Position(295, 35), Dimension(40, 40), font, window_->GetRenderer());
        


        Text* timerText = new Text(Position(400, 25), Dimension(250, 50), "Time passed :", font);

        gameTimer = new Timer(Position(660, 27), Dimension(100, 50), font, window_->GetRenderer());


        timerText->SetColor({ 50, 50, 100, 255 });
        gameTimer->SetColor({ 50, 50, 100, 255 });

        AddComponent(appleCounter);
        AddComponent(gameTimer);
        AddComponent(counterText);
        AddComponent(timerText);*/
       /* int topRightX = window_->GetWidth() - 100;
        Rectangle* rectangle = new Rectangle(Position(topRightX, 0), Dimension(100, 100));
        rectangle->SetColor({ 0, 0, 255, 255 });
        AddComponent(rectangle);*/


        grid = new Grid(Position(0, 100), Dimension(800, 500), 50);

        // Inside Enter function
        snake = new Snake(grid);

        apple = new Apple(grid);



        snake->IncreaseLength();
        snake->IncreaseLength();



        apple->SpawnApple();

        if (!grid->AppleExists())
        {
            apple->SpawnApple();
        }

        AddComponent(grid);

    }
    void Leave() override {}

    void RestartGame()
    {
        gameOver = false;
        /*clear up old snake, clear up it's space*/
        /*init snake again*/
    }
    void Update() override
    {
        if (gameOver)
        {
            return;
        }


        Uint32 currentTime = SDL_GetTicks();
        bool appleEaten = false;
       // gameTimer->Update();

        stats->UpdateTimer();

        if (currentTime - lastSnakeMoveTime_ >= snakeMoveInterval_) {

            snake->ClearSnakeFromGrid();

            auto snakeNewPos = snake->GetNextHeadPosition();

            if (grid->GetCellTypeAtCoordinate(snakeNewPos) == Grid::CellType::Apple)
            {
                appleEaten = true;
                apple->ClearApple();

            }


            if (!snake->Move(appleEaten))
            {
                


                stats->CheckHighscore();


                gameOver = true;

                gameOverUI = new GameOver(font, window_->GetRenderer());

                gameOverUI->StartGameOver();

                gameOverUI->SetHighScore(stats->GetAppleCount());

                AddComponent(gameOverUI);


                return;
            }


            snake->AddSnakeToGrid();

            if (appleEaten)
            {
                apple->SpawnApple();

                applesEaten_++;

                stats->IncreaseAppleCount();


                if (applesEaten_ == 1 || applesEaten_ == 4)
                {
                    snakeMoveInterval_ *= 0.8;
                }
                else if (applesEaten_ == 10)
                {
                    snakeMoveInterval_ *= 0.9;
                }


                appleEaten = false;
            }
            lastSnakeMoveTime_ = currentTime;
        }
    }



    void HandleEvents(const SDL_Event& evt) override {
        if (evt.type == SDL_KEYDOWN) {
            switch (evt.key.keysym.sym) {
            case SDLK_UP:
                snake->UpdateDirection(Snake::Direction::Up);
                break;
            case SDLK_DOWN:
                snake->UpdateDirection(Snake::Direction::Down);
                break;
            case SDLK_LEFT:
                snake->UpdateDirection(Snake::Direction::Left);
                break;
            case SDLK_RIGHT:
                snake->UpdateDirection(Snake::Direction::Right);
                break;
            }
        }
        else if (evt.type == SDL_MOUSEBUTTONDOWN) {
            // If the game is over, check if the restart button is clicked
            if (gameOver) {
                if (gameOverUI != nullptr)
                {

                    gameOverUI->CheckClick(evt.button.x, evt.button.y);




                    Restart();

                }
            }
        }
    }

    void Restart()
    {
 
      //  grid->ClearGrid();

        snakeMoveInterval_ = 210;

        applesEaten_ = 0;

        apple->ClearApple();
        applesEaten_ = false;

        gameOverUI->StopGameOver();

        stats->ResetAppleCounter();
        stats->ResetTimer();

        snake->RestartSnake();


        apple->SpawnApple();

        if (!grid->AppleExists())
        {
            apple->SpawnApple();
        }

        gameOver = false;
    }


    void PrintScreenName() override
    {
        std::cout << "GameScreen" << std::endl;
    }

private:
    Grid* grid = nullptr;
    Snake* snake = nullptr;
    Apple* apple = nullptr;
    GameOver* gameOverUI = nullptr;
    TTF_Font* font = nullptr;

    Uint32 lastSnakeMoveTime_ = 0;
    Uint32 snakeMoveInterval_ = 210;
   // Timer* gameTimer = nullptr;
   // TextCounter* appleCounter = nullptr;
    Stats* stats = nullptr;
    int applesEaten_ = 0;
    bool gameOver = false;
};
class MenuUI : public Component {
public:
    MenuUI(TTF_Font* font, SDL_Renderer* renderer, const std::string& imagePath)
        : title_(Position(90, 100), Dimension(600, 150), "Snake", font),
        createdByText_(Position(220, 250), Dimension(400, 40), "Created by Viktor Chochkov for Coherent Labs", font),
        startButton_(Position(290, 350), Dimension(250, 75), nullptr, font, "Start"),
        snakeImage_(Position(690, 133), Dimension(90, 90), imagePath, renderer),
        renderer_(renderer) {
        // Set green color for texts
        title_.SetColor({ 0, 255, 0, 255 });
        createdByText_.SetColor({ 50, 50, 100, 255 });

    }

    void Render(SDL_Renderer* renderer) override {
        title_.Render(renderer);
        createdByText_.Render(renderer);
        startButton_.Render(renderer);
        snakeImage_.Render(renderer);
    }

    bool CheckClick(int mouseX, int mouseY) {
        return startButton_.CheckClick(mouseX, mouseY);
    }

private:
    Text title_;
    Text createdByText_;
    Button startButton_;
    Image snakeImage_;
    SDL_Renderer* renderer_;
};


class MenuScreen : public Screen {
public:

    void Enter() override {
        // Create a red rectangle in the center of the screen
        if (TTF_Init() == -1) {
            std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
            return;
        }


        // Load font
        TTF_Font* font = TTF_OpenFont(FONT_PATH.c_str(), 28);  // Replace path_to_font.ttf with path to your .ttf file
        if (!font) {

            font = TTF_OpenFont(FONT_PATH_EXE.c_str(), 28);
            if (!font)
            {
                std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
                return;
            }
            
        }





        // Initialize MenuUI
        menuUI = new MenuUI(font, window_->GetRenderer(), IMAGE_PATH);

        AddComponent(menuUI);
    }

    void Leave() override {}

    void Update() override
    {

    }
    void HandleEvents(const SDL_Event& evt) override
    {

        if (evt.type == SDL_MOUSEBUTTONDOWN) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            if (menuUI->CheckClick(mouseX, mouseY))
            {
                window_->SetScreen(new GameScreen());
            }
            
        }
       /* else if (evt.type == SDL_KEYDOWN) {
            if (evt.key.keysym.sym == SDLK_RETURN) {
                if (window_) {
                    window_->SetScreen(new GameScreen());
                }
            }
        }*/

    }
    void PrintScreenName() override
    {
        std::cout << "MenuScreen" << std::endl;
    }


private:
    MenuUI* menuUI;
};









int main(int argc, char* argv[])
{
    std::srand(std::time(nullptr));
    std::cout << "Current path is " << std::filesystem::current_path() << '\n';

    Window window;
    MenuScreen menuScreen;


    window.SetScreen(&menuScreen);
    window.MainLoop();

    return 0;
}
