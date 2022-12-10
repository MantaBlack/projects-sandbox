#include <SFML/Graphics.hpp>
#include <vector>

class ParticleSystem : public sf::Drawable, public sf::Transformable
{
private:
    virtual void draw(sf::RenderTarget& target,
                      sf::RenderStates states) const;

private:
    struct Particle
    {
        sf::Vector2f velocity;
        sf::Time     lifetime;
    };

    void reset_particle(std::size_t index);

    std::vector<Particle>        m_particles;
    std::vector<sf::CircleShape> m_bodies;
    sf::Time                     m_lifetime;
    sf::Vector2f                 m_emitter;

public:
    ParticleSystem(unsigned int count)
    : m_particles(count),
      m_bodies(std::vector<sf::CircleShape>(count, sf::CircleShape(5.f, 15))),
      m_lifetime(sf::seconds(3.f)),
      m_emitter(0.f, 0.f)
    {}

    void set_emitter(sf::Vector2f position);

    void update(sf::Time elapsed);
};