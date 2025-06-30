export module timer;

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

export class Timer {
public:
    Timer() = default;

    void start(std::function<bool()> isActiveFn) {
        stop();
        elapsedSeconds_ = 0;
        running_ = true;

        future_ = std::async(std::launch::async, [this, isActiveFn] {
            while (running_) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                if (isActiveFn()) ++elapsedSeconds_;
            }
            });
    }

    void stop() {
        running_ = false;
        if (future_.valid()) future_.wait();
    }

    int elapsed() const { return elapsedSeconds_; }

private:
    std::atomic<bool> running_ = false;
    std::future<void> future_;
    std::atomic<int> elapsedSeconds_ = 0;
};