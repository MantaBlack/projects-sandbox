#include "ParticleEmitter.hpp"
#include <cmath>
#include <iostream>

void ParticleEmitter::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();

	states.texture = NULL;

	target.draw(m_vertices);

	/*const std::size_t total_vertices = m_num_triangles * 3 * m_particles.size();
	const std::size_t vertex_count = m_num_triangles * 3;

	for (size_t start_idx = 0; start_idx < total_vertices; start_idx += vertex_count)
	{
		target.draw(&m_vertices[start_idx], vertex_count, sf::Triangles);
	}*/

}

void ParticleEmitter::reset_particle(std::size_t index)
{
	// give random velocity and lifetime to the particle
	float angle = (std::rand() % 360) * 3.14f / 180.f;
	float speed = (std::rand() % 50) + 50.f;
	m_particles[index].velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
	m_particles[index].lifetime = sf::milliseconds((std::rand() % 2000) + 2000);

	// reset the position of the corresponding vertex
	m_particles[index].center = m_emitter;

	// assign a random color to the particle
	std::vector<sf::Color> colors = { sf::Color::Red, sf::Color::Green, sf::Color::Blue };
	m_particles[index].color = sf::Color(std::rand() % 255, std::rand() % 255, std::rand() % 255);

	// calculate vertices for this particle
	draw_particle(index);
}

void ParticleEmitter::draw_particle(std::size_t index)
{
	const float pi                   = 22.f / 7.f;
	const float two_pi               = 2.f * pi;
	const std::size_t total_vertices = m_num_triangles * 3;
	const float sector_angle         = two_pi / m_num_triangles;

	//sf::VertexArray new_vertices(sf::Triangles);
	Particle p = m_particles[index];

	std::size_t start_idx = index * total_vertices;
	std::size_t end_idx = start_idx + total_vertices;
	std::size_t idx = start_idx;

	for (float theta = 0.f; idx < end_idx; theta += sector_angle)
	{
		sf::Vector2f vertex(std::cos(theta) * m_radius, std::sin(theta) * m_radius);

		//std::size_t vertex_count = new_vertices.getVertexCount();

		if (idx == start_idx)
		{
			//new_vertices.append(sf::Vertex(p.center, p.color));
			m_vertices[idx] = sf::Vertex(p.center, p.color);
			++idx;
		}
		else if (idx < start_idx + 3)
		{
			//new_vertices.append(sf::Vertex(p.center + vertex, p.color));
			m_vertices[idx] = sf::Vertex(p.center + vertex, p.color);
			++idx;
		}
		else
		{
			/*new_vertices.append(sf::Vertex(new_vertices[vertex_count - 1].position, p.color));
			new_vertices.append(sf::Vertex(p.center, p.color));
			new_vertices.append(sf::Vertex(p.center + vertex, p.color));*/

			m_vertices[idx] = sf::Vertex(m_vertices[idx - 1].position, p.color);
			m_vertices[idx] = sf::Vertex(p.center, p.color);
			m_vertices[idx] = sf::Vertex(p.center + vertex, p.color);

			idx += 3;
		}
	}
}

void ParticleEmitter::move_particle(std::size_t index, sf::Time elapsed_time)
{
	const std::size_t total_vertices = m_num_triangles * 3;
	Particle p = m_particles[index];

	std::size_t start_idx = index * total_vertices;
	std::size_t end_idx = start_idx + total_vertices;

	float ratio = p.lifetime.asSeconds() / m_lifetime.asSeconds();

	for (size_t i = start_idx; i < end_idx; ++i)
	{
		// update position
		m_vertices[i].position += p.velocity * elapsed_time.asSeconds();

		// update transparency
		m_vertices[i].color.a = static_cast<sf::Uint8>(ratio * 255);
	}
}

void ParticleEmitter::set_emitter(sf::Vector2f position)
{
	m_emitter = position;
}

void ParticleEmitter::update(sf::Time elapsed)
{
	for (std::size_t i = 0; i < m_particles.size(); ++i)
	{
		Particle& p = m_particles[i];

		p.lifetime -= elapsed;

		if (p.lifetime <= sf::Time::Zero)
		{
			reset_particle(i);
		}

		move_particle(i, elapsed);
	}
}
