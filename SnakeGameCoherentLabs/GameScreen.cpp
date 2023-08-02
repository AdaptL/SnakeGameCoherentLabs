#include "GameScreen.h"
#include "Paths.h"
void GameScreen::Enter()
{
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



    grid = new Grid(Position(0, 100), Dimension(800, 500), 50);


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

void GameScreen::Leave()
{
}

void GameScreen::Update()
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
            gameOver = true;

            gameOverUI = new GameOver(font, window_->GetRenderer(), stats->CheckHighscore());

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

void GameScreen::HandleEvents(const SDL_Event& evt)
{
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

void GameScreen::PrintScreenName()
{
    std::cout << "GameScreen" << std::endl;
}

void GameScreen::RestartGame()
{

    gameOver = false;

    
}

GameScreen::~GameScreen()
{
    delete grid;
    delete apple;
    delete gameOverUI;
    delete font;
}

void GameScreen::Restart()
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
