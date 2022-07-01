#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Utils.h"
#include "Camera.h"

#include "Window.h"
#include "Win32KeyboardInput.h"
#include "Win32MouseInput.h"
#include "Win32Timer.h"

#include "GLDevice.h"
#include "GLResourceManager.h"
#include "GLRenderer.h"

using namespace glm;

#define NUM_PARTICLES 10000

struct Particles {
    glm::vec4 positions[NUM_PARTICLES];
    glm::vec4 colors[NUM_PARTICLES];
    glm::vec4 sizes[NUM_PARTICLES]; // weird std140 alignment rules
};

// Linear intERPolation
float lerp(float start, float end, float t) {
    return start + t * (end - start);
}

float randomFloat(float min, float max) {
    float t = (float)(rand() % 10000) / 10000.f; // gives us a value between 0.0 and 1.0
    return lerp(min, max, t);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    WindowParams params;
    params.hInstance = hInstance;
    params.nCmdShow = nCmdShow;
    params.title = L"Particle System";
    params.width = 3440;
    params.height = 1440;
    params.fullScreen = true;

    Window window = Window(params);
    input::Win32KeyboardInput keyboardInput;
    input::Win32MouseInput mouseInput(window.getHandle());

    window.setKeyboardEventHandler(&keyboardInput);
    window.setOnMouseMovedHandler(&mouseInput);

    gfx::GLDevice glDevice(window.getHandle());
    glDevice.makeCurrent();

    gfx::GLResourceManager resourceManager;
    gfx::GLRenderer renderer(resourceManager);

    GLuint shaderProgram;
    {
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

        shaderProgram = resourceManager.createProgramFromSource(shaders, 2);

        if (shaderProgram == gfx::RESOURCE_CREATION_FAILED) {
            ::MessageBoxA(
                window.getHandle(), 
                resourceManager.getLastError(), 
                "Shader Program Error", 
                MB_OK | MB_ICONERROR);
        }
    }

    const float MAX_EXPLOSION_RADIUS = 10.0f;

    Particles *particles = new Particles;

    for (int i = 0; i < NUM_PARTICLES; i++) {
        float theta = randomFloat(0, glm::pi<float>());
        float phi = randomFloat(0, 2.0*glm::pi<float>());
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

        particles->positions[i] = glm::vec4(x, y, z, 1);
        particles->colors[i] = glm::vec4(r, g, b, 1);
        particles->sizes[i] = glm::vec4(size, 100, 100, 100);
    }

    GLuint ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Particles), particles, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    const int NUM_INDICES = NUM_PARTICLES * 6;
    unsigned int*indices = new unsigned int[NUM_INDICES];

    for (int i = 0; i < NUM_INDICES; i++) {
        indices[i] = i;
    }

    GLsizeiptr indexBufferSize = sizeof(unsigned int) * NUM_PARTICLES * 6;
    GLuint indexBuffer;
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, indices, GL_STATIC_DRAW);

    window.show();

    Camera camera;

    Viewport viewport;
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = window.getClientWidth();
    viewport.height = window.getClientHeight();

    ClearOptions clearOptions;
    clearOptions.clearColor = true;
    clearOptions.r = 0.5f;
    clearOptions.g = 0.58f;
    clearOptions.b = 0.93f;
    clearOptions.a = 1.0f;
    clearOptions.clearDepth = true;
    clearOptions.depth = 1.0;
    clearOptions.clearStencil = true;
    clearOptions.stencilValue = 0;

    int numFrames = 0;
    Win32Timer timer;
    MSG msg = {};
    bool done = false;
    while (!done) {
        keyboardInput.onFrameBegin();
        mouseInput.onFrameBegin();
        timer.onFrameBegin();

        ++numFrames;

        while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT) {
                done = true;
            }
        }

        camera.processInput(keyboardInput, mouseInput, timer.getDeltaTime());

        viewport.width = window.getClientWidth();
        viewport.height = window.getClientHeight();

        renderer.clear(clearOptions);
        renderer.setupCamera(camera, viewport);

        glUseProgram(shaderProgram);

        glDrawElements(GL_TRIANGLES, NUM_INDICES, GL_UNSIGNED_INT, NULL);

        glDevice.swapBuffers();
    }

    double avgFrameTime = timer.getTotalTime() / numFrames;

    delete[] indices;
    delete particles;

    return 0;
}