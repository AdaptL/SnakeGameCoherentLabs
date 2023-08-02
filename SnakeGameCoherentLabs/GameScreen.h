#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include "Screen.h"
#include "GameOver.h"
#include "Grid.h"
#include "Snake.h"
#include "Apple.h"
#include "Stats.h"
#include "TextCounter.h"
#include "Timer.h"
#include "Text.h"

class GameScreen : public Screen {
public:
    void Enter() override;
    void Leave() override;
    void Update() override;
    void HandleEvents(const SDL_Event& evt) override;
    void PrintScreenName() override;
    void RestartGame();

private:
    void Restart();

    Grid* grid = nullptr;
    Snake* snake = nullptr;
    Apple* apple = nullptr;
    GameOver* gameOverUI = nullptr;
    TTF_Font* font = nullptr;
    Uint32 lastSnakeMoveTime_ = 0;
    Uint32 snakeMoveInterval_ = 210;

    Stats* stats = nullptr;
    int applesEaten_ = 0;
    bool gameOver = false;
};
