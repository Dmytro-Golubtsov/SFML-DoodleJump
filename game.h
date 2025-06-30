#pragma once

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

import coin;
import drawer;
import platform;
import player;
import timer;


class Game {
public:
    Game()
        : mode_{ 500, 700 },
        window_(sf::VideoMode(mode_.x, mode_.y), "DoodleJump"),
        rng_(std::random_device{}()),
        distX_(0.f, 432.f),
        distCoin_(0.f, 1.f),
        menu_(mode_),
        over_(menu_.font(), mode_),
        pauseUI_(menu_.font(), mode_) {
        window_.setFramerateLimit(60);
        locateAssets();
        loadAssets();
        scores_ = std::make_unique<ScoreStorage>(projectRoot_ / "scores" / "scores.txt");
        reset();
        push(State::Menu);
        drawer_ = std::make_unique<Drawer>(window_);
    }

    void run() {
        bgm_.start("sound/background-music.wav");
        const float dt = 1.f / 60.f;
        while (window_.isOpen()) {
            handleEvents();
            if (current() == State::Playing) update(dt);
            render();
        }
        bgm_.stop();
        timer_.stop();
    }

private:
    std::unique_ptr<Drawer> drawer_;

    enum class State { Menu, Playing, Achievements, GameOver, Pause };
    std::vector<State> states_;

    Timer timer_;

    BackgroundMusicPlayer bgm_;


    void push(State s) { states_.push_back(s); }
    void pop() { if (states_.size() > 1) states_.pop_back(); }
    State current() const { return states_.back(); }

    sf::Vector2u     mode_;
    sf::RenderWindow window_;


    fs::path projectRoot_;            


    sf::Texture   bgTex_, platTex_, texL_, texR_, coinTex_;
    sf::Sprite    bg_;
    sf::SoundBuffer jumpBuf_;
    sf::Sound     jump_;


    std::unique_ptr<Player>                player_;
    std::vector<std::unique_ptr<Platform>> plats_;
    std::vector<std::unique_ptr<Coin>>     coins_;


    sf::Vector2f  vel_{ 0.f, 0.f };
    float         hspeed_ = 0.f;


    std::mt19937                          rng_;
    std::uniform_real_distribution<float> distX_;
    std::uniform_real_distribution<float> distCoin_; 

    Menu       menu_;
    GameOverUI over_;
    PauseUI    pauseUI_;
    std::unique_ptr<ScoreStorage> scores_;

    unsigned score_ = 0;
    float    maxHeightReached_ = 0.f;

    void locateAssets() {

        fs::path p = fs::current_path();
        for (int i = 0; i < 5 && !fs::exists(p / "images"); ++i) p = p.parent_path();
        if (!fs::exists(p / "images")) throw std::runtime_error("Assets folders not found. Expected 'images' inside project");
        projectRoot_ = p;
    }




    void resetTimer() {
        timer_.start([this] { return current() == State::Playing; });
    }


    void loadAssets() {
        auto img = projectRoot_ / "images";
        auto snd = projectRoot_ / "sound";

        if (!bgTex_.loadFromFile((img / "background.png").string()) ||
            !platTex_.loadFromFile((img / "platform.png").string()) ||
            !texL_.loadFromFile((img / "doodlel.png").string()) ||
            !texR_.loadFromFile((img / "doodler.png").string()) ||
            !coinTex_.loadFromFile((img / "coin.png").string()) ||
            !jumpBuf_.loadFromFile((snd / "jump.wav").string()))
            throw std::runtime_error("Failed to load one or more assets");

        bg_.setTexture(bgTex_);
        jump_.setBuffer(jumpBuf_);
    }

    void reset() {
        resetTimer();
        score_ = 0;
        maxHeightReached_ = 0.f;
        plats_.clear(); coins_.clear();
        
        player_ = std::make_unique<Player>(250.f, 600.f, texL_, texR_);
        vel_ = { 0.f, 0.f }; hspeed_ = 0.f;

        for (int i = 0; i < 10; ++i) {
            float x = distX_(rng_) + platTex_.getSize().x / 2.f;
            float y = static_cast<float>(mode_.y - i * 70);
            plats_.push_back(std::make_unique<Platform>(x, y, platTex_));

        }
        const float px = player_->position().x;
        const float py = player_->position().y;
        plats_.push_back(std::make_unique<Platform>(px, py + 30.f, platTex_));
    }


    void handleEvents() {
        sf::Event ev{};
        while (window_.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) { window_.close(); return; }

            if (current() == State::Menu && ev.type == sf::Event::MouseButtonReleased) {
                auto c = menu_.click({ static_cast<float>(ev.mouseButton.x), static_cast<float>(ev.mouseButton.y) });
                if (c == Menu::Choice::Play) {
                    reset();
                    push(State::Playing);
                    resetTimer();
                }


                else if (c == Menu::Choice::Achievements) { push(State::Achievements); }
                else if (c == Menu::Choice::Quit) { window_.close(); }
            }
            else if (current() == State::GameOver && ev.type == sf::Event::MouseButtonReleased) {
                auto c = over_.click({ static_cast<float>(ev.mouseButton.x), static_cast<float>(ev.mouseButton.y) });
                if (c == GameOverUI::Choice::Restart) { reset(); states_.clear(); push(State::Playing); }
                else if (c == GameOverUI::Choice::Quit) { window_.close(); }
            }
            else if (current() == State::Playing && ev.type == sf::Event::KeyPressed && ev.key.code == sf::Keyboard::P) {
                push(State::Pause);
            }
            else if (current() == State::Pause && ev.type == sf::Event::KeyPressed && ev.key.code == sf::Keyboard::P) {
                pop();
            }
            else if (current() == State::Achievements && ev.type == sf::Event::KeyPressed && ev.key.code == sf::Keyboard::Escape) {
                pop();
            }
        }
    }

    void update(float dt) {

        bool left = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
        bool right = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
        if (left)  hspeed_ -= 2000.f * dt;
        if (right) hspeed_ += 2000.f * dt;
        if (!left && !right) {
            if (hspeed_ > 0) hspeed_ = std::max(0.f, hspeed_ - 1000.f * dt);
            else             hspeed_ = std::min(0.f, hspeed_ + 1000.f * dt);
        }
        hspeed_ = std::clamp(hspeed_, -300.f, 300.f);
        vel_.x = hspeed_;
        if (left && !right) player_->faceLeft();
        if (right && !left) player_->faceRight();

        vel_.y += 1800.f * dt;
        player_->move(vel_ * dt);

        float w = static_cast<float>(mode_.x);
        auto p = player_->position();
        if (p.x < 0) player_->setPosition({ w, p.y });
        else if (p.x > w) player_->setPosition({ 0.f, p.y });

        if (vel_.y > 0) {
            for (auto& pl : plats_) {
                if (pl->sprite().getGlobalBounds().contains(p.x, p.y + 35)) {
                    vel_.y = -900.f;
                    jump_.play();
                    break;
                }
            }
        }

        float midY = mode_.y / 2.f;
        if (p.y < midY) {
            float dy = midY - p.y;
            player_->move({ 0, dy });
            maxHeightReached_ += dy;       
            score_ = static_cast<unsigned>(maxHeightReached_ / 10);
            for (auto& pl : plats_) {
                pl->move({ 0, dy });
                if (pl->sprite().getPosition().y > mode_.y) {
                    pl->setPosition({ distX_(rng_) + platTex_.getSize().x / 2.f, 0.f });
      

                    if (distCoin_(rng_) < 0.3f) {
                        coins_.push_back(std::make_unique<Coin>(pl->sprite().getPosition().x,
                            pl->sprite().getPosition().y - 40.f,
                            coinTex_));
                    }
                }
            }
            for (auto& c : coins_) c->move({ 0, dy });
        }

        
        for (auto& c : coins_) {
            if (!c->collected() && c->sprite().getGlobalBounds().contains(p)) {
                c->collect();
                score_ += 50; 
            }
        }

 
        coins_.erase(std::remove_if(coins_.begin(), coins_.end(), [&](const auto& c) {
            return c->collected() || c->sprite().getPosition().y > mode_.y;
            }), coins_.end());


        if (player_->position().y > static_cast<float>(mode_.y)) {
            std::string nickname;
            std::regex pattern("^[A-Za-z]{4,15}$");
            do {
                std::cout << "Enter your nickname (4-15 letters, A-Z or a-z): ";
                std::cin >> nickname;
            } while (!std::regex_match(nickname, pattern));

            scores_->save(nickname, score_);
            push(State::GameOver);
        }
    }


    void drawTimer() {
        static sf::Text timerText;
        static bool initialized = false;
        if (!initialized) {
            timerText.setFont(menu_.font());
            timerText.setCharacterSize(24);
            timerText.setFillColor(sf::Color::Black);
            initialized = true;
        }

        int elapsed = timer_.elapsed();
        int mins = elapsed / 60;
        int secs = elapsed % 60;
        timerText.setString("Time: " + std::to_string(mins) + ":" + (secs < 10 ? "0" : "") + std::to_string(secs));
        auto bounds = timerText.getLocalBounds();
        timerText.setPosition(static_cast<float>(mode_.x) - bounds.width - 10.f, 10.f);
        drawer_->draw(timerText);
    }

    void render() {
        window_.clear();
        window_.draw(bg_);

        switch (current()) {
        case State::Menu:
            menu_.draw(window_);
            break;
        case State::Achievements:
            drawAchievements();
            break;
        case State::Playing:
        case State::Pause:
            drawGame();
            if (current() == State::Pause) pauseUI_.draw(window_);
            break;
        case State::GameOver:
            drawGame();
            over_.draw(window_);
            break;
        }
        if (current() == State::Playing || current() == State::Pause) drawTimer();
        window_.display();
    }

    void drawGame() {

        for (auto& pl : plats_)  drawer_->draw(pl->sprite());
        for (auto& c : coins_) {
            //c->sprite().setScale(0.5f, 0.5f);
            drawer_->draw(c->sprite());
        } 
        drawer_->draw(player_->sprite());
        drawScore();
    }

    void drawScore() {
        static sf::Text txt; static bool init = false;
        if (!init) { txt.setFont(menu_.font()); txt.setCharacterSize(24); txt.setFillColor(sf::Color::Black); init = true; }
        txt.setString("Score: " + std::to_string(score_));
        txt.setPosition(10.f, 10.f);
        drawer_->draw(txt);
    }

    void drawAchievements() {
        auto top = scores_->loadTop();
        sf::Text header("Top Scores", menu_.font(), 40);
        header.setFillColor(sf::Color::Black);
        auto hb = header.getLocalBounds();
        header.setOrigin(hb.width / 2, hb.height / 2);
        header.setPosition(mode_.x / 2.f, 80.f);
        drawer_->draw(header);

        float y = 150.f; int rank = 1;
        for (auto& [name, score] : top) {
            sf::Text t(std::to_string(rank++) + ". " + name + " - " + std::to_string(score), menu_.font(), 30);
            t.setFillColor(sf::Color::Black);
            auto b = t.getLocalBounds();
            t.setOrigin(b.width / 2, b.height / 2);
            t.setPosition(mode_.x / 2.f, y);
            y += 40.f;
            drawer_->draw(t);
        }

        sf::Text hint("Press Esc to return", menu_.font(), 24);
        hint.setFillColor(sf::Color::Black);
        auto hb2 = hint.getLocalBounds();
        hint.setOrigin(hb2.width / 2, hb2.height / 2);
        hint.setPosition(mode_.x / 2.f, mode_.y - 50.f);
        drawer_->draw(hint);
    }
};