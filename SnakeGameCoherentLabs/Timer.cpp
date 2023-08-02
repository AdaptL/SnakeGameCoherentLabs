#include "Timer.h"
#include <sstream>
#include <iomanip>
#include "SDL.h"

Timer::Timer(Position pos , Dimension dim, TTF_Font* font, SDL_Renderer* renderer)
    : Text(pos, dim, "00:00", font), renderer_(renderer), lastUpdateTime_(SDL_GetTicks()), seconds_(0), minutes_(0) {}

void Timer::Update() {
    Uint32 current_time = SDL_GetTicks();
    if (current_time > lastUpdateTime_ + 1000) {
        ++seconds_;
        if (seconds_ >= 60) {
            ++minutes_;
            seconds_ = 0;
        }
        SetText(GetFormattedTime(), renderer_);
        lastUpdateTime_ = current_time;
    }
}

void Timer::Reset() {
    seconds_ = 0;
    minutes_ = 0;
    Render(renderer_);
}

std::string Timer::GetFormattedTime() const {
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << minutes_ << ":" << std::setw(2) << std::setfill('0') << seconds_;
    return oss.str();
}
