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
	glm::vec4* positions = nullptr;
	glm::vec4* colors = nullptr;
	glm::vec4* sizes = nullptr;
	unsigned char* gfxBuffer = nullptr;
	int gfxBufferSize = 0;

	Config config;

	gfx::ResourceManager::HVAO vaoHandle = 0;
	gfx::ResourceManager::HBUFFER storageBufferHandle = 0;
	gfx::ResourceManager::HPROGRAM programHandle = 0;
};
