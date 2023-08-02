#include "Image.h"
#include "Paths.h"
Image::Image(Position position, Dimension dimension, const std::string& imagePath, SDL_Renderer* renderer, float size)
    : Rectangle(position, dimension), imagePath_(imagePath), renderer_(renderer), size_(size) {

    SDL_Surface* loadedSurface = IMG_Load(imagePath_.c_str());

    if (loadedSurface == nullptr) {
        loadedSurface = IMG_Load(IMAGE_PATH_EXE.c_str());

        if (loadedSurface == nullptr) {
            std::cerr << "Failed to load image. SDL_image Error: " << IMG_GetError() << std::endl;
            return;
        }
    }

    texture_ = SDL_CreateTextureFromSurface(renderer_, loadedSurface);
    if (texture_ == nullptr) {
        std::cerr << "Failed to create texture from loaded image. SDL Error: " << SDL_GetError() << std::endl;
    }

    SDL_FreeSurface(loadedSurface);
}

void Image::Render(SDL_Renderer* renderer) {
    dimension_.SetWidth(static_cast<int>(dimension_.GetWidth() * size_));
    dimension_.SetHeight(static_cast<int>(dimension_.GetHeight() * size_));

    SDL_Rect rect{ position_.GetX(), position_.GetY(), dimension_.GetWidth(), dimension_.GetHeight() };
    SDL_RenderCopy(renderer, texture_, nullptr, &rect);
}

Image::~Image() {
    SDL_DestroyTexture(texture_);
    texture_ = nullptr;
}
