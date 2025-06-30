export module player;

#include "includes.h"

export class Player {
public:
    Player(float x, float y, sf::Texture& texL, sf::Texture& texR)
        : left_(&texL), right_(&texR) {
        setTexture(texR);              // start facing right
        sprite_.setPosition(x, y);
    }

    void setTexture(sf::Texture& tex) {
        if (sprite_.getTexture() == &tex) return;
        sprite_.setTexture(tex, true);
        sprite_.setOrigin(tex.getSize().x / 2.f, tex.getSize().y / 2.f);
    }

    void faceLeft() { setTexture(*left_); }
    void faceRight() { setTexture(*right_); }

    void                    move(sf::Vector2f d) { sprite_.move(d); }
    void                    setPosition(sf::Vector2f p) { sprite_.setPosition(p); }
    sf::Vector2f            position() const { return sprite_.getPosition(); }
    const sf::Sprite& sprite()   const { return sprite_; }

private:
    sf::Sprite   sprite_;
    sf::Texture* left_{};
    sf::Texture* right_{};
};