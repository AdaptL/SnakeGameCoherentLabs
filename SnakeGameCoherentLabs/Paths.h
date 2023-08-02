#pragma once
#include <string>
#include <filesystem>

static const std::string  FONT_PATH = "..\\resources\\Fonts\\super-legend-boy-font\\SuperLegendBoy-4w8Y.ttf";
static const std::string  IMAGE_PATH = "..\\resources\\images\\snake_image.png";



static const std::string FONT_PATH_EXE = (std::filesystem::current_path() / "resources" / "Fonts" / "super-legend-boy-font" / "SuperLegendBoy-4w8Y.ttf").string();
static const std::string IMAGE_PATH_EXE = (std::filesystem::current_path() / "resources" / "Images" / "snake_image.png").string();
