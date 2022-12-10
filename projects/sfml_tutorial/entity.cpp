#include "MyEntity.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <iostream>
#include <string>

static unsigned int NUM_PARTICLES = 1000000;

int main()
{
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "My Entity!");

    // create the entity
    MyEntity my_entity(NUM_PARTICLES);

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

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        // make the partile system follow the mouse
        sf::Vector2i mouse = sf::Mouse::getPosition(window);
        my_entity.set_emitter(window.mapPixelToCoords(mouse));

        // update it
        sf::Time elapsed = clock.restart();

        sf::Clock timer;
        my_entity.update(elapsed);
        sf::Time duration = timer.restart();

        window.clear();
        window.draw(my_entity);
        text.setString(std::to_string(duration.asSeconds()));
        window.draw(text);
        window.display();
    }

    return 0;
}