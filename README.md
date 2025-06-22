# C++ HTTP Server (Winsock2 + Thread)
[TR]
Bu proje, Windows işletim sistemi üzerinde `Winsock2` ve çoklu iş parçacığı (thread) kullanılarak yazılmış basit bir HTTP sunucusudur. Gelen HTTP `GET` isteklerini işler ve belirtilen klasörden statik dosyaları sunar.

[ENG]
This project is a simple HTTP server written using `Winsock2` and multithreading (thread pool) on Windows. It handles incoming HTTP `GET` requests and serves static files from a specified directory.

---

## Özellikler / Features
[TR]
- Düşük seviyeli C++ socket programlama (Winsock2)
- Thread havuzu (thread pool) ile eşzamanlı bağlantı desteği
- Statik dosya sunumu (HTML, CSS, JS, resim vs.)
- Basit HTTP 1.0/1.1 GET desteği

[ENG]
- Low-level socket programming in C++ (Winsock2)
- Thread pool for handling multiple concurrent connections
- Static file serving (HTML, CSS, JS, images, etc.)
- Basic HTTP 1.0/1.1 GET support

---

## Gereksinimler / Requirements
[TR]
- Windows işletim sistemi
- C++17 destekleyen bir derleyici (MSVC, MinGW vs.)
- Visual Studio, Code::Blocks, veya terminal kullanımı

[ENG]
- Windows operating system
- A C++17 compatible compiler (MSVC, MinGW, etc.)
- Visual Studio, Code::Blocks, or terminal usage

---

## Kurulum ve Derleme / Setup and Compilation

<pre><code>clang++ server.cpp threadpool.cpp -o server.exe -std=c++23 -O3</code></pre>


## Kullanım / Usage

[TR]
- server.exe'yi çalıştır.
- Tarayıcıdan şu adrese git: 127.0.0.1:8080


[ENG]
- run server.exe
- Open your browser and visit: 127.0.0.1:8080




