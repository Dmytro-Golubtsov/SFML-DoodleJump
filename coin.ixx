export module coin;

#include "includes.h"

export class Coin {
public:
    Coin(float x, float y, sf::Texture& tex) {
        sprite_.setTexture(tex);
        sprite_.setOrigin(tex.getSize().x / 2.f, tex.getSize().y / 2.f);
        sprite_.setPosition(x, y);
    }

    void move(sf::Vector2f d) { sprite_.move(d); }
    const sf::Sprite& sprite() const { return sprite_; }

    bool collected() const { return collected_; }
    void collect() { collected_ = true; }

private:
    sf::Sprite sprite_;
    bool collected_ = false;
};
