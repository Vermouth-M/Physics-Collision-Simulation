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
    int id; // Untuk identifikasi unik

    Partikel(float x, float y, float vx, float vy, float r, sf::Color c, int particleId = 0) 
        : position(x, y), velocity(vx, vy), radius(r), color(c), id(particleId) {
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
        // mungkin bisa diedit suapaya dia ga stuck ke border?????
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
//class untuk quadtree(sudah diubah tanpa pointer, menggunakan vector dan index)
class Quadtree {
public:
    float x, y, lebar, tinggi;
    int kapasitas;
    vector<int> partikelIndices; // Menyimpan index partikel
    bool terbagi;
    
    vector<Quadtree> children; // Menyimpan child sebagai vector

    Quadtree(float px, float py, float w, float h, int kap = 4) {
        x = px;
        y = py;
        lebar = w;
        tinggi = h;
        kapasitas = kap;
        terbagi = false;
    }

    // Bagi menjadi 4 kuadran
    void subdivide() {
        float w2 = lebar / 2;
        float h2 = tinggi / 2;
        
        children.clear();
        children.reserve(4);
        children.push_back(Quadtree(x + w2, y, w2, h2, kapasitas));      // kanan atas
        children.push_back(Quadtree(x, y, w2, h2, kapasitas));           // kiri atas
        children.push_back(Quadtree(x + w2, y + h2, w2, h2, kapasitas)); // kanan bawah
        children.push_back(Quadtree(x, y + h2, w2, h2, kapasitas));      // kiri bawah
        
        terbagi = true;
    }

    // Cek partikel dalam area ini
    bool berisi(const Partikel& p) {
        return (p.position.x >= x && p.position.x < x + lebar && 
                p.position.y >= y && p.position.y < y + tinggi);
    }

    // Insert partikel ke quadtree
    bool insert(int partikelIndex, const vector<Partikel>& allPartikel) {
        if (!berisi(allPartikel[partikelIndex])) {
            return false;
        }
        
        if (partikelIndices.size() < kapasitas) {
            partikelIndices.push_back(partikelIndex);
            return true;
        }

        if (!terbagi) {
            subdivide();
        }
        
        for (int i = 0; i < 4; i++) {
            if (children[i].insert(partikelIndex, allPartikel)) {
                return true;
            }
        }

        return false;
    }

    // Cari partikel
    void query(float px, float py, float radius, vector<int>& hasil, const vector<Partikel>& allPartikel) {
        // Cek overlap dengan quadtree
        if (px + radius < x || //Posisi Kiri
            px - radius > x + lebar || //Posisi Kanan
            py + radius < y || //Posisi atas
            py - radius > y + tinggi) { //Posisi bawah
            return;
        }

        // Tambahkan partikel di node ini
        for (int idx : partikelIndices) {
            hasil.push_back(idx); //
        }

        if (terbagi) {
            for (int i = 0; i < 4; i++) {
                children[i].query(px, py, radius, hasil, allPartikel);
            }
        }
    }

    // Gambar grid
    void draw(sf::RenderWindow& window) {
        sf::RectangleShape kotak({lebar, tinggi});
        kotak.setPosition({x, y});
        kotak.setFillColor(sf::Color::Transparent);
        kotak.setOutlineColor(sf::Color(50, 50, 50));
        kotak.setOutlineThickness(1);
        window.draw(kotak);

        if (terbagi) {
            for (int i = 0; i < 4; i++) {
                children[i].draw(window);
            }
        }
    }
};

int main() {
    srand(time(0));
    
    vector<Partikel> listpartikel;
    sf::Clock clock;
    sf::Font font;
    font.openFromFile("Arial.ttf");
    sf::Text text(font);
    sf::Color color[]{
        sf::Color::Red, sf::Color::Green, sf::Color::Blue,
        sf::Color::Yellow, sf::Color::Magenta, sf::Color::Cyan
    };

    unsigned int LEBAR = 800;
    unsigned int TINGGI = 600;
    
    sf::RenderWindow window(sf::VideoMode({LEBAR, TINGGI}), "FP_Physics_Simulation");
    window.setFramerateLimit(60);



    // Spawn 100 bola SEKALI di awal & brute force lokasi spawn agar tidak overlap
    for(int i = 0; i < 100; i++) {
        bool posisiValid = false;
        float x, y;
        
        while (!posisiValid){
            x = 50 + rand() % (LEBAR - 100);
            y = 50 + rand() % (TINGGI - 100);
            posisiValid = true;
            
            for (int j = 0; j < listpartikel.size(); j++){
                float dx = x - listpartikel[j].position.x;
                float dy = y - listpartikel[j].position.y;
                float jarak = sqrt(dx*dx + dy*dy);
                float jarakMin = 10 + listpartikel[j].radius + 5;
                if (jarak < jarakMin){
                    posisiValid = false;
                    break;
                }
            }
        }
        
        float vx = -100 + rand() % 200;   
        float vy = -100 + rand() % 200;  
        float r = 8;
        sf::Color w = color[rand() % 6];
        
        listpartikel.push_back(Partikel(x, y, vx, vy, r, w, i));
    }
    
    bool useQuadTree = true;
    int jumlahPengecekan = 0;
    text.setCharacterSize(18);
    text.setFillColor(sf::Color::White);
    text.setPosition({10, 10});

    while(window.isOpen()) {
        float waktu = clock.restart().asSeconds();
        while(const optional<sf::Event> event = window.pollEvent()) {
            if(event->is<sf::Event::Closed>()) {
                window.close();
            }
           
            if(const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if(mousePressed->button == sf::Mouse::Button::Left) {
                    float x = static_cast<float>(mousePressed->position.x);
                    float y = static_cast<float>(mousePressed->position.y);
                    float vx = -100 + rand() % 200;  
                    float vy = -100 + rand() % 200;
                    float r = 10 + rand() % 20;
                    sf::Color w = color[rand() % 6];
                    listpartikel.push_back(Partikel(x, y, vx, vy, r, w, listpartikel.size()));
                }
            }
            
            if(const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if(keyPressed->code == sf::Keyboard::Key::Space) {
                    useQuadTree = !useQuadTree;
                }
            }
        }
        
        // Update semua bola
        for(int i = 0; i < listpartikel.size(); i++) {
            listpartikel[i].updatekondisi(LEBAR, TINGGI, waktu);
        }
        
        jumlahPengecekan = 0;

        if (useQuadTree) {
            //QUADTREE
            Quadtree qt(0, 0, LEBAR, TINGGI, 4);
            
            // Insert semua partikel ke QuadTree
            for(int i = 0; i < listpartikel.size(); i++) {
                qt.insert(i, listpartikel);
            }

            // Cek collision dengan QuadTree
            for(int i = 0; i < listpartikel.size(); i++) {
                vector<int> kandidat;
                //area pencarian
                float searchRadius = listpartikel[i].radius * 3;
                qt.query(listpartikel[i].position.x, listpartikel[i].position.y, 
                         searchRadius, kandidat, listpartikel);
                
                for (int idx : kandidat) {
                    if (idx != i) {
                        jumlahPengecekan++;
                        if (listpartikel[i].cekcollision(listpartikel[idx])) {
                            listpartikel[i].colission(listpartikel[idx]);
                        }
                    }
                }
            }

            // Gambar
            window.clear(sf::Color::Black);
            qt.draw(window);
            
        } else {
            //BRUTE FORCE 
            for(int i = 0; i < listpartikel.size(); i++) {
                for(int j = i + 1; j < listpartikel.size(); j++) {
                    jumlahPengecekan++;
                    if(listpartikel[i].cekcollision(listpartikel[j])) {
                        listpartikel[i].colission(listpartikel[j]);
                    }
                }
            }

            window.clear(sf::Color::Black);
        }

        // Gambar semua partikel
        for(int i = 0; i < listpartikel.size(); i++) {
            listpartikel[i].draw(window);
        }

        // Tampilkan info
        string metode = useQuadTree ? "QuadTree" : "Brute Force";
        string teks = "Metode: " + metode + "\n";
        teks += "Partikel: " + to_string(listpartikel.size()) + "\n";
        teks += "Pengecekan: " + to_string(jumlahPengecekan) + "\n";
        teks += "\nSPACE: Ganti metode";
        
        text.setString(teks);
        window.draw(text);

        window.display();
    }

    return 0;
}