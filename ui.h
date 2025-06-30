#pragma once

#include "includes.h"

class Menu {
public:
    enum class Choice { None, Play, Achievements, Quit };

    explicit Menu(sf::Vector2u wnd) {
        if (!font_.loadFromFile("font/Arial.ttf"))
            throw std::runtime_error("Cannot load system font");

        makeText(title_, "Doodle Jump", 48, wnd, 100);
        makeText(play_, "Play", 32, wnd, 300);
        makeText(ach_, "Achievements", 32, wnd, 370);
        makeText(quit_, "Quit", 32, wnd, 440);
    }

    void draw(sf::RenderWindow& w) const {
        w.draw(title_);
        w.draw(play_);
        w.draw(ach_);
        w.draw(quit_);
    }

    Choice click(sf::Vector2f p) const {
        if (play_.getGlobalBounds().contains(p)) return Choice::Play;
        if (ach_.getGlobalBounds().contains(p)) return Choice::Achievements;
        if (quit_.getGlobalBounds().contains(p)) return Choice::Quit;
        return Choice::None;
    }

    const sf::Font& font() const { return font_; }

private:
    void makeText(sf::Text& t, const std::string& s, unsigned sz, sf::Vector2u wnd, float y) {
        t.setFont(font_);
        t.setString(s);
        t.setCharacterSize(sz);
        t.setFillColor(sf::Color::Black);
        auto b = t.getLocalBounds();
        t.setOrigin(b.width / 2.f, b.height / 2.f);
        t.setPosition(wnd.x / 2.f, y);
    }

    sf::Font font_;
    sf::Text title_, play_, ach_, quit_;
};


class GameOverUI {
public:
    enum class Choice { None, Restart, Quit };

    GameOverUI(const sf::Font& f, sf::Vector2u wnd) {
        build(lbl_, "Game Over", 48, f, wnd, 200);
        build(btnR_, "Restart", 32, f, wnd, 350);
        build(btnQ_, "Quit", 32, f, wnd, 420);
    }

    void draw(sf::RenderWindow& w) const {
        w.draw(lbl_);
        w.draw(btnR_);
        w.draw(btnQ_);
    }

    Choice click(sf::Vector2f p) const {
        if (btnR_.getGlobalBounds().contains(p)) return Choice::Restart;
        if (btnQ_.getGlobalBounds().contains(p)) return Choice::Quit;
        return Choice::None;
    }

private:
    static void build(sf::Text& t, const std::string& s, unsigned sz, const sf::Font& f, sf::Vector2u wnd, float y) {
        t.setFont(f);
        t.setString(s);
        t.setCharacterSize(sz);
        t.setFillColor(sf::Color::Black);
        auto b = t.getLocalBounds();
        t.setOrigin(b.width / 2.f, b.height / 2.f);
        t.setPosition(wnd.x / 2.f, y);
    }

    sf::Text lbl_, btnR_, btnQ_;
};


class PauseUI {
public:
    PauseUI(const sf::Font& f, sf::Vector2u wnd) {
        txt_.setFont(f);
        txt_.setString("PAUSED\nPress P to resume");
        txt_.setCharacterSize(40);
        txt_.setFillColor(sf::Color::Black);
        auto b = txt_.getLocalBounds();
        txt_.setOrigin(b.width / 2.f, b.height / 2.f);
        txt_.setPosition(wnd.x / 2.f, wnd.y / 2.f);
    }

    void draw(sf::RenderWindow& w) const { w.draw(txt_); }

private:
    sf::Text txt_;
};
