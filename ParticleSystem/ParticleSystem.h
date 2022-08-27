#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "DrawCall.h"

namespace gfx {
	class ResourceManager;
}

class ParticleSystem {
public:
	struct Config {
		unsigned int maxParticles;
	};

	ParticleSystem(const Config& config);
	~ParticleSystem();

	void initGraphicsResources(gfx::ResourceManager& resourceManager);

	void update(double deltaT);
	void getDrawCalls(gfx::ResourceManager& resourceManager, std::vector<gfx::DrawCall>& drawCalls);

private:

	struct Particle {
		glm::vec4 position;
		glm::vec3 velocity;
		glm::vec4 color;
		float size;
		float lifetime;
		float maxLife;
	};

	struct HopEmitter {
		glm::vec3 worldPos;
		float circleRadius;
		int numHops;
		float hopHeight;
		float horizontalSpeed;
		double lifetime;

		float offsetRadius;
		float drag;

		glm::vec4 particleStartColor;
		glm::vec4 particleMidColor;
		glm::vec4 particleEndColor;

		float particleStartSize;
		float particleEndSize;
		int particlesPerSecond;

		float particleMinLifetime;
		float particleMaxLifetime;
	};

	Particle* particles = nullptr;
	int numActiveParticles = 0;

	Config config;
	HopEmitter emitter;

	gfx::ResourceManager::HVAO vaoHandle = 0;
	gfx::ResourceManager::HBUFFER storageBufferHandle = 0;
	gfx::ResourceManager::HPROGRAM programHandle = 0;

	// Updates a single particle.
	inline void updateParticle(Particle& particle, double deltaT);
};
