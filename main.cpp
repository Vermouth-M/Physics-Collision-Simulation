#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>


class Partikel {
public:
    sf::Vector2f position;
    sf::Vector2f velocity;
    float radius;
    sf::Color color;
    float mass;

    Partikel(float x, float y, float r, sf::Color c) 
        : position(x, y), radius(r), color(c) {
        mass = r * r; 
    }
};

int main() {

}