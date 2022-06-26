#version 460 core
layout (location = 0) in vec3 aPos;

// The Camera UBO interface block
layout (std140, binding = 0) uniform Camera {
    mat4 worldMat;
    mat4 viewMat;
    mat4 projMat;
    mat4 viewProjMat;
} camera;

layout(std430, binding = 0) buffer Vert
{
    vec4 color;
} vert[2];

out vec4 color;

void main() {
    color = vert[0].color;
    gl_Position = camera.viewProjMat * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}