#include "MyEntity.hpp"
#include <cmath>

void MyEntity::draw(sf::RenderTarget& target,
    sf::RenderStates states) const
{
    // apply the entity's trasnform: combine it with the one that was passed
    // by the caller
    states.transform *= getTransform(); // defined in sf::Transform

    // textures are not used
    states.texture = NULL;

    // draw the vertex array
    target.draw(m_vertices, states);
}

void MyEntity::reset_particle(std::size_t index)
{
    // give random velocity and lifetime to the particle
    float angle = (std::rand() % 360) * 3.14f / 180.f;
    float speed = (std::rand() % 50) + 50.f;
    m_particles[index].velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
    m_particles[index].lifetime = sf::milliseconds((std::rand() % 2000) + 2000);

    // reset the position of the corresponding vertex
    m_vertices[index].position = m_emitter;

    std::vector<sf::Color> colors = { sf::Color::Red, sf::Color::Green, sf::Color::Blue };
    m_vertices[index].color = colors[index % 3];
}

void MyEntity::set_emitter(sf::Vector2f position)
{
    m_emitter = position;
}

void MyEntity::update(sf::Time elapsed)
{
    for (std::size_t i = 0; i < m_particles.size(); ++i)
    {
        // update particle lifetime
        Particle& p = m_particles[i];
        p.lifetime -= elapsed;

        // if the particle is dead then respawn it
        if (p.lifetime <= sf::Time::Zero)
        {
            reset_particle(i);
        }

        // update the position of the corresponding vertex
        m_vertices[i].position += p.velocity * elapsed.asSeconds();

        // update the alpha (transparency) of the particle according to its lifetime
        float ratio = p.lifetime.asSeconds() / m_lifetime.asSeconds();
        m_vertices[i].color.a = static_cast<sf::Uint8>(ratio * 255);
    }
}