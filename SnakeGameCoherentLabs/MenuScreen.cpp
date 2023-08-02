#include "MenuScreen.h"
#include "Paths.h"
void MenuScreen::Enter() {
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return;
    }

    TTF_Font* font = TTF_OpenFont(FONT_PATH.c_str(), 28);
    if (!font) {
        font = TTF_OpenFont(FONT_PATH_EXE.c_str(), 28);
        if (!font) {
            std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
            return;
        }
    }

    menuUI = new MenuUI(font, window_->GetRenderer(), IMAGE_PATH);
    AddComponent(menuUI);
}

void MenuScreen::Leave() {}

void MenuScreen::Update() {}

void MenuScreen::HandleEvents(const SDL_Event& evt) {
    if (evt.type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        if (menuUI->CheckClick(mouseX, mouseY)) {
            window_->SetScreen(new GameScreen());
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

void MenuScreen::PrintScreenName() {
    std::cout << "MenuScreen" << std::endl;
}

MenuScreen::~MenuScreen() {}
