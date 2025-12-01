# Physics-Collision-Simulation

Project ini merupakan Final Project dari Mata Kuliah Struktur Data dan Algoritma A.Project ini merupakan sebuah simulasi colission partikel menggunakan bahasa pemprograman C++

## Table of Content
- [Overview](#Overview)
- [Dasar Teori](#Dasar-Teori)
- [Cara kerja Code]()
- [Dependendcy]()
- [Cara Install]()
- [Progress]()
- [Bug]()

## Overview
Simulasi ini menampilkan partikel-partikel yang bergerak dan bertumbukan dalam sebuah ruang 2D. Program ini membandingkan dua metode deteksi collision:

- QuadTree: Algoritma spatial partitioning yang efisien untuk deteksi collision
- Brute Force: Metode sederhana yang mengecek setiap pasangan partikel

Project ini mengimplementasikan struktur data QuadTree untuk mengoptimalkan deteksi collision pada simulasi fisika partikel 2D menggunakan SFML Graphics Library.

## Dasar-Teori
Program ini menggunakan Teori kekekalan Momentum .Pada program ini sendiri dianggap bahwa setiap partikel yang ada memiliki kelentingan(e) yang sama yaitu 1, hal ini mengartikan bahwa pada program ini sendiri semua dari partikel memiliki sifat lenting sempurna


referensi line code pada perhitungan rumus saat colission dengan bola lain (Line 61-72)

dengan Rumus(dikali 2 pada m2 x v2 agar mempercepat collision)

v'₁ = ((m₁ - m₂) × v₁ + 2 × m₂ × v₂) / (m₁ + m₂)

v'₂ = ((m₂ - m₁) × v₂ + 2 × m₁ × v₁) / (m₁ + m₂)

## Dependendcy
- SFML 3.0.1
- ARIAL.ttf
- C++ 17++
- GCC 1.4.2

## Cara Install
  ### Compile
  g++ -c main.cpp -I<sfml-install-path>/include
  g++ main.o -o physics_sim -L<sfml-install-path>/lib -lsfml-graphics -lsfml-window -lsfml-system

  ### Run
  ./main

## Progress
- [x] Meyiapkan SFML.
- [x] Class Awal Partikel.
- [x] Simulasi Awal Dengan Partikel(1 buah) dan bisa mantul(Hanya transfer kecepatan) dengan batas window.
- [x] Simulasi menggunakan Partikel(100 bola) dan masing masing dapat mantul sesuai dengan teori kekekalan Momentum.
- [x] Penambahan Algoritma Quadtree
- [x] Penambahan Fitur Penambahan Partikel dan UI dasar(Jumlah bola,perhitungan pengecekan pada setiap metode)
- [x] Penyelsaian Algoritma Quadtree
- [x] Fitur ganti metode dan UI status metode

## Known Bug
- Bola dapat stuck di dinding dan jika terpantul/terkena bola lain secara terus menerus kemungkinan dapat hilang
- Quadtree masih belum bisa dikatakan optimal karena secara pencarian bola masih belum konsisten
