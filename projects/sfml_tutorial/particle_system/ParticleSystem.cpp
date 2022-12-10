#include "ParticleSystem.hpp"

#include <cmath>
#include <iostream>


void ParticleSystem::draw(sf::RenderTarget& target,
                          sf::RenderStates states) const
{
    states.transform *= getTransform();
    states.texture = NULL;

    for (std::size_t i = 0; i < m_bodies.size(); ++i)
    {
        target.draw(m_bodies[i], states);
    }
}

void ParticleSystem::reset_particle(std::size_t index)
{
    float angle = (std::rand() % 360) * 3.14f / 180.f;
    float speed = (std::rand() % 50) + 50.f;
    m_particles[index].velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
    m_particles[index].lifetime = sf::milliseconds((std::rand() % 2000) + 1000);

    m_bodies[index].setPosition(m_emitter);

    std::vector<sf::Color> colors = {sf::Color::Red, sf::Color::Green, sf::Color::Blue};
    m_bodies[index].setFillColor(colors[index % 3]);
}

void ParticleSystem::set_emitter(sf::Vector2f position)
{
    m_emitter = position;
}

void ParticleSystem::update(sf::Time elapsed)
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
        m_bodies[i].setPosition(m_bodies[i].getPosition() + (p.velocity * elapsed.asSeconds()));

        // update the alpha (transparency) of the particle according to its lifetime
        float ratio = p.lifetime.asSeconds() / m_lifetime.asSeconds();
        sf::Color c = m_bodies[i].getFillColor();
        c.a = static_cast<sf::Uint8>(ratio * 255);
        m_bodies[i].setFillColor(c);
    }
}