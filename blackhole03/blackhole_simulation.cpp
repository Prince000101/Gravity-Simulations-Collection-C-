#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <cmath>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>

struct Star {
    sf::Vector2f position;
    float angle;
    float radius;
    float speed;
    float fade;
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "2D Black Hole Simulation");
    window.setFramerateLimit(60);

    sf::Shader shader;
    if (!shader.loadFromFile("lens_distortion.frag", sf::Shader::Fragment)) {
        std::cerr << "Failed to load shader\n";
        return -1;
    }

    sf::RenderTexture scene;
    scene.create(800, 600);

    // Nebula Background
    sf::Image bgImage;
    bgImage.create(800, 600, sf::Color::Black);
    for (int i = 0; i < 1000; ++i) {
        int x = rand() % 800;
        int y = rand() % 600;
        sf::Color c(rand() % 255, rand() % 255, rand() % 255, 150);
        bgImage.setPixel(x, y, c);
    }
    sf::Texture bgTexture;
    bgTexture.loadFromImage(bgImage);
    sf::Sprite background(bgTexture);

    // Accretion disk
    sf::RenderTexture ringRender;
    int ringSize = 256;
    ringRender.create(ringSize, ringSize);
    sf::Vector2f center(ringSize / 2.f, ringSize / 2.f);
    for (int i = 0; i < 30; ++i) {
        float r = 100.f - i * 2.5f;
        sf::CircleShape c(r);
        c.setOrigin(r, r);
        c.setPosition(center);
        c.setFillColor(sf::Color(255, 140, 50, 8 + (30 - i) * 3));
        ringRender.draw(c, sf::BlendAdd);
    }
    ringRender.display();
    sf::Sprite ring(ringRender.getTexture());
    ring.setOrigin(center);

    // Stars
    std::vector<Star> stars;
    auto spawnStar = [&]() {
        Star s;
        s.angle = static_cast<float>(rand() % 360);
        s.radius = 150.f + static_cast<float>(rand() % 150);
        s.speed = 0.6f + static_cast<float>(rand() % 100) / 300.f;
        s.fade = 255.f;
        stars.push_back(s);
    };
    for (int i = 0; i < 40; ++i) spawnStar();

    // Light arcs (curved lines)
    sf::VertexArray arcs(sf::LinesStrip);

    sf::Vector2f bhPos(400.f, 300.f);
    float speed = 200.f;
    sf::Clock clock;
    int frameCount = 0;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::E) {
                    std::ostringstream ss;
                    ss << "frame_" << std::setw(3) << std::setfill('0') << frameCount++ << ".png";
                    scene.getTexture().copyToImage().saveToFile(ss.str());
                    std::cout << "Saved " << ss.str() << "\n";
                }
                if (event.key.code == sf::Keyboard::A) spawnStar();
                if (event.key.code == sf::Keyboard::D && !stars.empty()) stars.pop_back();
            }
        }

        // Keyboard movement
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) bhPos.x -= speed * dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) bhPos.x += speed * dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) bhPos.y -= speed * dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) bhPos.y += speed * dt;

        bhPos.x = std::clamp(bhPos.x, 0.f, 800.f);
        bhPos.y = std::clamp(bhPos.y, 0.f, 600.f);
        ring.setPosition(bhPos);
        ring.setRotation(clock.getElapsedTime().asSeconds() * 20);

        // Update stars
        for (auto& s : stars) {
            s.angle += s.speed * dt;
            s.radius -= 10.f * dt;
            s.fade -= 30.f * dt;
            if (s.radius < 40.f || s.fade <= 0) {
                s.radius = 200.f + rand() % 150;
                s.angle = rand() % 360;
                s.fade = 255.f;
            }
            s.position = bhPos + sf::Vector2f(std::cos(s.angle) * s.radius, std::sin(s.angle) * s.radius);
        }

        // Build arcs
        arcs.clear();
        for (auto& s : stars) {
            arcs.append(sf::Vertex(s.position, sf::Color(255, 255, 200, s.fade)));
        }

        // Render to texture
        scene.clear();
        scene.draw(background);
        scene.draw(arcs);
        scene.draw(ring, sf::BlendAdd);
        scene.display();

        shader.setUniform("texture", scene.getTexture());
        shader.setUniform("resolution", sf::Vector2f(800, 600));
        shader.setUniform("blackHolePos", bhPos);

        sf::Sprite finalScene(scene.getTexture());
        window.clear();
        window.draw(finalScene, &shader);
        window.display();
    }

    return 0;
}
