#pragma once
#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <fstream>
#include "Component.h"
#include "Text.h"
#include "TextCounter.h"
#include "Timer.h"

class Stats : public Component {
public:
    Stats(SDL_Renderer* renderer, TTF_Font* font);
    ~Stats();

    void IncreaseAppleCount();
    void UpdateTimer();
    void ResetTimer();
    void ResetAppleCounter();
    int GetAppleCount();
    bool CheckHighscore();
    void Render(SDL_Renderer* renderer) override;

    static int GetHighscore();
    static bool NewHighScore(int newHighScore);

    static void SaveHighscore();

private:
    void LoadHighscore();


    static int highscore_;
    static int oldHighScore_;
    Text* counterText_;
    TextCounter* appleCounter_;
    Text* timerText_;
    Timer* gameTimer_;
    SDL_Renderer* renderer_;
    TTF_Font* font_;
};
