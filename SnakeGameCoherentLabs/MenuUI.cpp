#include "MenuUI.h"

MenuUI::MenuUI(TTF_Font* font, SDL_Renderer* renderer, const std::string& imagePath)
    : title_(Position(90, 100), Dimension(600, 150), "Snake", font),
    createdByText_(Position(220, 250), Dimension(400, 40), "Created by Viktor Chochkov for Coherent Labs", font),
    startButton_(Position(290, 350), Dimension(250, 75), nullptr, font, "Start"),
    snakeImage_(Position(690, 133), Dimension(90, 90), imagePath, renderer),
    renderer_(renderer) {
    // Set green color for texts
    title_.SetColor({ 0, 255, 0, 255 });
    createdByText_.SetColor({ 50, 50, 100, 255 });
}

void MenuUI::Render(SDL_Renderer* renderer) {
    title_.Render(renderer);
    createdByText_.Render(renderer);
    startButton_.Render(renderer);
    snakeImage_.Render(renderer);
}

bool MenuUI::CheckClick(int mouseX, int mouseY) {
    return startButton_.CheckClick(mouseX, mouseY);
}
