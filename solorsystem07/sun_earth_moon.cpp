#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <sstream>
#include <iomanip>

constexpr float PI = 3.14159265f;
constexpr float TWO_PI = 2.f * PI;
constexpr float EARTH_YEAR_DAYS = 365.25f;
constexpr float LUNAR_MONTH_DAYS = 29.53f; // Moon cycle approx
constexpr float FPS = 60.f;

struct ClockDisplay {
    int year=0, day=0, hour=0, minute=0;
    void update(float simDays) {
        float totalHours = simDays*24.f;
        year = int(totalHours/(24.f*EARTH_YEAR_DAYS));
        float remH = std::fmod(totalHours, 24.f*EARTH_YEAR_DAYS);
        day  = int(remH/24.f);
        float rem = std::fmod(remH,24.f);
        hour = int(rem);
        minute = int((rem - hour)*60.f);
    }
    std::string str() const {
        std::ostringstream o;
        o << "Year " << year << ", Day " << day << ", "
          << std::setw(2) << std::setfill('0') << hour << ":"
          << std::setw(2) << minute;
        return o.str();
    }
};

struct Celestial {
    float radius;
    sf::Color color;
    sf::CircleShape shape;
    Celestial(float r, sf::Color c):radius(r),color(c) {
        shape.setRadius(r);
        shape.setOrigin(r,r);
        shape.setFillColor(c);
    }
    void setPos(sf::Vector2f p) { shape.setPosition(p); }
    sf::Vector2f getPos() const { return shape.getPosition(); }
};

void drawTerminator(sf::RenderWindow& w, sf::Vector2f pos, float R, float angle) {
    int SEG = 60;
    sf::VertexArray fan(sf::TriangleFan, SEG+2);
    fan[0] = {pos, sf::Color::Transparent};
    for(int i=0; i<=SEG; i++) {
        float a = angle - PI/2 + PI*i/SEG;
        sf::Vector2f p = pos + sf::Vector2f{std::cos(a), std::sin(a)} * R;
        fan[i+1] = {p, sf::Color(0,0,0,160)};
    }
    w.draw(fan);
}

void drawSunBeam(sf::RenderWindow& w, sf::Vector2f s, sf::Vector2f e) {
    sf::Vector2f d = e - s;
    float L = std::hypot(d.x,d.y);
    if (L < 1.f) return;
    auto u = d / L;
    auto P = sf::Vector2f(-u.y, u.x);
    float w0 = 40.f, w1 = 80.f;

    sf::Vertex quad[4] = {
        {s + P*(w0/2.f), sf::Color(255,255,150,120)},
        {s - P*(w0/2.f), sf::Color(255,255,150,120)},
        {e - u*10.f + P*(w1/2.f), sf::Color(255,255,150,20)},
        {e - u*10.f - P*(w1/2.f), sf::Color(255,255,150,20)}
    };
    w.draw(quad, 4, sf::TriangleStrip);
}

void drawEarthShadow(sf::RenderWindow& w, sf::Vector2f earthPos, float earthR) {
    sf::CircleShape shadow(earthR * 2.f);
    shadow.setOrigin(earthR*2.f, earthR*2.f);
    shadow.setPosition(earthPos);
    shadow.setFillColor(sf::Color(0,0,0,100));
    w.draw(shadow);
}

void drawMoon(sf::RenderWindow& w, sf::Vector2f pos, float r, sf::Vector2f sunPos) {
    sf::CircleShape moon(r);
    moon.setOrigin(r,r);
    moon.setPosition(pos);
    moon.setFillColor(sf::Color(200,200,200));
    w.draw(moon);

    // Moon phase shading
    sf::Vector2f moonToSun = sunPos - pos;
    sf::Vector2f moonToEarth = pos - sf::Vector2f(450,300);
    float phaseAngle = std::acos(
        (moonToSun.x*moonToEarth.x + moonToSun.y*moonToEarth.y) /
        (std::hypot(moonToSun.x, moonToSun.y)*std::hypot(moonToEarth.x, moonToEarth.y) + 0.0001f)
    );

    sf::CircleShape shadow(r);
    shadow.setOrigin(r,r);
    shadow.setPosition(pos);

    sf::Color shadowColor(0,0,0,180);

    if (phaseAngle < PI/2) {
        shadow.setFillColor(shadowColor);
        sf::Vector2f offset = sf::Vector2f(std::cos(phaseAngle), std::sin(phaseAngle)) * r * 0.8f;
        shadow.setPosition(pos + offset);
        w.draw(shadow);
    }
}

// New: Draw Moon shadow on Earth (solar eclipse)
void drawMoonShadowOnEarth(sf::RenderWindow& w, sf::Vector2f sunPos, sf::Vector2f earthPos, float earthR,
                           sf::Vector2f moonPos, float moonR) {
    // Vector from Sun to Earth
    sf::Vector2f sunToEarth = earthPos - sunPos;
    float sunToEarthDist = std::hypot(sunToEarth.x, sunToEarth.y);
    sf::Vector2f sunToEarthDir = sunToEarth / sunToEarthDist;

    // Vector from Sun to Moon
    sf::Vector2f sunToMoon = moonPos - sunPos;
    float sunToMoonDist = std::hypot(sunToMoon.x, sunToMoon.y);
    sf::Vector2f sunToMoonDir = sunToMoon / sunToMoonDist;

    // Check if Moon is between Sun and Earth (simple projection test)
    float proj = (sunToMoonDir.x * sunToEarthDir.x + sunToMoonDir.y * sunToEarthDir.y);
    bool between = (proj > 0.99f) && (sunToMoonDist < sunToEarthDist);

    // Check distance between moon line and earth line (perpendicular)
    sf::Vector2f moonToEarthVec = earthPos - moonPos;
    float perpendicularDist = std::abs(moonToEarthVec.x * sunToEarthDir.y - moonToEarthVec.y * sunToEarthDir.x);

    if (between && perpendicularDist < earthR + moonR) {
        // Draw a dark circle (shadow) on Earth at the shadow position

        // Calculate shadow position on Earth surface
        sf::Vector2f shadowPos = earthPos - sunToEarthDir * earthR * 0.7f;

        sf::CircleShape shadow(moonR * 1.5f);
        shadow.setOrigin(shadow.getRadius(), shadow.getRadius());
        shadow.setPosition(shadowPos);
        shadow.setFillColor(sf::Color(20,20,20,180));
        w.draw(shadow);
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(900, 600), "Sun-Earth-Moon Simulation");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        std::cerr << "Failed to load font\n";
        return -1;
    }

    Celestial sun(50.f, sf::Color(255, 255, 100));
    Celestial earth(20.f, sf::Color(100, 150, 255));
    Celestial moon(8.f, sf::Color(200, 200, 200));

    float simDays = 0.f;
    float speed = 1.f;
    float targetSpeed = 1.f;
    bool paused = false;

    sf::Vector2f pan(0.f,0.f);
    float zoom = 1.f;
    bool dragging = false;
    sf::Vector2i dragStart;
    sf::Vector2f panStart;

    ClockDisplay clockDisp;

    sf::Clock deltaClock;

    sf::Vector2f center(450, 300);

    while(window.isOpen()) {
        sf::Event event;
        while(window.pollEvent(event)) {
            if(event.type == sf::Event::Closed) window.close();

            if(event.type == sf::Event::MouseWheelScrolled) {
                if (event.mouseWheelScroll.delta > 0)
                    zoom *= 1.1f;
                else
                    zoom /= 1.1f;
            }
            if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                dragging = true;
                dragStart = sf::Mouse::getPosition(window);
                panStart = pan;
            }
            if(event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                dragging = false;
            }
            if(event.type == sf::Event::KeyPressed) {
                if(event.key.code == sf::Keyboard::Up) {
                    targetSpeed *= 1.5f;
                }
                if(event.key.code == sf::Keyboard::Down) {
                    targetSpeed /= 1.5f;
                    if (targetSpeed < 0.01f) targetSpeed = 0.01f;
                }
                if(event.key.code == sf::Keyboard::Space) {
                    paused = !paused;
                }
            }
        }

        if(dragging) {
            sf::Vector2i mp = sf::Mouse::getPosition(window);
            pan = panStart + sf::Vector2f(mp - dragStart);
        }

        float dt = deltaClock.restart().asSeconds();

        speed += (targetSpeed - speed) * dt * 5.f;

        if(paused) dt = 0.f;

        simDays += dt * FPS * speed;

        // Calculate positions
        float earthAngle = TWO_PI * (simDays / EARTH_YEAR_DAYS);
        sf::Vector2f earthPos = center + sf::Vector2f(std::cos(earthAngle), std::sin(earthAngle)) * 220.f;
        earth.setPos(earthPos);

        float moonAngle = TWO_PI * (simDays / LUNAR_MONTH_DAYS);
        sf::Vector2f moonPos = earthPos + sf::Vector2f(std::cos(moonAngle), std::sin(moonAngle)) * 50.f;
        moon.setPos(moonPos);

        window.clear(sf::Color(10,10,20));

        sf::View view = window.getView();
        view.setCenter(center - pan / zoom);
        view.setSize(window.getSize().x / zoom, window.getSize().y / zoom);
        window.setView(view);

        sun.setPos(center);
        window.draw(sun.shape);

        drawSunBeam(window, center, earthPos);

        drawEarthShadow(window, earthPos, earth.radius);

        drawTerminator(window, earthPos, earth.radius, earthAngle);

        sf::CircleShape glow(earth.radius + 4.f);
        glow.setOrigin(earth.radius + 4.f, earth.radius + 4.f);
        glow.setPosition(earthPos);
        glow.setFillColor(sf::Color(50, 100, 255, 50));
        window.draw(glow);

        window.draw(earth.shape);

        // Draw Moon shadow on Earth (solar eclipse)
        drawMoonShadowOnEarth(window, center, earthPos, earth.radius, moonPos, moon.radius);

        // Lunar eclipse detection (moon in Earth's shadow cone)
        bool inEclipse = false;
        sf::Vector2f earthToSun = center - earthPos;
        sf::Vector2f earthToMoon = moonPos - earthPos;
        float proj = (earthToMoon.x*earthToSun.x + earthToMoon.y*earthToSun.y) /
                     (std::hypot(earthToSun.x, earthToSun.y) + 0.0001f);
        float dist = std::hypot(earthToMoon.x - proj * (earthToSun.x/std::hypot(earthToSun.x, earthToSun.y)),
                                earthToMoon.y - proj * (earthToSun.y/std::hypot(earthToSun.x, earthToSun.y)));
        if (proj > 0 && proj < 50 && dist < earth.radius*0.7f)
            inEclipse = true;

        if (inEclipse) {
            sf::CircleShape bloodMoon(moon.radius);
            bloodMoon.setOrigin(moon.radius, moon.radius);
            bloodMoon.setPosition(moonPos);
            bloodMoon.setFillColor(sf::Color(120, 20, 20, 200));
            window.draw(bloodMoon);
        } else {
            drawMoon(window, moonPos, moon.radius, center);
        }

        window.draw(moon.shape);

        window.setView(window.getDefaultView());

        sf::Text hudText;
        hudText.setFont(font);
        hudText.setCharacterSize(16);
        hudText.setFillColor(sf::Color::White);
        hudText.setPosition(10.f, 10.f);

        clockDisp.update(simDays);
        std::ostringstream hudStream;
        hudStream << "Simulation Time: " << clockDisp.str() << "\n";
        hudStream << "Speed: " << std::fixed << std::setprecision(2) << speed << "x " << (paused ? "(Paused)" : "") << "\n";
        hudStream << "Controls:\n - Arrow Up/Down: Speed\n - Space: Pause\n - Mouse Drag: Pan\n - Mouse Wheel: Zoom\n";
        hudText.setString(hudStream.str());

        window.draw(hudText);

        window.display();
    }

    return 0;
}
