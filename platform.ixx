export module platform;
#include <SFML/Graphics.hpp>
export class Platform {
public:
    Platform(float x, float y, sf::Texture& tex) {
        sprite_.setTexture(tex);
        sprite_.setOrigin(tex.getSize().x / 2.f, tex.getSize().y / 2.f);
        sprite_.setPosition(x, y);
    }

    void                    move(sf::Vector2f d) { sprite_.move(d); }
    const sf::Sprite& sprite()  const { return sprite_; }
    void                    setPosition(sf::Vector2f p) { sprite_.setPosition(p); }

private:
    sf::Sprite sprite_;
};