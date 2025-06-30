#include <algorithm>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <future>         
#include <atomic>         
#include <thread>         
#include <chrono>        
#include <regex>
#include <functional>     
#include <ranges>
#include <string>
#include "music.h"
#include "score_storage.h"
#include "ui.h"
#include  "game.h"
import coin;
import drawer;
import platform;
import player;
import timer;


int main() {
    try {
        Game{}.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
