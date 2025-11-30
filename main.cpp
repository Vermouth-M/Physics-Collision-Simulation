#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>

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
    bool cekcollision(Partikel& lain){
        sf::Vector2f delta = position - lain.position;
        //rumus trigonometri buat tentuin jarak detilnya ada dikertas
        float jarak = sqrt(delta.x*delta.x+delta.y*delta.y);
        return jarak < (radius + lain.radius);
    }
    void colission(Partikel& lain){
        sf::Vector2f temp = velocity;
        velocity = lain.velocity;
        lain.velocity = temp;

        sf::Vector2f delta = position - lain.position;
        float jarak = sqrt(delta.x * delta.x + delta.y * delta.y);
        if (jarak > 0) {
            float overlap = (radius + lain.radius - jarak) / 2.0f;
            sf::Vector2f arah = delta / jarak;
            
            position += arah * overlap;
            lain.position -= arah * overlap;
        }
    }
};
//class untuk quadtree
class Quadtree {

}

int main() {
    srand(time(0));
    
    vector<Partikel> listpartikel;
    sf::Color color[]{
        sf::Color::Red, sf::Color::Green, sf::Color::Blue,
        sf::Color::Yellow, sf::Color::Magenta, sf::Color::Cyan
    };

    unsigned int LEBAR = 800;
    unsigned int TINGGI = 600;
    
    sf::RenderWindow window(sf::VideoMode({LEBAR, TINGGI}), "FP_Physics_Simulation");
    window.setFramerateLimit(60);

    // Spawn 50 bola SEKALI di awal & brute force lokasi spawn agar tidak overlap
    for(int i = 0; i < 50; i++) {
        bool ceklokasi = false;
        while (!ceklokasi){
            float x = 50 + rand() % (LEBAR - 100);
            float y = 50 + rand() % (TINGGI - 100);
            ceklokasi = true;
            for (int j = 0;listpartikel.size()<j;j++){
                float tempx = x-listpartikel[j].position.x;
                float tempy = y-listpartikel[j].position.y;
                float jarak = sqrt(tempx*tempx+tempy*tempy);
                if (jarak < 30){
                    ceklokasi = false;
                    break;
                }

            }

        }
        float x = 50 + rand() % (LEBAR - 100);
        float y = 50 + rand() % (TINGGI - 100);
        float vx = -100 + rand() % 200;   
        float vy = -100 + rand() % 200;  
        float r = 10 + rand() % 20;
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
        //Cek tumbukan(brute force utk saat ini)
        for(int i = 0; i < listpartikel.size(); i++) {
            for(int j = i + 1; j < listpartikel.size(); j++) {
                if(listpartikel[i].cekcollision(listpartikel[j])) {
                    listpartikel[i].colission(listpartikel[j]);
                }
            }
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