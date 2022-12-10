#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream>

int main()
{
    sf::Window window;
    window.create(sf::VideoMode(800, 600),
                  "My OpenGL window",
                  sf::Style::Default,
                  sf::ContextSettings(24));

    // activate the window
    window.setActive(true);

    sf::ContextSettings settings = window.getSettings();

    std::cout << "depth bits:" << settings.depthBits << std::endl;
    std::cout << "stencil bits:" << settings.stencilBits << std::endl;
    std::cout << "antialiasing level:" << settings.antialiasingLevel << std::endl;
    std::cout << "version:" << settings.majorVersion << "." << settings.minorVersion << std::endl;

    // run the main loop
    bool running = true;

    while (running)
    {
        // handle events
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                // end the program
                running = false;
            }
            else if (event.type == sf::Event::Resized)
            {
                // adjust the viewport when the window is resized
                glViewport(0,
                           0,
                           event.size.width,
                           event.size.height);
            }
        }

        // clear the buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw . . .

        // deactivate the window's context
        window.setActive(false);

        // end the current frame (internally swaps front and back buffers)
        window.display();
    }

    // release resources

    return 0;
}