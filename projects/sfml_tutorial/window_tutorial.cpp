#include <SFML/Window.hpp>

int main()
{
    sf::Window window;
    window.create(sf::VideoMode(800, 600), "My window");

    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;

        while (window.pollEvent(event))
        {
            // check whether close event was requested
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }
    }

    return 0;
}