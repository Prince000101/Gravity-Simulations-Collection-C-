#include <SFML/Graphics.hpp>
#include <cmath>

constexpr float SCALE = 1e-9f;

class Sun {
public:
    sf::Vector2f position;
    float radius;
    sf::Color color;

    Sun(const sf::Vector2f& pos, float r)
        : position(pos), radius(r), color(sf::Color::Yellow) {}

    void draw(sf::RenderWindow& window, float scale) const {
        sf::CircleShape shape(radius * scale);
        shape.setFillColor(color);
        shape.setOrigin(radius * scale, radius * scale);
        shape.setPosition(position * scale + sf::Vector2f(window.getSize()) * 0.5f);
        window.draw(shape);
    }
};

class BlackHole {
public:
    sf::Vector2f position;
    float eventHorizonRadius;

    BlackHole(const sf::Vector2f& pos, float radius)
        : position(pos), eventHorizonRadius(radius) {}

    void draw(sf::RenderWindow& window, float scale, float time) const {
        sf::Vector2f center = position * scale + sf::Vector2f(window.getSize()) * 0.5f;

        // Accretion Disk (rotating)
        float diskRadius = eventHorizonRadius * 3.f * scale;
        sf::CircleShape disk(diskRadius);
        disk.setOrigin(diskRadius, diskRadius);
        disk.setPosition(center);
        disk.setFillColor(sf::Color::Transparent);
        disk.setOutlineThickness(6.f);
        disk.setOutlineColor(sf::Color(255, 100, 0, 180));
        disk.setRotation(std::fmod(time * 20, 360));
        window.draw(disk);

        // Event Horizon
        float ehRadius = eventHorizonRadius * scale;
        sf::CircleShape core(ehRadius);
        core.setOrigin(ehRadius, ehRadius);
        core.setPosition(center);
        core.setFillColor(sf::Color(10, 10, 10));
        window.draw(core);

        // Gravitational distortion
        sf::CircleShape distortion(ehRadius * 1.8f);
        distortion.setOrigin(distortion.getRadius(), distortion.getRadius());
        distortion.setPosition(center);
        distortion.setFillColor(sf::Color(0, 0, 0, 120));
        window.draw(distortion);
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Black Hole Light Bending Demo");
    window.setFramerateLimit(60);

    BlackHole blackHole(sf::Vector2f(0, 0), 4e9f); // Large black hole
    Sun sun(sf::Vector2f(6e9f, 0), 1.5e9f);         // Sun to the side

    float scale = SCALE;
    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Keyboard controls
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) sun.position.x -= 1e8f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) sun.position.x += 1e8f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) sun.position.y -= 1e8f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) sun.position.y += 1e8f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Add)) scale *= 1.05f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract)) scale *= 0.95f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
            sun.position = sf::Vector2f(6e9f, 0);
            scale = SCALE;
        }

        window.clear(sf::Color(5, 5, 20));

        float time = clock.getElapsedTime().asSeconds();
        sun.draw(window, scale);
        blackHole.draw(window, scale, time);

        window.display();
    }

    return 0;
}
