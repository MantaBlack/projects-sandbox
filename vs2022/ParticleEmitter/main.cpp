#include "ParticleEmitter.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <cstdlib>
#include <iostream>
#include <string>

int main()
{
    const unsigned int WIDTH  = 1920;
    const unsigned int HEIGHT = 1080;
    const float LIFETIME = 3.f;
    const float RADIUS = 10.f;
    const std::size_t NUM_TRIANGLES = 12;
    const std::size_t NUM_PARTICLES = 1000;

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Particle Emitter");
    //window.setFramerateLimit(60);

    // create the entity
    ParticleEmitter particle_emitter(NUM_PARTICLES, LIFETIME, RADIUS, NUM_TRIANGLES);

    sf::Font font;

    if (!font.loadFromFile("saxmono.ttf"))
    {
        std::cerr << "Failed to load font" << std::endl;
        return 1;
    }

    sf::Text text;

    text.setFont(font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::Red);

    // create a clock to track the elapsed time
    sf::Clock clock;

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        // make the partile system follow the mouse
        sf::Vector2i mouse = sf::Mouse::getPosition(window);
        particle_emitter.set_emitter(window.mapPixelToCoords(mouse));

        // update it
        sf::Time elapsed = clock.restart();

        sf::Clock timer;
        particle_emitter.update(elapsed);
        sf::Time duration = timer.restart();

        window.clear();
        window.draw(particle_emitter);

        text.setString(std::to_string(duration.asSeconds()));
        window.draw(text);
        window.display();
    }

    return 0;
}