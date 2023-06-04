//	Title: Waterfall
//	By: Ioan-Lica Marin
//	@2024

//
//	ENTRY POINT
//

#include <iostream>
#include <Engine.h>
#include <Rnd/ORenderer.h>
#include <Rnd/OScript.h>
#include <Rnd/Entity.h>
#include <Rnd/Time.h>

#include <array>
#include <chrono>
#include <random>
#include <set>
#include <random>

#define _USE_MATH_DEFINES
#include <math.h>

#include <glm/glm.hpp>

double RandomDouble(double lowerBound, double upperBound) {
	std::default_random_engine randEng(rand());
	std::uniform_real_distribution<double> unifDist(lowerBound, upperBound);
	return unifDist(randEng);
}

class Particle {
public:
	Particle() {
		entity = new rnd::Entity();
		rnd::Transform particleTransform;
		particleTransform.scale = glm::vec3{ 0.1f };
		particleTransform.translate = pos;
		entity->SetModel("models/lpsphere.obj", glm::vec4{ 0.5f, 0.6f, 1.0f, 1.0f });
		entity->SetTransform(particleTransform);
	}

	~Particle() {
	}

	void Apply() {
		rnd::Transform particleTransform = entity->GetTransfrom();
		particleTransform.translate = pos;
		entity->SetTransform(particleTransform);
		glm::vec4 nonColor{ fabs((vel.x + vel.y + vel.z) / 3 + 0.5), fabs(vel.y), fabs(vel.z), 1.0f };
		entity->SetColor(glm::normalize(nonColor));
	}

	rnd::Entity* entity;

	glm::dvec3 pos{ 0.0 };
	glm::dvec3 vel{ 0.0 };

	double density = 0.0;
	double pressure = 0.0;
	double viscosity = 0.0;


};

class MainScript : rnd::OScript {
private:

	// Runs when script gets initialized inside the renderer
	void Start() {

		for (int i = 0; i < m_NumberOfParticles; i++) {
			Particle particle{};

			particle.pos.x = RandomDouble(m_XSpanS, m_XSpanE);
			particle.pos.y = RandomDouble(m_YSpanS, m_XSpanE);
			particle.pos.z = RandomDouble(m_ZSpanS, m_XSpanE);

			particle.vel.x = RandomDouble(-1.0, 1.0);
			particle.vel.y = RandomDouble(-1.0, 1.0);
			particle.vel.z = RandomDouble(-1.0, 1.0);

			particle.density = 0.0;
			particle.pressure = 0.0;
			particle.viscosity = 0.0;

			m_Particles.push_back(particle);
		}


		/*const double spacing = m_ParticleRadius * 10;
		const double volume = spacing * spacing * spacing;


		for (double x = spacing; x < m_SimulationWidth; x += spacing) {
			for (double y = spacing; y < m_SimulationHeight; y += spacing) {
				for (double z = spacing; z < m_SimulationDepth; z += spacing) {
					Particle particle{};
					particle.pos.x = x + RandomDouble(0, m_ParticleRadius / 2);
					particle.pos.y = y + RandomDouble(0, m_ParticleRadius / 2);
					particle.pos.z = z + RandomDouble(0, m_ParticleRadius / 2);

					particle.vel.x = RandomDouble(-1000, 1000);
					particle.vel.y = RandomDouble(-1000, 1000);
					particle.vel.z = RandomDouble(-1000, 1000);

					particle.density = 0.0;
					particle.pressure = 0.0;
					particle.viscosity = 0.0;

					m_Particles.push_back(particle);
				}
			}
		}*/


		/*// Initialize the vector field
		//m_VectorField.resize(m_VectorFieldExtent.x);
		//for (size_t i = 0; i < m_VectorFieldExtent.x; i++)
		//	m_VectorField[i].resize(m_VectorFieldExtent.y);

		// Initialize particles
		for (size_t i = 0; i < m_ParticleNumer; i++) {
			glm::vec3 position{ ((double)rand() / (RAND_MAX)) * 100.0f, ((double)rand() / (RAND_MAX)) * 100.0f , 10.0f };

			rnd::Entity* particle = new rnd::Entity();
			rnd::Transform particleTransform;
			particleTransform.scale = glm::vec3{ 0.1f };
			particleTransform.translate = position;
			particle->SetModel("models/cube.obj", glm::vec4{ 0.5f, 0.6f, 1.0f, 1.0f });
			particle->SetTransform(particleTransform);

			m_Velocity[particle] = glm::vec3{ 0.0f, 0.0f, 0.0f };
			m_Entities.push_back(particle);
		}*/
	}

	// Runs on every frame
	void Update() {
		// Apply external forces
		//for(size_t i = 0; i < m_Particles.size(); i++) {
		//	m_Particles[i].vel.z -= m_Gravity * rnd::Time::DeltaTime();
		//}

		// Apply collisions
		for (size_t i = 0; i < m_Particles.size(); i++) {
			Particle& current = m_Particles[i];
			for (size_t j = 0; j < m_Particles.size(); j++) {
				if (i == j)
					continue;

				Particle& neighbour = m_Particles[j];

				if (glm::length(current.pos - neighbour.pos) >= m_ParticleRadius)
					continue;

				glm::dvec3 normal = glm::normalize(current.pos - neighbour.pos);
				//if (isnan(normal.x) || isnan(normal.y) || isnan(normal.z))
				//	continue;

				glm::dvec3 relVel = current.vel - neighbour.vel;

				double normalVel = glm::dot(relVel, normal);

				if (normalVel > 0) {
					//if (neighbour.vel.z == 0)
					//	current.vel.z += m_Gravity * rnd::Time::DeltaTime();
					continue;
				}

				//current.pos = neighbour.pos * (normal * m_ParticleRadius);
				current.vel += -normalVel * normal;
				neighbour.vel -= -normalVel * normal;
				current.vel *= m_ParticleDamping;
				neighbour.vel *= m_ParticleDamping;
				
			}
		}

		for (size_t i = 0; i < m_Particles.size(); i++) {
			Particle& current = m_Particles[i];

			glm::dvec3 accel{ 0.0 };
			for (size_t j = 0; j < m_Particles.size(); j++) {
				if (i == j)
					continue;

				Particle& neighbour = m_Particles[j];

				if (glm::length(current.pos - neighbour.pos) >= m_ParticleRadius * 2 || glm::length(current.pos - neighbour.pos) >= m_ParticleRadius * 2)
					continue;

				glm::dvec3 acceldir = current.pos - neighbour.pos;
				double dist = glm::length(acceldir);

				accel += acceldir * std::pow(1.1, dist);
			}

			current.vel += accel * m_ParticleDamping;

		}

		// Density and pressure
		

		// Viscosity
		

		// Acceleration


		// Position Update
		for (size_t i = 0; i < m_Particles.size(); i++) {
			Particle& particle = m_Particles[i];

			particle.pos.x += particle.vel.x * rnd::Time::DeltaTime();
			particle.pos.y += particle.vel.y * rnd::Time::DeltaTime();
			particle.pos.z += particle.vel.z * rnd::Time::DeltaTime();

			//particle.vel = (fabs(glm::length(particle.vel)) > 1000) ? particle.vel * (1000 / fabs(glm::length(particle.vel))) : particle.vel;

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

			if (particle.pos.z <= 0) {
				particle.pos.z = 0;
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





		/*for (rnd::Entity* particle : m_Entities) {
			// Apply accelerations
			m_Velocity[particle] -= m_Gravity * rnd::Time::DeltaTime();

			// Apply tranformation
			glm::vec3 translation = particle->GetTransfrom().translate;
			translation += m_Velocity[particle];

			if (translation.z < -10.0f) {
				translation.z = -10.0f;
				m_Velocity[particle].z = 0;
			}

			particle->SetTranslation(translation);

		//	ComputeDensityPressure();

		//	ComputeForces();

		//	Integrate();

		//	ComputeCollisions();

		//	ApplyPosition();
		}

		// Simulation loop

		/// for all particles i do
		///		apply forces vi = vi + f_ext(xi) * deltatime
		///		predict position xi_predict = xi + vi * deltatime
		///
		/// for all particles i do
		///		find neighboring particles Ni(xi_predict)
		///
		/// while iter < solverIterations do
		///		for all particles i do
		///			calculate lambda_i
		///
		///		for all particles i do
		///			calculate deltap_i
		///			perform collision detection and response
		///
		///		for all particles i do
		///			update position xi_star = xi_star + deltap_i
		///
		///	for all particles i do
		///		update velocity vi = 1 / deltaTime * (xi_star - xi)
		///		apply vorticity confinement and xsph viscosity
		///		update position xi = xi_star
		///

		*/

	}

	const int m_NumberOfParticles = 1000;


	// Constants - Particles
	const double m_ParticleRadius = 0.5;
	const double m_ParticleMass = 18.0;
	const double m_ParticleRestDensity = 10.0;
	const double m_ParticleGasConstant = 461.5;
	const double m_ParticleViscosity = 0.01;
	const double m_ParticleStiffness = 3.0;
	const double m_ParticleDamping = 0.8;

	// Constants - Simulation
	const double m_SimulationWidth = 20.0;
	const double m_SimulationHeight = 20.0;
	const double m_SimulationDepth = 20.0;
	const double m_Gravity = 9.8;
	const double m_MaxSpeed = 350.0;

	// Constants - Spawn
	const double m_XSpanS = 5;
	const double m_XSpanE = 15;
	const double m_YSpanS = 5;
	const double m_YSpanE = 15;
	const double m_ZSpanS = 15;
	const double m_ZSpanE = 19;

	


	// Particle Vector
	std::vector<Particle> m_Particles;



	// Constants
	//const size_t m_ParticleNumer = 50000;
	//const glm::vec3 m_Gravity{ 0.0f, 0.0f, 9.8f };

	//glm::uvec2 m_VectorFieldExtent{ 100, 100 };


	//
	//std::vector<std::vector<glm::vec3>> m_VectorField;
	//std::vector<rnd::Entity*> m_Entities;
	//std::unordered_map<rnd::Entity*, glm::vec3> m_Velocity;

	// Parameters to be integrated into the UI


};

int main() {
	std::cout << "Version - a0.1\n";

	// The rnd::OScript constructor sets it up to be ran inside the renderer
	MainScript mainScript;

	// Renderer is created and execution is started
	rnd::ORenderer* renderer = rnd::ORenderer::GetInstance();
	int Err = renderer->Execute();

	return Err;
}