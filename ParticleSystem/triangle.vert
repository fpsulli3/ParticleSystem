#version 460 core

#define NUM_PARTICLES 10000

const vec4 offsets[6] = vec4[6](
	vec4(-0.5, -0.5, 0, 1),
	vec4(-0.5,  0.5, 0, 1),
	vec4( 0.5,  0.5, 0, 1),
	vec4(-0.5, -0.5, 0, 1),
	vec4( 0.5,  0.5, 0, 1),
	vec4( 0.5, -0.5, 0, 1)
);

// The Camera UBO interface block
layout (std140, binding = 0) uniform Camera {
    mat4 worldMat;
    mat4 viewMat;
    mat4 projMat;
    mat4 viewProjMat;
} camera;

layout(std140, binding = 0) buffer Particles {
    vec4 positions[NUM_PARTICLES];
    vec4 colors[NUM_PARTICLES];
    float sizes[NUM_PARTICLES];
} particles;

out vec4 color;

void main() {
    int particleID = gl_VertexID / 6;
    int offsetIndex = gl_VertexID % 6;

    vec4 viewSpacePos = camera.viewMat * particles.positions[particleID];   
    vec4 offset = offsets[offsetIndex] * particles.sizes[particleID];
    viewSpacePos += offset;
    gl_Position = camera.projMat * viewSpacePos;
    
    color = particles.colors[particleID];
}