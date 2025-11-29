#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <ctime>

using namespace std;

class Partikel {
public:
    sf::Vector2f position;
    sf::Vector2f velocity;
    float radius;
    sf::Color color;
    float mass;

    Partikel(float x, float y, float vx, float vy, float r, sf::Color c) 
        : position(x, y), velocity(vx, vy), radius(r), color(c) {
        mass = r * r; 
    }

    void draw(sf::RenderWindow& window) {
        sf::CircleShape lingkaran({radius});
        lingkaran.setPosition({position.x - radius, position.y - radius});
        lingkaran.setFillColor(color);
        window.draw(lingkaran);
    }

    void updatekondisi(float lebar, float tinggi) {
        // Update posisi
        position += velocity * 0.016f;

        // Pantul
        if (position.x < radius || position.x > lebar - radius) {
            velocity.x = -velocity.x;
        }
        if (position.y < radius || position.y > tinggi - radius) {
            velocity.y = -velocity.y;
        }
    }
};

int main() {
    srand(time(0));
    
    vector<Partikel> listpartikel;
    sf::Color color[]{
        sf::Color::Red, sf::Color::Green, sf::Color::Blue,
        sf::Color::Yellow, sf::Color::Magenta, sf::Color::Cyan
    };

    int LEBAR = 800;
    int TINGGI = 600;
    
    sf::RenderWindow window(sf::VideoMode({LEBAR, TINGGI}), "FP_Physics_Simulation");
    window.setFramerateLimit(60);

    // Spawn 50 bola SEKALI di awal
    for(int i = 0; i < 50; i++) {
        float x = 50 + rand() % (LEBAR - 100);
        float y = 50 + rand() % (TINGGI - 100);
        float vx = -100 + rand() % 200;  // -100 sampai 100
        float vy = -100 + rand() % 200;  // -100 sampai 100
        float r = 10;
        sf::Color w = color[rand() % 6];
        
        listpartikel.push_back(Partikel(x, y, vx, vy, r, w));
    }

    while(window.isOpen()) {
        while(const auto event = window.pollEvent()) {
            if(event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        // Update semua bola
        for(int i = 0; i < listpartikel.size(); i++) {
            listpartikel[i].updatekondisi(LEBAR, TINGGI);
        }

        // Gambar semua bola
        window.clear(sf::Color::Black);
        for(int i = 0; i < listpartikel.size(); i++) {
            listpartikel[i].draw(window);
        }
        window.display();
    }

    return 0;
}