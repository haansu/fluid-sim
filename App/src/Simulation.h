#pragma once

#include <Rnd/OScript.h>
#include <Rnd/Entity.h>

#include <array>
#include <chrono>
#include <random>
#include <set>
#include <random>

#define _USE_MATH_DEFINES
#include <math.h>

#include <glm/glm.hpp>

class Particle {
public:
	Particle() {
		entity = new rnd::Entity();
		rnd::Transform particleTransform;
		particleTransform.scale = glm::vec3{ 0.2f };
		particleTransform.translate = pos;
		entity->SetModel("models/lpsphere.obj", glm::vec4{ 0.5f, 0.6f, 1.0f, 1.0f });
		entity->SetTransform(particleTransform);
	}

	~Particle() { }

	void Apply() {
		rnd::Transform particleTransform = entity->GetTransfrom();
		particleTransform.translate = pos;
		entity->SetTransform(particleTransform);
		glm::vec4 nonColor{ fabs((vel.x + vel.y + vel.z) / 3 + 0.5), fabs(4 * density), fabs(0.5f), 1.0f };
		entity->SetColor(glm::normalize(nonColor));
	}

	rnd::Entity* entity;

	glm::dvec3 pos{ 0.0 };
	glm::dvec3 vel{ 0.0 };

	double density = 0.0;
	double pressure = 0.0;
	double viscosity = 0.0;


};

class Simulation : rnd::OScript {
private:

	// Runs when script gets initialized inside the renderer
	void Start();

	// Runs on every frame
	void Update();

	int m_NumberOfParticles = 1000;


	// Constants - Particles
	const double m_ParticleRadius = 1.0;
	//const double m_ParticleMass = 18.0;
	//const double m_ParticleGasConstant = 461.5;
	double m_ParticleRestDensity = 5.0;
	double m_ParticleViscosity = 0.1;
	double m_ParticleStiffness = 3;
	double m_ParticleDamping = 0.98;

	// Constants - Simulation
	const double m_SimulationWidth = 20.0;
	const double m_SimulationHeight = 20.0;
	const double m_SimulationDepth = 20.0;
	const double m_Gravity = 9.8;
	const double m_MaxSpeed = 350.0;

	// Constants - Spawn
	const double m_XSpanS = 2;
	const double m_XSpanE = 18;
	const double m_YSpanS = 2;
	const double m_YSpanE = 18;
	const double m_ZSpanS = 10;
	const double m_ZSpanE = 18;

	// Particle Vector
	std::vector<Particle> m_Particles;
};