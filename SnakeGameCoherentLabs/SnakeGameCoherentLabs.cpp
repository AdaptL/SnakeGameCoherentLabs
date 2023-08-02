#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <functional>
#include <map>
#include <deque>
#include <unordered_set>
#include <cstdlib> 
#include <string>
#include <ostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <ostream>
#include <filesystem>
#include "SDL_image.h"
#include "Window.h"
#include "MenuScreen.h"

/*TODOS*/
/*
add in-depth documentation
use smart pointers instead of raw
integrate threads for optimization
implement templates with Component and Screen classes for better abstraction
Take advantage of new C++ 17 features
implement move semantics for rendering optimization
define hard-coded dimensions and positions to avoid magic numbers
separate classes into modules and namespaces
fix warnigns
*/

int main(int argc, char* argv[])
{
    std::srand(std::time(nullptr));
    std::cout << "Current path is " << std::filesystem::current_path() << '\n';

    Window window;
    MenuScreen menuScreen;


   window.SetScreen(&menuScreen);
   window.MainLoop();

    return 0;
}
