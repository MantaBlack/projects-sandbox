#include <SFML/Graphics.hpp>

static void test_sfml()
{
	sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(shape);
        window.display();
    }
}

// Draw Tutorial
static void draw_tutorial()
{
    // create the window
    sf::RenderWindow window;

    window.create(sf::VideoMode(800, 600),
        "My OpenGL window",
        sf::Style::Default,
        sf::ContextSettings(24));

    // run program as long as window is open
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

        // clear the window with a color.
        // this is mandatory otherwise new frame will be drawn on top of previous frame
        window.clear(sf::Color::Black);

        // draw everything here...
        sf::VertexArray vertices(sf::Points, 5);
        float spos = 50.0f;

        for (int i = 0; i < 5; ++i)
        {
            vertices[i].position = sf::Vector2f(i * spos, i * spos);
            vertices[i].color = sf::Color::Red;
        }

        window.draw(vertices);

        // end current frame
        window.display();
    }
}

int main()
{
    draw_tutorial();

    return 0;
}