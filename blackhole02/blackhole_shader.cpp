#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <cmath>
#include <iostream>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Black Hole Visual FX");
    window.setFramerateLimit(60);

    // Load shader
    sf::Shader shader;
    if (!shader.loadFromFile("lens_distortion.frag", sf::Shader::Fragment)) {
        std::cerr << "Error: Couldn't load lens_distortion.frag\n";
        return -1;
    }

    // Background
    sf::Texture bgTexture;
    bgTexture.create(800, 600);
    sf::Image bgImage;
    bgImage.create(800, 600, sf::Color::Black);

    // Fill with stars
    for (int i = 0; i < 400; ++i) {
        int x = rand() % 800;
        int y = rand() % 600;
        bgImage.setPixel(x, y, sf::Color(200 + rand() % 55, 200 + rand() % 55, 255));
    }

    bgTexture.update(bgImage);
    sf::Sprite background(bgTexture);

    // Accretion ring generation
    sf::RenderTexture ringRender;
    int ringSize = 256;
    ringRender.create(ringSize, ringSize);
    ringRender.clear(sf::Color::Transparent);
    sf::Vector2f center(ringSize / 2.f, ringSize / 2.f);

    for (int i = 0; i < 30; ++i) {
        float radius = 100.f - i * 2.5f;
        sf::CircleShape circle(radius);
        circle.setOrigin(radius, radius);
        circle.setPosition(center);
        sf::Color color(255, 180, 50, 8 + (30 - i) * 3);
        circle.setFillColor(color);
        ringRender.draw(circle, sf::BlendAdd);
    }

    sf::CircleShape hole(40.f);
    hole.setOrigin(40.f, 40.f);
    hole.setPosition(center);
    hole.setFillColor(sf::Color(0, 0, 0, 0));
    ringRender.draw(hole, sf::BlendNone);
    ringRender.display();

    sf::Texture ringTexture = ringRender.getTexture();
    sf::Sprite ring(ringTexture);
    ring.setOrigin(ringSize / 2.f, ringSize / 2.f);
    ring.setScale(1.f, 1.f);

    // Off-screen rendering target
    sf::RenderTexture scene;
    scene.create(800, 600);

    // Black hole position
    sf::Vector2f bhPos(400.f, 300.f);
    float speed = 200.f;

    sf::Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
        }

        // Movement
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) bhPos.x -= speed * dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) bhPos.x += speed * dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) bhPos.y -= speed * dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) bhPos.y += speed * dt;

        // Keep on screen
        bhPos.x = std::clamp(bhPos.x, 0.f, 800.f);
        bhPos.y = std::clamp(bhPos.y, 0.f, 600.f);

        // Accretion ring pulsing
        float pulse = 1.0f + 0.1f * std::sin(clock.getElapsedTime().asSeconds() * 4);
        ring.setScale(pulse, pulse);
        ring.setRotation(clock.getElapsedTime().asSeconds() * 20);
        ring.setPosition(bhPos);

        // Render scene to texture
        scene.clear();
        scene.draw(background);
        scene.draw(ring, sf::BlendAdd);
        scene.display();

        // Apply shader with black hole position
        shader.setUniform("texture", scene.getTexture());
        shader.setUniform("resolution", sf::Vector2f(800, 600));
        shader.setUniform("blackHolePos", bhPos);
        shader.setUniform("time", clock.getElapsedTime().asSeconds());

        // Final draw
        sf::Sprite screenSprite(scene.getTexture());

        window.clear();
        window.draw(screenSprite, &shader);
        window.display();
    }

    return 0;
}
