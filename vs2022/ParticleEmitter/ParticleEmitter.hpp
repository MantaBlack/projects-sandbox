#include <SFML/Graphics.hpp>

#include <vector>

class ParticleEmitter : public sf::Drawable, public sf::Transformable
{
private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	struct Particle
	{
		sf::Vector2f center;
		sf::Vector2f velocity;
		sf::Time     lifetime;
		sf::Color    color;
	};

	void reset_particle(std::size_t index);
	void draw_particle(std::size_t index);
	void move_particle(std::size_t index, sf::Time elapsed_time);

	std::vector<Particle> m_particles;
	sf::VertexArray       m_vertices;
	sf::Time              m_lifetime;
	sf::Vector2f          m_emitter;

	float m_radius;
	std::size_t m_num_triangles;

public:
	ParticleEmitter(std::size_t num_particles,
		float lifetime,
		float radius,
		std::size_t num_triangles)
		: m_particles(num_particles),
		m_vertices(sf::Triangles, num_triangles * 3 * num_particles),
		m_lifetime(sf::seconds(lifetime)),
		m_radius(radius),
		m_num_triangles(num_triangles)
	{}

	void set_emitter(sf::Vector2f position);

	void update(sf::Time elapsed);
};