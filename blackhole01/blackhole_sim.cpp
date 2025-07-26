#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <vector>
#include <cstdlib>

int main() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "🔥 Black Hole Simulation");
    window.setFramerateLimit(60);

    // Load background
    sf::Texture bgTex;
    if (!bgTex.loadFromFile("stars.jpg")) {
        std::cerr << "Missing stars.jpg\n";
        return 1;
    }
    sf::Sprite background(bgTex);
    background.setScale(
        (float)window.getSize().x / bgTex.getSize().x,
        (float)window.getSize().y / bgTex.getSize().y
    );

    // Load accretion ring
    sf::Texture ringTex;
    if (!ringTex.loadFromFile("ring.png")) {
        std::cerr << "Missing ring.png\n";
        return 1;
    }
    ringTex.setSmooth(true);
    sf::Sprite ring(ringTex);
    ring.setOrigin(ringTex.getSize().x / 2, ringTex.getSize().y / 2);
    ring.setScale(0.3f, 0.3f);

    // Shader
    sf::Shader shader;
    if (!shader.loadFromFile("lens_distortion.frag", sf::Shader::Fragment)) {
        std::cerr << "Missing lens_distortion.frag\n";
        return 1;
    }

    sf::Vector2f bh_pos(window.getSize().x / 2, window.getSize().y / 2);
    sf::Vector2f velocity(0.f, 0.f);
    float acceleration = 800.f;
    float drag = 0.9f;
    float ringRotation = 0;

    // Orbiting stars
    struct Star {
        sf::CircleShape shape;
        float angle, distance, speed;
    };
    std::vector<Star> orbitStars;
    for (int i = 0; i < 20; ++i) {
        float d = 50 + rand() % 200;
        float a = rand() % 360;
        float s = 10 + rand() % 40;
        sf::CircleShape shape(2.f);
        shape.setFillColor(sf::Color::White);
        orbitStars.push_back({shape, a, d, s});
    }

    sf::Clock clock;
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        sf::Event event;
        while (window.pollEvent(event))
            if (event.type == sf::Event::Closed)
                window.close();

        // Movement input
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  velocity.x -= acceleration * dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) velocity.x += acceleration * dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    velocity.y -= acceleration * dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  velocity.y += acceleration * dt;

        velocity *= drag;
        bh_pos += velocity * dt;

        // Clamp
        bh_pos.x = std::clamp(bh_pos.x, 0.f, (float)window.getSize().x);
        bh_pos.y = std::clamp(bh_pos.y, 0.f, (float)window.getSize().y);

        // Shader uniforms
        sf::Vector2f bh_uv(bh_pos.x / window.getSize().x, bh_pos.y / window.getSize().y);
        shader.setUniform("texture", bgTex);
        shader.setUniform("blackHolePos", bh_uv);
        shader.setUniform("time", clock.getElapsedTime().asSeconds());

        // Update orbiting stars
        for (auto& star : orbitStars) {
            star.angle += star.speed * dt;
            float rad = star.angle * 3.14159f / 180.f;
            sf::Vector2f p = bh_pos + sf::Vector2f(std::cos(rad), std::sin(rad)) * star.distance;
            star.shape.setPosition(p);
        }

        // Accretion disk rotation
        ring.setPosition(bh_pos);
        ringRotation += 20 * dt;
        ring.setRotation(ringRotation);

        // Render
        window.clear();
        window.draw(background, &shader);
        for (auto& star : orbitStars) window.draw(star.shape);
        window.draw(ring); // Draw on top of distortion
        window.display();
    }

    return 0;
}
