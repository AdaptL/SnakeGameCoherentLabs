#include "GameOver.h"

GameOver::GameOver(TTF_Font* font, SDL_Renderer* renderer, bool newHighScore)
    : gameOver_(false), highScore_(0),
    gameOverText_(Position(110, 200), Dimension(600, 75), "Game Over", font),
    highScoreText_(Position(260, 290), Dimension(300, 40), "", font),
    restartButton_(Position(300, 350), Dimension(200, 50), nullptr, font, "Restart"),
    background_(Position(2, 102), Dimension(796, 496), { 0, 0, 0, 255 }),
    font_(font), renderer_(renderer) { newHighScore_ = newHighScore; }

void GameOver::Render(SDL_Renderer* renderer) {
    if (gameOver_) {
        background_.Render(renderer);
        gameOverText_.Render(renderer);
        if (newHighScore_) {
            highScoreText_.SetText("New high score: " + std::to_string(Stats::GetHighscore()), renderer);
            highScoreText_.Render(renderer);
        
        }
        else {
            highScoreText_.SetText("Score: " + std::to_string(GetHighScore()), renderer);
            highScoreText_.Render(renderer);
        }
        restartButton_.Render(renderer);
    }
}

void GameOver::CheckClick(int mouseX, int mouseY) {
    if (gameOver_) {
        restartButton_.CheckClick(mouseX, mouseY);
    }
}

void GameOver::StartGameOver() {
    gameOver_ = true;
}

void GameOver::StopGameOver() {
    gameOver_ = false;
}

void GameOver::SetHighScore(int highScore) {
    highScore_ = highScore;
}

int GameOver::GetHighScore() {
    return highScore_;
}
