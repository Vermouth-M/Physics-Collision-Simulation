#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>


class Partikel {
public:
    sf::Vector2f position;
    float speed;
    float radius;
    sf::Color color;
    float mass;

    Partikel(float x, float y, float r, sf::Color c, float vel) 
        : position(x, y), radius(r), color(c), speed(vel) {
        mass = r * r; 
    }
    void draw(sf::RenderWindow& window){
        sf::CircleShape lingkaran({radius});
        lingkaran.setPosition({position.x-radius, position.y-radius});

    }

};

int main() {

}