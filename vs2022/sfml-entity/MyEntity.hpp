#include <SFML/Graphics.hpp>
#include <vector>

class MyEntity : public sf::Drawable, public sf::Transformable
{
private:
    virtual void draw(sf::RenderTarget& target,
        sf::RenderStates states) const;

    struct Particle
    {
        sf::Vector2f velocity;
        sf::Time     lifetime;
    };

    void reset_particle(std::size_t index);

    std::vector<Particle> m_particles;
    sf::VertexArray       m_vertices;
    sf::Time              m_lifetime;
    sf::Vector2f          m_emitter;

    float m_radius = 5.f;
    std::vector<sf::CircleShape> m_circles;

public:
    MyEntity(unsigned int count)
        : m_particles(count),
        m_vertices(sf::Points, count),
        m_lifetime(sf::seconds(3.f)),
        m_emitter(0.f, 0.f),
        m_circles(count, sf::CircleShape(m_radius))
    {}

    void set_emitter(sf::Vector2f position);

    void update(sf::Time elapsed);

};