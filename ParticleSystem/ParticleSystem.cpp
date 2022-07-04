#include "ParticleSystem.h"

#include <glm/ext.hpp>
#include "DrawCall.h"
#include "ResourceManager.h"
#include "Utils.h"

#define VERTS_PER_PARTICLE 6

// Linear intERPolation
float lerp(float start, float end, float t) {
    return start + t * (end - start);
}

float randomFloat(float min, float max) {
    float t = (float)(rand() % 10000) / 10000.f; // gives us a value between 0.0 and 1.0
    return lerp(min, max, t);
}

ParticleSystem::ParticleSystem(const ParticleSystem::Config& config): config(config) {
	positions = new glm::vec4[config.maxParticles];
	colors = new glm::vec4[config.maxParticles];
	sizes = new glm::vec4[config.maxParticles];

    // TODO: Get rid of this extra buffer by having the ResourceManager
    // expose the memory mapping via lambda or similar.
    gfxBufferSize = sizeof(glm::vec4) * 3 * config.maxParticles;
    gfxBuffer = new unsigned char[gfxBufferSize];
}

ParticleSystem::~ParticleSystem() {
    if (positions != nullptr) {
        delete[] positions;
    }

    if (colors != nullptr) {
        delete[] colors;
    }

    if (sizes != nullptr) {
        delete[] sizes;
    }

    if (gfxBuffer != nullptr) {
        delete[] gfxBuffer;
    }
}

void ParticleSystem::initGraphicsResources(gfx::ResourceManager& resourceManager) {
	const int numIndices = config.maxParticles * VERTS_PER_PARTICLE;
	unsigned int* indices = new unsigned int[numIndices];

	for (int i = 0; i < numIndices; i++) {
		indices[i] = i;
	}

	gfx::ResourceManager::VAOConfig vaoConfig;
	vaoConfig.indexBufferSizeBytes = sizeof(unsigned int) * numIndices;
	vaoConfig.indexData = indices;
	
	vaoHandle = resourceManager.createVAO(vaoConfig);

	int storageDataSize = sizeof(glm::vec4) * 3 * config.maxParticles;
	storageBufferHandle = resourceManager.createStreamingStorageBuffer(storageDataSize, nullptr);

    std::string vertShaderSource = loadAsciiFile("triangle.vert");
    std::string fragShaderSource = loadAsciiFile("triangle.frag");
    gfx::ResourceManager::ShaderSource vertexShaderSource = {
        gfx::ResourceManager::ShaderType::VERTEX_SHADER,
        vertShaderSource.c_str()
    };
    gfx::ResourceManager::ShaderSource fragmentShaderSource = {
        gfx::ResourceManager::ShaderType::FRAGMENT_SHADER,
        fragShaderSource.c_str()
    };
    gfx::ResourceManager::ShaderSource shaders[2] = { vertexShaderSource, fragmentShaderSource };

    programHandle = resourceManager.createProgramFromSource(shaders, 2);
}

void ParticleSystem::update(double deltaT) {
    const float MAX_EXPLOSION_RADIUS = 10.0f;

    static bool initialized = false;
    if (!initialized) {
        for (int i = 0; i < config.maxParticles; i++) {
            float theta = randomFloat(0, glm::pi<float>());
            float phi = randomFloat(0, 2.0 * glm::pi<float>());
            float t = randomFloat(0.0, 1.0);
            t = t * t * t * t; // makes the falloff more curvy
            float radius = t * MAX_EXPLOSION_RADIUS;

            float x = radius * cos(phi) * sin(theta);
            float y = radius * cos(theta);
            float z = radius * sin(phi) * sin(theta);

            float size = lerp(1.0, 0.2, t) + randomFloat(0.0f, 0.1f);

            float r = 0.9f + randomFloat(0.0f, 0.1f);
            float g = lerp(0.9, 0.4, t) + randomFloat(0.0f, 0.1f);
            float b = randomFloat(0.0f, 0.2f);

            positions[i] = glm::vec4(x, y, z, 1);
            colors[i] = glm::vec4(r, g, b, 1);
            sizes[i] = glm::vec4(size, 100, 100, 100);
        }
        initialized = true;
    }
}

void ParticleSystem::getDrawCalls(gfx::ResourceManager& resourceManager, std::vector<gfx::DrawCall>& drawCalls) {
    // Copy particle properties to CPU gfxBuffer;
    int sizePerElement = sizeof(glm::vec4) * config.maxParticles;
    memcpy_s(gfxBuffer, gfxBufferSize, positions, sizePerElement);
    memcpy_s(gfxBuffer+sizePerElement, gfxBufferSize-sizePerElement, colors, sizePerElement);
    memcpy_s(gfxBuffer+2* sizePerElement, gfxBufferSize-2*sizePerElement, sizes, sizePerElement);

    resourceManager.streamDataToStorageBuffer(storageBufferHandle, gfxBufferSize, gfxBuffer);

    gfx::DrawCall call;
    call.mode = gfx::DrawCall::Mode::TRIANGLES;
    call.numIndices = config.maxParticles * VERTS_PER_PARTICLE;
    call.indexType = gfx::DrawCall::IndexType::UINT;
    call.indices = nullptr;
    call.programHandle = programHandle;
    call.storageBuffer = storageBufferHandle;
    call.vaoHandle = vaoHandle;
    call.storageBufferBaseIndex = 0;

    drawCalls.push_back(call);
}
