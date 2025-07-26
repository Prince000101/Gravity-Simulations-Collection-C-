#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

int main() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Small Movable Black Hole");
    window.setFramerateLimit(60);

    // Load background
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("stars.jpg")) {
        std::cerr << "Failed to load stars.jpg\n";
        return -1;
    }

    sf::Sprite background(backgroundTexture);
    background.setScale(
        static_cast<float>(window.getSize().x) / backgroundTexture.getSize().x,
        static_cast<float>(window.getSize().y) / backgroundTexture.getSize().y
    );

    // Load shader
    sf::Shader shader;
    if (!shader.loadFromFile("lens_distortion.frag", sf::Shader::Fragment)) {
        std::cerr << "Failed to load lens_distortion.frag\n";
        return -1;
    }

    sf::Vector2f bh_pos = sf::Vector2f(window.getSize()) * 0.5f;
    float speed = 200.f;  // Pixels per second
    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Time step
        float dt = clock.restart().asSeconds();

        // Keyboard movement
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  bh_pos.x -= speed * dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) bh_pos.x += speed * dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    bh_pos.y -= speed * dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  bh_pos.y += speed * dt;

        // Clamp to screen bounds
        bh_pos.x = std::max(0.f, std::min(bh_pos.x, static_cast<float>(window.getSize().x)));
        bh_pos.y = std::max(0.f, std::min(bh_pos.y, static_cast<float>(window.getSize().y)));

        // Convert to normalized coordinates [0, 1]
        sf::Vector2f bh_uv = sf::Vector2f(
            bh_pos.x / window.getSize().x,
            bh_pos.y / window.getSize().y
        );

        // Set shader uniforms
        shader.setUniform("texture", backgroundTexture);
        shader.setUniform("blackHolePos", bh_uv);
        shader.setUniform("time", clock.getElapsedTime().asSeconds());

        // Draw
        window.clear();
        window.draw(background, &shader);
        window.display();
    }

    return 0;
}
