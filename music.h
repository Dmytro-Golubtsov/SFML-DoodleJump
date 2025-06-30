#pragma once


#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <random>
#include <vector>
#include <memory>
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

class BackgroundMusicPlayer {
public:
    BackgroundMusicPlayer() = default;

    void start(const std::string& path) {
        stop();
        if (!buffer_.loadFromFile(path)) return;
        sound_.setBuffer(buffer_);
        sound_.setLoop(true);
        running_ = true;
        thread_ = std::thread([this] {
            sound_.play();
            while (running_ && sound_.getStatus() == sf::Sound::Playing) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            });
    }

    void stop() {
        running_ = false;
        if (sound_.getStatus() == sf::Sound::Playing)
            sound_.stop();
        if (thread_.joinable())
            thread_.join();
    }

    ~BackgroundMusicPlayer() {
        stop();
    }

private:
    std::thread thread_;
    std::atomic<bool> running_{ false };
    sf::SoundBuffer buffer_;
    sf::Sound sound_;
};