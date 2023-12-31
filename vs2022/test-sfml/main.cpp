#include <SFML/Graphics.hpp>

#include <cmath>
#include <iostream>

static void test_sfml()
{
	sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
    const float radius = 2.f;

    sf::CircleShape shape1(radius);
    shape1.setFillColor(sf::Color::Green);

    sf::CircleShape shape2(radius);
    shape2.setFillColor(sf::Color::Red);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        
        float x = 50.f - radius;
        float y = 50.f - radius;

        shape1.setPosition(x, y);
        window.draw(shape1);
        
        shape2.setPosition(x+50.f, y+50.f);
        window.draw(shape2);

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


static void draw_circle()
{
    const float pi            = 22.f / 7.f;
    const float end           = 2.f * pi;
    const float radius        = 100.f;
    const int num_triangles   = 12;
    const int total_vertices  = num_triangles * 3;
    const float sector_angle  = end / num_triangles;
    const sf::Color colors[3] = {sf::Color::Red, sf::Color::Green, sf::Color::Blue};
    
    sf::RenderWindow window;

    window.create(sf::VideoMode(800, 800),
        "Draw Circle",
        sf::Style::Default,
        sf::ContextSettings(24));

    window.setFramerateLimit(60);

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

        window.clear(sf::Color::Black);

        sf::VertexArray vertices(sf::Triangles);

        const sf::Vector2f center(400.f, 400.f);
        float theta = 0.f;
        int c = 0;

        for (; vertices.getVertexCount() < total_vertices; theta += sector_angle)
        {
            float x = std::cos(theta) * radius;
            float y = std::sin(theta) * radius;

            if (vertices.getVertexCount() == 0)
            {
                vertices.append(sf::Vertex(center, colors[c]));
            }
            else if (vertices.getVertexCount() < 3)
            {
                vertices.append(sf::Vertex(center + sf::Vector2f(x, y), colors[c]));
            }
            else
            {
                c += 1;

                if (c >= 3)
                {
                    c = 0;
                }

                vertices.append(sf::Vertex(vertices[vertices.getVertexCount() - 1].position, colors[c]));
                vertices.append(sf::Vertex(center, colors[c]));
                vertices.append(sf::Vertex(center + sf::Vector2f(x, y), colors[c]));
            }
        }

        window.draw(vertices);

        window.display();
    }
}


int main()
{
    draw_circle();

    return 0;
}