#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include "Component.h"
#include "Text.h"
#include "Button.h"
#include "Rectangle.h"
#include "Stats.h"

class GameOver : public Component {
public:
    GameOver(TTF_Font* font, SDL_Renderer* renderer, bool newHighScore = false);

    void Render(SDL_Renderer* renderer) override;
    void CheckClick(int mouseX, int mouseY);
    void StartGameOver();
    void StopGameOver();
    void SetHighScore(int highScore);
    int GetHighScore();

private:
    bool newHighScore_;
    bool gameOver_;
    int  highScore_;
    Text gameOverText_;
    Text highScoreText_;
    Button restartButton_;
    Rectangle background_;
    TTF_Font* font_;
    SDL_Renderer* renderer_;
};
