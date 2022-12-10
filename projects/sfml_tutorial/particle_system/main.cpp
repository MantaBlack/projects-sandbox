#include "ParticleSystem.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <iostream>
#include <string>

static unsigned int NUM_PARTICLES = 100000;

int main()
{
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Particle System!");

    // create the entity
    ParticleSystem bodies(NUM_PARTICLES);

    sf::Font font;

    if (!font.loadFromFile("/usr/share/fonts/truetype/ubuntu/UbuntuMono-R.ttf"))
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
        sf::Vector2f mouse_pos(.0f, .0f);
        sf::Vector2i mouse = sf::Mouse::getPosition(window);

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                mouse_pos = window.mapPixelToCoords(mouse);
                std::cout << mouse_pos.x << ", " << mouse_pos.y << std::endl;
            }
        }

        // make the partile system follow the mouse
        bodies.set_emitter(window.mapPixelToCoords(mouse));

        // update it
        sf::Time elapsed = clock.restart();

        sf::Clock timer;
        bodies.update(elapsed);
        sf::Time duration = timer.restart();

        window.clear();
        window.draw(bodies);
        text.setString(std::to_string(duration.asSeconds()));
        window.draw(text);
        window.display();
    }

    return 0;
}