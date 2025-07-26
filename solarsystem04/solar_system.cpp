#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <string>
#include <sstream>

struct Planet {
    std::string name;
    float radius;           // Display radius
    float orbitRadius;      // Visual orbit radius
    float orbitalPeriod;    // In Earth years
    sf::Color color;
    float angle = 0.0f;     // Current position in orbit (radians)

    sf::CircleShape shape;
    sf::Text label;
    sf::Text info;
};

int main() {
    sf::RenderWindow window(sf::VideoMode(1400, 1000), "Solar System Simulation");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf"))
        return -1;

    sf::Vector2f sunPos(window.getSize().x / 2.f, window.getSize().y / 2.f);

    std::vector<Planet> planets = {
        {"Mercury",  4.f,   60.f,   0.24f, sf::Color(200, 200, 200)},
        {"Venus",    6.f,   90.f,   0.62f, sf::Color(255, 165, 0)},
        {"Earth",    7.f,  120.f,   1.00f, sf::Color::Blue},
        {"Mars",     5.f,  150.f,   1.88f, sf::Color(255, 80, 80)},
        {"Jupiter", 13.f,  200.f,  11.86f, sf::Color(255, 200, 150)},
        {"Saturn",  11.f,  260.f,  29.45f, sf::Color(255, 230, 150)},
        {"Uranus",   9.f,  310.f,  84.02f, sf::Color(150, 255, 255)},
        {"Neptune",  9.f,  360.f, 164.8f,  sf::Color(100, 150, 255)},
        {"Pluto",    3.f,  400.f, 248.0f,  sf::Color(180, 180, 200)}
    };

    // Setup visuals
    for (auto& p : planets) {
        p.shape.setRadius(p.radius);
        p.shape.setOrigin(p.radius, p.radius);
        p.shape.setFillColor(p.color);

        p.label.setFont(font);
        p.label.setCharacterSize(12);
        p.label.setFillColor(sf::Color::White);
        p.label.setString(p.name);

        std::stringstream ss;
        ss << "Period: " << p.orbitalPeriod << "y";
        p.info.setFont(font);
        p.info.setCharacterSize(10);
        p.info.setFillColor(sf::Color(180, 180, 180));
        p.info.setString(ss.str());
    }

    // Sun
    sf::CircleShape sun(30.f);
    sun.setOrigin(30.f, 30.f);
    sun.setPosition(sunPos);
    sun.setFillColor(sf::Color::Yellow);

    // Orbit rings
    std::vector<sf::CircleShape> orbits;
    for (const auto& p : planets) {
        sf::CircleShape orbit(p.orbitRadius);
        orbit.setOrigin(p.orbitRadius, p.orbitRadius);
        orbit.setPosition(sunPos);
        orbit.setFillColor(sf::Color::Transparent);
        orbit.setOutlineColor(sf::Color(80, 80, 80));
        orbit.setOutlineThickness(1);
        orbits.push_back(orbit);
    }

    float simulationSpeed = 1.0f; // Default time multiplier
    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed)
                window.close();
            if (e.type == sf::Event::KeyPressed) {
                if (e.key.code == sf::Keyboard::Up) simulationSpeed *= 1.2f;
                if (e.key.code == sf::Keyboard::Down) simulationSpeed /= 1.2f;
            }
        }

        float dt = clock.restart().asSeconds();

        window.clear(sf::Color::Black);
        window.draw(sun);

        // Draw orbits
        for (const auto& orbit : orbits)
            window.draw(orbit);

        // Update planets
        for (auto& p : planets) {
            float angularSpeed = 2 * 3.14159f / (p.orbitalPeriod * 365.25f); // radians/day
            p.angle += angularSpeed * dt * simulationSpeed * 50.0f;

            float x = sunPos.x + std::cos(p.angle) * p.orbitRadius;
            float y = sunPos.y + std::sin(p.angle) * p.orbitRadius;

            p.shape.setPosition(x, y);
            p.label.setPosition(x - p.label.getLocalBounds().width / 2, y - p.radius - 20);
            p.info.setPosition(x - p.info.getLocalBounds().width / 2, y + p.radius + 5);

            window.draw(p.shape);
            window.draw(p.label);
            window.draw(p.info);
        }

        window.display();
    }

    return 0;
}
