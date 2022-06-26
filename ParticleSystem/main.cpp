#include <string>
#include <GL/glew.h>
#include "Window.h"
#include "GLRenderer.h"
#include "Utils.h"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include "GLDevice.h"
#include "GLResourceManager.h"
#include "KeyboardInput.h"
#include "Camera.h"

using namespace glm;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    WindowParams params;
    params.hInstance = hInstance;
    params.nCmdShow = nCmdShow;
    params.title = L"Particle System";
    params.width = 1920;
    params.height = 1080;

    Window window = Window(params);
    input::KeyboardInput keyInput;

    window.setOnKeyUpHandler([&keyInput] (UINT vkCode) {
        keyInput.notifyKeyUpEvent(vkCode);
    });
    
    window.setOnKeyDownHandler([&keyInput] (UINT vkCode) {
        keyInput.notifyKeyDownEvent(vkCode);
    });

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
        -1.0f, -1.0f, -2.0f,
         1.0f, -1.0f, -2.0f,
         0.0f,  1.0f, -2.0f
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

    int redInput = 0;
    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {

            if (keyInput.isKeyDown(VK_DOWN)) {
                ++redInput;
            }

            ClearOptions clearOptions;
            clearOptions.clearColor = true;
            clearOptions.r = (redInput % 256) / 255.0f;
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
            renderer.setupViewport(viewport);

            renderer.setupCamera(cameraUbo, camera);

            glUseProgram(shaderProgram);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);

            glDevice.swapBuffers();
            keyInput.swapBuffers();
        }
    }

    return 0;
}