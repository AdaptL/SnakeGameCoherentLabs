#include "Stats.h"

int Stats::oldHighScore_ = 0;
int Stats::highscore_ = 0;

Stats::Stats(SDL_Renderer* renderer, TTF_Font* font)
    : renderer_(renderer), font_(font), counterText_(nullptr), appleCounter_(nullptr), timerText_(nullptr), gameTimer_(nullptr) {
    counterText_ = new Text(Position(35, 25), Dimension(250, 50), "Apples eaten :", font);
    appleCounter_ = new TextCounter(Position(295, 35), Dimension(40, 40), font, renderer);

    timerText_ = new Text(Position(400, 25), Dimension(250, 50), "Time passed :", font);
    gameTimer_ = new Timer(Position(660, 27), Dimension(100, 50), font, renderer);

    timerText_->SetColor({ 50, 50, 100, 255 });
    gameTimer_->SetColor({ 50, 50, 100, 255 });

    LoadHighscore();
}

Stats::~Stats() {
    delete counterText_;
    delete appleCounter_;
    delete timerText_;
    delete gameTimer_;
}

void Stats::IncreaseAppleCount() {
    appleCounter_->Increment();
}

void Stats::UpdateTimer() {
    gameTimer_->Update();
}

void Stats::ResetTimer() {
    gameTimer_->Reset();
}

void Stats::ResetAppleCounter() {
    appleCounter_->Reset();
}

int Stats::GetAppleCount() {
    return appleCounter_->GetCounter();
}

bool Stats::CheckHighscore() {
    if (appleCounter_->GetCounter() > highscore_) {
        highscore_ = appleCounter_->GetCounter();
        SaveHighscore(); 
        return true;
    }
    return false;
}

void Stats::Render(SDL_Renderer* renderer) {
    counterText_->Render(renderer);
    appleCounter_->Render(renderer);
    timerText_->Render(renderer);
    gameTimer_->Render(renderer);
}

int Stats::GetHighscore() {
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

bool Stats::NewHighScore(int newHighScore) {
    if (newHighScore > GetHighscore()) {
        highscore_ = newHighScore;
        SaveHighscore();
        return true;
    }
    return false;
}

void Stats::LoadHighscore() {
    std::ifstream file("highscore.txt");

    if (file.is_open()) {
        file >> highscore_;
    }
    else {
        highscore_ = 0;
        SaveHighscore();
    }
}

void Stats::SaveHighscore() {
    std::ofstream file("highscore.txt");
    if (file.is_open()) {
        file << highscore_;
    }
}
