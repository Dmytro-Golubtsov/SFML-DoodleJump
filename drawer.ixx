export module drawer;

import <SFML/Graphics.hpp>;
import <concepts>;

export template<typename T>
concept IsDrawable = std::derived_from<T, sf::Drawable>;

export class Drawer {
public:
    explicit Drawer(sf::RenderWindow& win) : window_(win) {}

    template<IsDrawable T>
    void draw(const T& object) {
        window_.draw(object);
    }

private:
    sf::RenderWindow& window_;
};