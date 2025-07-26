#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <string>
#include <iostream>

struct CelestialBody {
    float radius;               // visual size
    float orbitRadius;          // distance from parent
    float orbitalPeriod;        // in Earth days or years (will normalize)
    sf::Color color;
    float initialAngle;         // starting angle in radians
    sf::Vector2f position;

    sf::CircleShape shape;
    sf::CircleShape ring;       // for planets with rings
    bool hasRings = false;

    CelestialBody* parent = nullptr;
    std::vector<CelestialBody> moons;

    // Initialize shape and ring
    void init() {
        shape.setRadius(radius);
        shape.setOrigin(radius, radius);
        shape.setFillColor(color);

        if (hasRings) {
            ring.setRadius(radius * 1.7f);
            ring.setOrigin(ring.getRadius(), ring.getRadius());
            ring.setFillColor(sf::Color::Transparent);
            ring.setOutlineThickness(radius * 0.15f);
            ring.setOutlineColor(sf::Color(200, 180, 100, 150));
        }

        for (auto& moon : moons)
            moon.init();
    }

    // Calculate position exactly by angle based on elapsed time, no drift
    void updatePosition(const sf::Vector2f& parentPos, float elapsedDays) {
        float periodDays = orbitalPeriod;
        if (parent == nullptr)
            periodDays *= 365.25f; // planets have period in years

        float angle = initialAngle + 2.f * 3.14159265f * (elapsedDays / periodDays);
        float x = parentPos.x + std::cos(angle) * orbitRadius;
        float y = parentPos.y + std::sin(angle) * orbitRadius;
        position = sf::Vector2f(x, y);
        shape.setPosition(position);

        if (hasRings)
            ring.setPosition(position);

        for (auto& moon : moons)
            moon.updatePosition(position, elapsedDays);
    }

    void draw(sf::RenderWindow& window, float zoom, const sf::Vector2f& panOffset) const {
        // Draw orbit ring around parent
        if (parent != nullptr) {
            sf::CircleShape orbitRing(orbitRadius * zoom);
            orbitRing.setOrigin(orbitRadius * zoom, orbitRadius * zoom);
            orbitRing.setPosition(parent->position * zoom + panOffset);
            orbitRing.setFillColor(sf::Color::Transparent);
            orbitRing.setOutlineColor(sf::Color(100, 100, 100, 100));
            orbitRing.setOutlineThickness(1.f);
            window.draw(orbitRing);
        }

        // Draw rings if any
        if (hasRings) {
            sf::CircleShape ringDraw = ring;
            ringDraw.setScale(zoom, zoom);
            ringDraw.move(panOffset);
            window.draw(ringDraw);
        }

        sf::CircleShape shapeDraw = shape;
        shapeDraw.setScale(zoom, zoom);
        shapeDraw.move(panOffset);
        window.draw(shapeDraw);

        // Draw moons recursively
        for (const auto& moon : moons)
            moon.draw(window, zoom, panOffset);
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(1400, 1000), "Solar System Clean");
    window.setFramerateLimit(60);

    sf::Vector2f sunPos(window.getSize().x / 2.f, window.getSize().y / 2.f);

    CelestialBody sun{
        30.f, 0.f, 0.f,
        sf::Color::Yellow,
        0.f,
        sunPos,
        sf::CircleShape(), sf::CircleShape(), false,
        nullptr,
        {}
    };
    sun.init();

    auto makeMoon = [](float radius, float orbitR, float period, sf::Color c, float initialAngle = 0.f) {
        CelestialBody moon{radius, orbitR, period, c, initialAngle};
        moon.parent = nullptr;
        moon.init();
        return moon;
    };

    std::vector<CelestialBody> planets = {
        {6.f, 60.f, 0.24f, sf::Color(200, 200, 200), 0.1f, {}, sf::CircleShape(), sf::CircleShape(), false, nullptr, {}},
        {9.f, 90.f, 0.62f, sf::Color(255, 165, 0), 0.5f, {}, sf::CircleShape(), sf::CircleShape(), false, nullptr, {}},
        {10.f, 130.f, 1.00f, sf::Color::Blue, 1.0f, {}, sf::CircleShape(), sf::CircleShape(), false, nullptr, {
            {3.f, 20.f, 27.3f, sf::Color(150,150,150), 0.f, {}, sf::CircleShape(), sf::CircleShape(), false, nullptr, {}}
        }},
        {8.f, 170.f, 1.88f, sf::Color(255, 80, 80), 1.8f, {}, sf::CircleShape(), sf::CircleShape(), false, nullptr, {
            {2.f, 15.f, 0.319f, sf::Color(160,160,160), 0.3f, {}, sf::CircleShape(), sf::CircleShape(), false, nullptr, {}},
            {2.f, 22.f, 1.263f, sf::Color(180,180,180), 0.6f, {}, sf::CircleShape(), sf::CircleShape(), false, nullptr, {}}
        }},
        {25.f, 220.f, 11.86f, sf::Color(255, 200, 150), 3.0f, {}, sf::CircleShape(), sf::CircleShape(), true, nullptr, {
            {4.f, 30.f, 1.77f, sf::Color(255, 200, 100), 0.1f, {}, sf::CircleShape(), sf::CircleShape(), false, nullptr, {}},
            {4.f, 38.f, 3.55f, sf::Color(180, 180, 220), 0.3f, {}, sf::CircleShape(), sf::CircleShape(), false, nullptr, {}},
            {5.f, 46.f, 7.15f, sf::Color(150, 150, 200), 0.5f, {}, sf::CircleShape(), sf::CircleShape(), false, nullptr, {}},
            {5.f, 54.f, 16.7f, sf::Color(140, 140, 160), 0.7f, {}, sf::CircleShape(), sf::CircleShape(), false, nullptr, {}}
        }},
        {22.f, 280.f, 29.45f, sf::Color(255, 230, 150), 5.0f, {}, sf::CircleShape(), sf::CircleShape(), true, nullptr, {
            {5.f, 30.f, 15.95f, sf::Color(230, 200, 150), 0.2f, {}, sf::CircleShape(), sf::CircleShape(), false, nullptr, {}},
            {4.f, 40.f, 4.52f, sf::Color(220, 210, 200), 0.5f, {}, sf::CircleShape(), sf::CircleShape(), false, nullptr, {}},
            {4.f, 48.f, 79.3f, sf::Color(200, 190, 180), 0.7f, {}, sf::CircleShape(), sf::CircleShape(), false, nullptr, {}}
        }},
        {18.f, 340.f, 84.02f, sf::Color(150, 255, 255), 7.0f, {}, sf::CircleShape(), sf::CircleShape(), true, nullptr, {
            {3.f, 22.f, 1.41f, sf::Color(180, 200, 200), 0.3f, {}, sf::CircleShape(), sf::CircleShape(), false, nullptr, {}},
            {4.f, 30.f, 2.52f, sf::Color(160, 190, 190), 0.6f, {}, sf::CircleShape(), sf::CircleShape(), false, nullptr, {}},
            {4.f, 38.f, 4.14f, sf::Color(140, 170, 170), 0.9f, {}, sf::CircleShape(), sf::CircleShape(), false, nullptr, {}}
        }},
        {17.f, 390.f, 164.8f, sf::Color(100, 150, 255), 8.0f, {}, sf::CircleShape(), sf::CircleShape(), true, nullptr, {
            {5.f, 28.f, 5.88f, sf::Color(120, 170, 210), 0.4f, {}, sf::CircleShape(), sf::CircleShape(), false, nullptr, {}}
        }},
        {5.f, 430.f, 248.0f, sf::Color(180, 180, 200), 9.0f, {}, sf::CircleShape(), sf::CircleShape(), false, nullptr, {
            {3.f, 20.f, 6.39f, sf::Color(160, 160, 180), 0.1f, {}, sf::CircleShape(), sf::CircleShape(), false, nullptr, {}}
        }}
    };

    for (auto& planet : planets) {
        planet.parent = &sun;
        planet.init();
        for (auto& moon : planet.moons)
            moon.parent = &planet;
    }

    float simulationSpeed = 1.0f; // days per second
    float zoom = 1.0f;
    sf::Vector2f panOffset(0.f, 0.f);

    sf::Clock clock;
    bool dragging = false;
    sf::Vector2i dragStart;
    sf::Vector2f panStart;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            else if (event.type == sf::Event::MouseWheelScrolled) {
                if (event.mouseWheelScroll.delta > 0)
                    zoom *= 1.1f;
                else
                    zoom /= 1.1f;

                if (zoom < 0.1f) zoom = 0.1f;
                if (zoom > 10.f) zoom = 10.f;
            }

            else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                dragging = true;
                dragStart = sf::Mouse::getPosition(window);
                panStart = panOffset;
            }
            else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                dragging = false;
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up) simulationSpeed *= 1.2f;
                if (event.key.code == sf::Keyboard::Down) simulationSpeed /= 1.2f;
                if (simulationSpeed < 0.01f) simulationSpeed = 0.01f;
                if (simulationSpeed > 100.f) simulationSpeed = 100.f;
            }
        }

        if (dragging) {
            sf::Vector2i pos = sf::Mouse::getPosition(window);
            sf::Vector2i delta = pos - dragStart;
            panOffset = panStart + sf::Vector2f(delta.x, delta.y);
        }

        float dt = clock.restart().asSeconds();
        static float elapsedDays = 0.f;
        elapsedDays += dt * simulationSpeed;

        window.clear(sf::Color::Black);

        // Draw Sun at center + pan + zoom
        sf::Vector2f sunScreenPos = sunPos * zoom + panOffset;
        sun.shape.setPosition(sunScreenPos);
        sun.shape.setScale(zoom, zoom);
        window.draw(sun.shape);

        // Draw planet orbits + planets + moons
        for (auto& planet : planets) {
            planet.updatePosition(sunScreenPos, elapsedDays);
            planet.draw(window, zoom, panOffset);
        }

        // Show simulation speed info (optional)
        sf::Font font;
        if (font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf")) {
            sf::Text speedText;
            speedText.setFont(font);
            speedText.setCharacterSize(18);
            speedText.setFillColor(sf::Color::White);
            speedText.setPosition(10, 10);
            speedText.setString("Speed (Up/Down): " + std::to_string(simulationSpeed) + " days/sec\n"
                                "Mouse drag to pan\nMouse wheel to zoom");
            window.draw(speedText);
        }

        window.display();
    }
    return 0;
}
