#include "Simulation.h"

#include <Rnd/ORenderer.h>
#include <Rnd/Time.h>
#include <Rnd/UIHelper.h>

double RandomDouble(double lowerBound, double upperBound) {
	std::default_random_engine randEng(rand());
	std::uniform_real_distribution<double> unifDist(lowerBound, upperBound);
	return unifDist(randEng);
}


void Simulation::Start() {
	int xVel, yVel, zVel;
	rnd::UIHelper::ReadSimulationStartParams(xVel, yVel, zVel, m_NumberOfParticles);

	for (int i = 0; i < m_NumberOfParticles; i++) {
		Particle particle{};

		particle.pos.x = RandomDouble(m_XSpanS, m_XSpanE);
		particle.pos.y = RandomDouble(m_YSpanS, m_XSpanE);
		particle.pos.z = RandomDouble(m_ZSpanS, m_XSpanE);

		particle.vel.x = RandomDouble(xVel - 1.0, xVel + 1.0);
		particle.vel.y = RandomDouble(yVel - 1.0, yVel + 1.0);
		particle.vel.z = RandomDouble(zVel - 1.0, zVel + 1.0);

		particle.density = 0.0;
		particle.pressure = 0.0;
		particle.viscosity = 0.0;

		m_Particles.push_back(particle);
	}
}

void Simulation::Update() {

	bool reset = false;
	bool gravity = true;
	bool collisions = true;
	rnd::UIHelper::ReadSimulationData(reset, gravity, collisions, m_ParticleViscosity, m_ParticleRestDensity, m_ParticleDamping, m_ParticleStiffness);

	if (reset) {
		for (auto& elem : m_Particles) {
			delete elem.entity;
		}
		m_Particles.clear();
		Start();
	}

	// Apply collisions
	for (size_t i = 0; i < m_Particles.size() && collisions; i++) {
		Particle& current = m_Particles[i];

		for (size_t j = 0; j < m_Particles.size(); j++) {
			if (i == j)
				continue;

			Particle& neighbour = m_Particles[j];

			if (glm::length(current.pos - neighbour.pos) >= m_ParticleRadius)
				continue;

			glm::dvec3 normal = glm::normalize(current.pos - neighbour.pos);

			glm::dvec3 relVel = current.vel - neighbour.vel;

			double normalVel = glm::dot(relVel, normal);

			if (normalVel > 0)
				continue;

			current.vel += -normalVel * normal;
			neighbour.vel -= -normalVel * normal;
		}
	}

	// Calculate density
	for (size_t i = 0; i < m_Particles.size(); i++) {
		Particle& current     = m_Particles[i];
		double	  totalVolume = 0.0;

		for (size_t j = 0; j < m_Particles.size(); j++) {
			if (i == j)
				continue;

			Particle& neighbor = m_Particles[j];
			double distance = glm::distance(current.pos, neighbor.pos);

			if (distance >= 2 * m_ParticleRadius)
				continue;

			double h = (2 * m_ParticleRadius) - static_cast<double>(distance);
			double overlapVolume = M_PI * (h * h * ((2.0 * m_ParticleRadius) - h) / 3.0);
			totalVolume += overlapVolume;
		}

		double particleVolume = (4.0 / 3.0) * M_PI * (3 * m_ParticleRadius) + totalVolume;

		current.density = 3 / particleVolume;
	}

	for (size_t i = 0; i < m_Particles.size(); i++) {
		Particle& current = m_Particles[i];

		glm::dvec3 pressureForce{ 0.0 };

		for (size_t j = 0; j < m_Particles.size(); j++) {
			if (i == j)
				continue;
			Particle& neighbor = m_Particles[j];


			const glm::dvec3 velocityDiff = neighbor.vel - current.vel;
			const double distance = glm::distance(current.pos, neighbor.pos);

			if (distance >= m_ParticleRadius * 2.0)
				continue;
			
			const glm::dvec3 force = m_ParticleViscosity * rnd::Time::SimulationDeltaTime() * m_ParticleDamping * velocityDiff / (distance * distance);
			current.vel += force;
			neighbor.vel -= force;


			glm::dvec3 direction = neighbor.pos - current.pos;
			const double densityDiff = current.density + neighbor.density - 2.0 * m_ParticleRestDensity;
			const double pressure = m_ParticleStiffness * densityDiff;

			pressureForce += pressure * rnd::Time::SimulationDeltaTime() * m_ParticleDamping * (direction / (distance * distance));
		}

		current.vel += pressureForce;
	}

	// Position Update
	for (size_t i = 0; i < m_Particles.size(); i++) {
		Particle& particle = m_Particles[i];

		if (gravity)
			particle.vel.z -= m_Gravity * rnd::Time::SimulationDeltaTime();

		particle.pos += particle.vel * static_cast<double>(rnd::Time::SimulationDeltaTime());

		if (particle.pos.x <= 0) {
			particle.pos.x = 0;
			particle.vel.x = -particle.vel.x;
			particle.vel *= m_ParticleDamping;
		}

		if (particle.pos.y <= 0) {
			particle.pos.y = 0;
			particle.vel.y = -particle.vel.y;
			particle.vel *= m_ParticleDamping;
		}

		double rnd = 0;// RandomDouble(0.0, 0.1);
		if (particle.pos.z <= 0 + rnd) {
			particle.pos.z = 0 + rnd;
			particle.vel.z = -particle.vel.z;
			particle.vel *= m_ParticleDamping;
		}

		if (particle.pos.x >= m_SimulationWidth) {
			particle.pos.x = m_SimulationWidth;
			particle.vel.x = -particle.vel.x;
			particle.vel *= m_ParticleDamping;
		}

		if (particle.pos.y >= m_SimulationDepth) {
			particle.pos.y = m_SimulationDepth;
			particle.vel.y = -particle.vel.y;
			particle.vel *= m_ParticleDamping;
		}

		if (particle.pos.z >= m_SimulationHeight) {
			particle.pos.z = m_SimulationHeight;
			particle.vel.z = -particle.vel.z;
			particle.vel *= m_ParticleDamping;
		}

		particle.Apply();
	}
}