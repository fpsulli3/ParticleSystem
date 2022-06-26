#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Utils.h"
#include "Camera.h"

#include "Window.h"
#include "Win32KeyboardInput.h"
#include "Win32MouseInput.h"

#include "GLDevice.h"
#include "GLResourceManager.h"
#include "GLRenderer.h"

using namespace glm;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    WindowParams params;
    params.hInstance = hInstance;
    params.nCmdShow = nCmdShow;
    params.title = L"Particle System";
    params.width = 1920;
    params.height = 1080;

    Window window = Window(params);
    input::Win32KeyboardInput keyboardInput;
    input::Win32MouseInput mouseInput;

    window.setKeyboardEventHandler(&keyboardInput);
    window.setOnMouseMovedHandler(&mouseInput);

    gfx::GLDevice glDevice(window.getHandle());
    glDevice.makeCurrent();

    gfx::GLResourceManager resourceManager;
    gfx::GLRenderer renderer;

    GLuint shaderProgram;
    {
        std::string vertShaderSource = loadAsciiFile("triangle.vert");
        std::string fragShaderSource = loadAsciiFile("triangle.frag");
        shaderProgram = resourceManager.createProgramFromSource({
                { GL_VERTEX_SHADER, vertShaderSource.c_str() },
                { GL_FRAGMENT_SHADER, fragShaderSource.c_str() }
            });

        if (shaderProgram == gfx::RESOURCE_CREATION_FAILED) {
            ::MessageBoxA(
                window.getHandle(), 
                resourceManager.getLastError(), 
                "Shader Program Error", 
                MB_OK | MB_ICONERROR);
        }

        float color[4] = {1.0, 0.0, 0.0, 1.0};
        GLuint ssbo;
        glGenBuffers(1, &ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float)*4, &color, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
    }

    float vertices[] = {
        -1.0f, -1.0f, -20.0f,
         1.0f, -1.0f, -20.0f,
         0.0f,  1.0f, -20.0f
    };

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    window.show();

    GLuint cameraUbo;
    glGenBuffers(1, &cameraUbo);
    Camera camera;

    MSG msg = {};
    bool done = false;
    while (!done) {
        keyboardInput.onFrameBegin();
        mouseInput.onFrameBegin();

        while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT) {
                done = true;
            }
        }

        camera.processInput(keyboardInput, mouseInput, 1.0f / 60.0f);

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
        renderer.clear(clearOptions);

        Viewport viewport;
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = window.getClientWidth();
        viewport.height = window.getClientHeight();

        renderer.setupCamera(cameraUbo, camera, viewport);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glDevice.swapBuffers();
    }

    return 0;
}