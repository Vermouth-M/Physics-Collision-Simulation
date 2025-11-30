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

    void updatekondisi(float lebar, float tinggi, float dt) {
        // Update posisi
        position += velocity * dt;
        float kecepatanMaks = 500.0f;
        float kecepatan = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);

        if (kecepatan > kecepatanMaks){
            velocity = velocity * (kecepatanMaks / kecepatan);
        }

        // Pantul border
        if (position.x < radius || position.x > lebar - radius) {
            velocity.x = -velocity.x * 0.99f;
        }
        if (position.y < radius || position.y > tinggi - radius) {
            velocity.y = -velocity.y * 0.99f;
        }
    }
    bool cekcollision(Partikel& lain){
        sf::Vector2f delta = position - lain.position;
        //rumus trigonometri buat tentuin jarak detilnya ada dikertas
        float jarak = sqrt(delta.x*delta.x+delta.y*delta.y);
        return jarak < (radius + lain.radius);
    }
    void colission(Partikel& lain){
        sf::Vector2f delta = position - lain.position;
        float jarak = sqrt(delta.x * delta.x + delta.y * delta.y);
        if (jarak == 0){
            return;
        }
        // mantul harus sesuai dgn rumus tumbukan
        // rumus rumbukan = v' = (Mass*velocity+Mass.lain*velocity.lain)/mass+mass.lain
        sf::Vector2f arah = delta / jarak;
        //percepatan awal
        float arahv1 = velocity.x * arah.x + velocity.y * arah.y;
        float arahv2 = lain.velocity.x * arah.x + lain.velocity.y * arah.y;
        //percepatan after tumbukan(kali 2 biar agak cepat)
        float v1_1 = ((mass - lain.mass) * arahv1 + 2 * lain.mass * arahv2) / (mass + lain.mass);
        float v2_1 = ((lain.mass - mass) * arahv2 + 2 * lain.mass * arahv1) / (mass + lain.mass);
        // perhitungan total (awal-setelah(agar tdk minus))
        float totalperhitungan_1 = v1_1 - arahv1;
        float totalperhitungan_2 = v2_1 - arahv2;

        velocity += arah * totalperhitungan_1;
        lain.velocity += arah * totalperhitungan_2;

        if (jarak > 0) {
            float overlap = (radius + lain.radius - jarak) / 2.0f;
            sf::Vector2f arah = delta / jarak;
            
            position += arah * overlap;
            lain.position -= arah * overlap;
        }
    }
};
//class untuk quadtree(belom lanjut)(ref:https://github.com/msinkec/quadtree-collision-detection/blob/master/quadtree.js)
class Quadtree {
    public:
    float x,y,lebar,tinggi;
    int kap;
    vector<int> partikel2;
    bool terbagi;

    vector<Quadtree> child; 

    Quadtree(float px,float py,float lebarlayar,float tinggilayar,int kapasitas){
        x = px;
        y = py;
        lebar = lebarlayar;
        tinggi = tinggilayar;
        kap = kapasitas;
        terbagi = false;
    }
    //bagi jadi 2 bagian
    void pembagian(){
        float w_2 = lebar / 2;
        float h_2 = tinggi / 2;
        a = new Quadtree(x - w_2,y,w_2,h_2,kap);
        b = new Quadtree(x,y,w_2,h_2,kap);
        c = new Quadtree(x - h_2,y + h_2,w_2,h_2,kap);
        d = new Quadtree(x,y+h_2,w_2,h_2,kap);

        terbagi = true;

    }
    bool berisi(float px, float py) {
        return (px >= x && px < x + lebar && py >= y && py < y + tinggi);
    }
    //cara insertnya gimana anjir
    bool insert(float id, float dx, float dy){
        if(!berisi(px,py)){
            return false;
        }
        if (partikel2.size < kapasitas){
            partikel2.push_back(id);
            return true;
        }
        if (!terbagi){
            pembagian();
        }
        for (int i = 0; i < child.size(); i++) {
            if (child[i].insert(id, dx, dy)) {
                return true;
            }
        }

        return false;

        if(berisi(px,py)){
            return true;
        }

    }

    void draw(sf::RenderWindow& window) {
        sf::RectangleShape kotak({lebar, tinggi});
        kotak.setPosition({x, y});
        kotak.setFillColor(sf::Color::Transparent);
        kotak.setOutlineColor(sf::Color(50, 50, 50));
        kotak.setOutlineThickness(1);
        window.draw(kotak);

        if (terbagi) {
            for (int i = 0; i < children.size(); i++) {
                children[i].draw(window);
            }
        }


};

int main() {
    srand(time(0));
    
    vector<Partikel> listpartikel;
    sf::Clock clock;
    sf::Color color[]{
        sf::Color::Red, sf::Color::Green, sf::Color::Blue,
        sf::Color::Yellow, sf::Color::Magenta, sf::Color::Cyan
    };

    unsigned int LEBAR = 800;
    unsigned int TINGGI = 600;
    
    sf::RenderWindow window(sf::VideoMode({LEBAR, TINGGI}), "FP_Physics_Simulation");
    window.setFramerateLimit(60);

    // Spawn 50 bola SEKALI di awal & brute force lokasi spawn agar tidak overlap
    for(int i = 0; i < 100; i++) {
        bool ceklokasi = false;
        while (!ceklokasi){
            float x = 50 + rand() % (LEBAR - 100);
            float y = 50 + rand() % (TINGGI - 100);
            ceklokasi = true;
            for (int j = 0;j < listpartikel.size();j++){
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
        float waktu = clock.restart().asSeconds();
        while(const auto event = window.pollEvent()) {
            if(event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        // Update semua bola
        for(int i = 0; i < listpartikel.size(); i++) {
            listpartikel[i].updatekondisi(LEBAR, TINGGI, waktu);
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