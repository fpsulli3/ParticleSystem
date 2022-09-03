#include "ParticleSystem.h"

#include <glm/ext.hpp>
#include "DrawCall.h"
#include "ResourceManager.h"
#include "Utils.h"

#define VERTS_PER_PARTICLE 4 // the particles will be square, these are the 4 corners
#define INDICES_PER_PARTICLE 6 
#define NUM_SHADER_PROPERTIES 3 // 3 properties: position, color, and size

// Linear intERPolation
template <typename T>
T lerp(T start, T end, float t) {
    return start + t * (end - start);
}

template <typename T>
float lerp(T start, T mid, T end, float t) {
    return t < 0.5 
        ? lerp(start, mid, t / 0.5f) 
        : lerp(mid, end, (t - 0.5f) / 0.5f);
}

float randomFloat(float min, float max) {
    float t = (float)(rand() % 10000) / 10000.f; // gives us a value between 0.0 and 1.0
    return lerp(min, max, t);
}

ParticleSystem::ParticleSystem(const ParticleSystem::Config& config): config(config) {
	particles = new Particle[config.maxParticles];
    memset(particles, 0, sizeof(Particle) * config.maxParticles);

    emitter.circleRadius = 20.0f;
    emitter.numHops = 8;
    emitter.hopHeight = 3.0f;
    emitter.horizontalSpeed = 25.0f;
    emitter.lifetime;
    emitter.offsetRadius = 1;
    emitter.drag = 0.9;
    emitter.worldPos = glm::vec4(0, 0, -10, 1);
    emitter.particleMinLifetime = 2.7;
    emitter.particleMinLifetime = 3.0;
    emitter.particlesPerSecond = 30000;
    emitter.particleStartSize = 0.1;
    emitter.particleEndSize = 0.1;
    emitter.particleStartColor = glm::vec4(1.0, 1.0, 0.1, 1.0);
    emitter.particleMidColor   = glm::vec4(1.0, 0.1, 0.1, 1.0);
    emitter.particleEndColor   = glm::vec4(0.2, 0.1, 0.2, 1.0);
}

ParticleSystem::~ParticleSystem() {
    if (particles != nullptr) {
        delete[] particles;
        particles = nullptr;
    }
}

void ParticleSystem::initGraphicsResources(gfx::ResourceManager& resourceManager) {
    // First, we're going to create our Shader pipeline, also known as a *Program*
    // This Program will contain two shaders:
    // - Vertex Shader: This is a small program that processes each vertex. At a 
    //   bare minimum, the output of the vertex shader needs to tell the graphics 
    //   pipeline WHERE on the screen the vertex is to be drawn. However, a vertex 
    //   shader can also output other properties of the vertex, such as the vertex 
    //   color. These properties are then passed to the Fragment Shader.
    // - Fragment Shader: Also known sometimes as a 'Pixel Shader'. Suppose you 
    //   draw a triangle with 3 verts. The vertex shader will run 3 times (once 
    //   for each vertex), and then the graphics pipeline will calculate which 
    //   pixels are covered by that triangle. Then, for each of those pixels, it 
    //   will run your fragment shader. So if the triangle covers 800 pixels (say), 
    //   then your fragment shader will be invoked 800 times! The properties that 
    //   were output from the vertex shader will serve as inputs to the fragment 
    //   shader. But what if each vertex in the triangle has a different color?
    //   In that case, the color is smoothly blended across the face of the triangle.
    //   The same is true for all properties, e.g. texture coordinates, normals, etc.
    // Once we compile each shader, we "link" them together into a single program.

    std::string vertShaderSource = loadAsciiFile("particle.vert"); // load from file
    std::string fragShaderSource = loadAsciiFile("particle.frag"); // load from file
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

    // Next, we're going to allocate a big block of memory on the GPU to store the 
    // particle data. For this, we have two main options:
    //   
    // 1. Create a Vertex Buffer Object (VBO). This is a special-purpose GPU memory 
    //    buffer that is meant to hold an array of vertex data. The graphics pipeline
    //    will then feed this vertex data to our Vertex Shader, one vertex at a time.
    //    Since each particle is a square, we'll need 4 vertices for each particle.
    // 2. Create a Shader Storage Buffer Object (SSBO). This is a more general-purpose
    //    block of GPU memory that our shader can access. We can store whatever type 
    //    of data we want in it. Also, our shader can access the entire thing (not 
    //    just one element at a time). So, it's a lot more flexible. This allows for 
    //    some interesting optimizations. For example, we can store particle colors on 
    //    a per-particle basis instead of a per-vertex basis. Since there are 4X as 
    //    many verts as particles, that is a significant savings. We can also store 
    //    the center-point position of each particle in the SSBO, rather than storing 
    //    the position of each vertex in a VBO. More savings! We'll need to calculate 
    //    the individual vertex positions in the vertex shader. However, this is also 
    //    a win because calculating those on the GPU is much faster than on the CPU. 
    //    The only down side is that we'll need some extra information (namely the 
    //    particle size) in order to calculate those positions in the shader. This 
    //    will cost some extra memory, but much less than what we're saving.
    // 
    // So let's us an SSBO!

    // Each particle has 3 properties (position, color, and size) and we'll need 16 
    // bytes for each property. We could get away with just a float (4 bytes) for the 
    // size, but the memory layout we're using requires 16-byte alignment for each 
    // property.
    int storageDataSize = sizeof(glm::vec4) * NUM_SHADER_PROPERTIES * config.maxParticles;

    // We're creating a "streaming" SSBO because we want to be able to update the data 
    // every frame as the properties of the particles change.
    storageBufferHandle = resourceManager.createStreamingStorageBuffer(storageDataSize, nullptr);

    // Next, we need to create an index buffer. The index buffer is just an array of integers 
    // that tells the vertex shader which vertices to draw, and in what order. So pretend that 
    // we have an array of vertices [a,b,c,d]. If we create an index buffer with [0,1,2,0,2,3],
    // then that will tell the graphics pipeline to draw the vertices in this order: a,b,c,a,c,d
    // So even though we have 4 vertices, we're drawing 6 with a couple repeats. The vertex 
    // shader will therefore be invoked 6 times, one for each index.

    // Each particle is a square. A square is comprised of two triangles (we like to draw 
    // almost everything as triangles). Each triangle requires 3 indices (one for each 
    // corner). Therefore, we need 6 indices per particle.
    const int numIndices = config.maxParticles * INDICES_PER_PARTICLE;

    // Initially, we're just creating a temporary index buffer on the CPU side using 
    // regular system memory. We'll fill this out, then transfer it to GPU memory.
	unsigned int* indices = new unsigned int[numIndices];

	for (int i = 0; i < config.maxParticles; i++) {
        unsigned int vertIndex0 = i * VERTS_PER_PARTICLE;
        unsigned int vertIndex1 = vertIndex0 + 1;
        unsigned int vertIndex2 = vertIndex0 + 2;
        unsigned int vertIndex3 = vertIndex0 + 3;

        int start = i * INDICES_PER_PARTICLE;
		indices[start]   = vertIndex0;
        indices[start+1] = vertIndex1;
        indices[start+2] = vertIndex2;
        indices[start+3] = vertIndex0;
        indices[start+4] = vertIndex2;
        indices[start+5] = vertIndex3;
	}

    // Next, we're going to set up a Vertex Array Object (VAO). This is an object that 
    // combines any VBOs and index buffers into one cohesive whole. It also allows us 
    // to specify how the VBO memory is laid out (where did we put the positions? where 
    // did we put the colors?). Since we're using an SSBO instead of a VBO, we can actually 
    // skip all that crap and just specify the index buffer. Another benefit of SSBOs!
	gfx::ResourceManager::VAOConfig vaoConfig;
	vaoConfig.indexBufferSizeBytes = sizeof(unsigned int) * numIndices;
	vaoConfig.indexData = indices;

    // createVAO will create the VAO and actually transfer the index buffer to the GPU
	vaoHandle = resourceManager.createVAO(vaoConfig);    
}

// A method for updating a single particle.
inline void ParticleSystem::updateParticle(Particle& particle, double deltaT) {
    const float g = -9.8; // gravity (acceleration)
    float deltaVy = g * deltaT; // change in velocity due to gravity for this frame

    // Update the velocity
    particle.velocity.y += deltaVy; // gravity
    particle.velocity *= 1.0f - (emitter.drag * deltaT);

    // Update the position
    particle.position.x += particle.velocity.x * deltaT;
    particle.position.y += particle.velocity.y * deltaT;
    particle.position.z += particle.velocity.z * deltaT;

    // Do a little collision detect with the floor.
    if (particle.position.y < 0) {
        // Flip the particle about the xz plane.
        particle.position.y = -particle.position.y;
        // Flip the y-velocity too so the particle goes upward.
        particle.velocity.y = -particle.velocity.y;
    }

    // What percentage of the particle's lifetime has it lived?
    float t = particle.lifetime / particle.maxLife;

    // Interpolate the colors
    particle.color.r = lerp(emitter.particleStartColor.r, emitter.particleMidColor.r, emitter.particleEndColor.r, t);
    particle.color.g = lerp(emitter.particleStartColor.g, emitter.particleMidColor.g, emitter.particleEndColor.g, t);
    particle.color.b = lerp(emitter.particleStartColor.b, emitter.particleMidColor.b, emitter.particleEndColor.b, t);

    // Interpolate the size
    particle.size = lerp(emitter.particleStartSize, emitter.particleEndSize, t);
}

// Updates the entire particle system
void ParticleSystem::update(double deltaT) {
    // Update any existing particles that are still alive
    const float g = -9.8; // gravity (acceleration)
    float deltaVy = g * deltaT; // change in velocity due to gravity for this frame
    int activeParticleCount = 0; 
    for (int i = 0; i < config.maxParticles; ++i) {
        Particle& particle = particles[i];
        particle.lifetime += deltaT;
        if (particle.lifetime < particle.maxLife) {
            ++activeParticleCount;
            updateParticle(particle, deltaT);
        }
    }

    // Emit new particles

    // Word of caution here. We are potentially going to 
    // emit several dozen particles each frame.

    // Technically, a frame represents a range of time that is 
    // between t0 and t1=t0+deltaT. We need our particles to 
    // look as though they've been emitted at various times 
    // during that range.

    // We do this by choosing a random time during that range 
    // and updating the particle as though it has been alive 
    // for that long.

    // If we don't do this, the eye will be able to perceive 
    // that the particles are being emitted in batches instead 
    // of continuously.

    int numParticlesToEmit = emitter.particlesPerSecond * deltaT;
    int availableNewParticles = config.maxParticles - activeParticleCount;

    if (numParticlesToEmit > availableNewParticles) {
        numParticlesToEmit = availableNewParticles;
    }

    // Calculate the emitter location
    // The following overly-complicated junk is just to make 
    // the emitter seem like it's hopping around in a circle.
    float radiansPerHop = 2 * glm::pi<float>() / emitter.numHops;
    float hopHalfLength = emitter.circleRadius * sin(radiansPerHop * 0.5f);
    float hopLength = hopHalfLength * 2;
    float totalPerimeter = hopLength * emitter.numHops;

    emitter.lifetime += deltaT;
    float totalHorizontalDistanceTraveled = emitter.horizontalSpeed * emitter.lifetime;

    int hopsTraveled = totalHorizontalDistanceTraveled / hopLength;
    float distanceTraveledThisHop = totalHorizontalDistanceTraveled - (hopsTraveled * hopLength);
    float t = distanceTraveledThisHop / hopLength;

    hopsTraveled %= emitter.numHops;

    float beforeHopRadians = radiansPerHop * hopsTraveled;
    glm::vec3 beforeHopPosition;
    beforeHopPosition.x = emitter.circleRadius * cos(beforeHopRadians);
    beforeHopPosition.y = 0;
    beforeHopPosition.z = emitter.circleRadius * sin(beforeHopRadians);

    float nextHop = (hopsTraveled + 1) % emitter.numHops;
    float nextHopRadians = radiansPerHop * nextHop;
    glm::vec3 nextHopPosition;
    nextHopPosition.x = emitter.circleRadius * cos(nextHopRadians);
    nextHopPosition.y = 0;
    nextHopPosition.z = emitter.circleRadius * sin(nextHopRadians);

    // This will initially just have the horizontal (x and z) axes
    // and we'll calculate the height (y axies) separately.
    glm::vec3 emitterPosition = lerp(beforeHopPosition, nextHopPosition, t);

    // Now calculate the y axis.
    float h = emitter.hopHeight;
    emitterPosition.y = 4*h*t - 4*h*t*t;

    int particleIndex = 0;
    float s = pow((1 - (4 * t - 4 * t * t)), 4);
    bool justUseTangential = (1 - s) < FLT_EPSILON;

    float angle = atan2(emitterPosition.x, emitterPosition.z);
    glm::vec3 normalizedEmitterPos = glm::normalize(emitterPosition);
    glm::vec3 tangentialVelocity(0,0,0);
    tangentialVelocity.x = -normalizedEmitterPos.z * emitter.horizontalSpeed;
    tangentialVelocity.z = normalizedEmitterPos.x * emitter.horizontalSpeed;


    glm::vec3 emitterVelocity = tangentialVelocity;
    if (!justUseTangential) {
        glm::vec3 segmentVelocity = glm::normalize(nextHopPosition - beforeHopPosition) * emitter.horizontalSpeed;
        emitterVelocity = lerp(segmentVelocity, tangentialVelocity, s);
    }

    emitterVelocity.y = 4*h - 8*h*t;

    for (int i = 0; i < numParticlesToEmit; ++i) {
        // Find an unused particle
        Particle* particle = nullptr;
        for (; particleIndex < config.maxParticles; ++particleIndex) {
            if (particles[particleIndex].lifetime >= particles[particleIndex].maxLife) {
                particle = &particles[particleIndex];
                break;
            }
        }

        if (particle == nullptr) {
            // We've run out of room for new particles.
            // This shouldn't ever happen because we were careful to make 
            // sure that the number of new particles to emit is not greater 
            // that the number of available particles. However, it is always 
            // good to double-check when it comes to null pointers.
            break;
        }

        // When during this frame did the particle emit?
        float dT = randomFloat(0, deltaT);
                
        float offsetRadius = randomFloat(0, emitter.offsetRadius);
        float offsetTheta = randomFloat(0, glm::pi<float>());
        float offsetPhi = randomFloat(-glm::pi<float>(), glm::pi<float>());

        float offsetX = cos(offsetPhi) * sin(offsetTheta) * offsetRadius;
        float offsetY = cos(offsetTheta) * offsetRadius;
        float offsetZ = sin(offsetPhi) * sin(offsetTheta) * offsetRadius;
        
        particle->position.x = emitter.worldPos.x + emitterPosition.x + offsetX;
        particle->position.y = emitter.worldPos.y + emitterPosition.y + offsetY;
        particle->position.z = emitter.worldPos.z + emitterPosition.z + offsetZ;
        particle->position.w = 1;

        particle->color = emitter.particleStartColor;

        particle->velocity.x = emitterVelocity.x + randomFloat(-1.5f, 1.5f);
        particle->velocity.y = emitterVelocity.y + randomFloat(-1.5f, 1.5f);
        particle->velocity.z = emitterVelocity.z + randomFloat(-1.5f, 1.5f);

        particle->size = emitter.particleStartSize;
        particle->lifetime = 0.0f;
        particle->maxLife = randomFloat(emitter.particleMinLifetime, emitter.particleMaxLifetime);

        // Update the particle as if it has already been 
        // alive for deltaT - dT
        updateParticle(*particle, deltaT - dT);

        ++activeParticleCount;
    }

    numActiveParticles = activeParticleCount;
}

void ParticleSystem::getDrawCalls(gfx::ResourceManager& resourceManager, std::vector<gfx::DrawCall>& drawCalls) {
    // Here we're basically copying the particle data to memory that the shader can access.
    resourceManager.streamDataToStorageBuffer(storageBufferHandle, [this](void* buffer) {
            // The shader needs 3 properties:
            // - position (p)
            // - color (c)
            // - size (s)
            // These properties need to be sent to the shader using a specific memory layout 
            // called std140. There are two basic rules.
            // 1. The properties need to be contigious, i.e. if there are four particles, 
            //    then we need to send the data like pppp|cccc|ssss, NOT pcs|pcs|pcs|pcs
            // 2. The properties need to be aligned to 4 floats. This is fine for 
            //    p and c, because they are already vec4s. However, the size is just 
            //    a single float. So we'll have to allocate 4 floats per size, and just 
            //    use the first float, leaving the other 3 floats unused. It is a waste of 
            //    some memory, but we have plenty, and the nice alignment makes the
            //    GPU go brrrrr.
            glm::vec4* position = (glm::vec4*)buffer;
            glm::vec4* color = position + config.maxParticles;
            glm::vec4* size = color + config.maxParticles;

            for (int i = 0; i < config.maxParticles; i++) {
                const Particle& particle = particles[i];
                bool isAlive = particle.lifetime < particle.maxLife;
                if (isAlive) {
                    // For some reason, memcpy seems faster than assignment. I should investigate.
                    memcpy_s(position++, sizeof(glm::vec4), &particle.position, sizeof(glm::vec4));
                    memcpy_s(color++, sizeof(glm::vec4), &particle.color, sizeof(glm::vec4));
                    memcpy_s(size++, sizeof(float), &particle.size, sizeof(float));
                }
            }
        });

    gfx::DrawCall call;
    call.mode = gfx::DrawCall::Mode::TRIANGLES;
    call.numIndices = numActiveParticles * INDICES_PER_PARTICLE;
    call.indexType = gfx::DrawCall::IndexType::UINT;
    call.indices = nullptr;
    call.programHandle = programHandle;
    call.storageBuffer = storageBufferHandle;
    call.vaoHandle = vaoHandle;
    call.storageBufferBaseIndex = 0;

    drawCalls.push_back(call);
}
