#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include "Screen.h"
#include "MenuUI.h"
#include "GameScreen.h"

class MenuScreen : public Screen {
public:
    void Enter() override;
    void Leave() override;
    void Update() override;
    void HandleEvents(const SDL_Event& evt) override;
    void PrintScreenName() override;

private:
    MenuUI* menuUI;
};
