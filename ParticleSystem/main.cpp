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

#include "ParticleSystem.h"

using namespace glm;

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

    ParticleSystem::Config particleSystemConfig;
    particleSystemConfig.maxParticles = 10000;
    ParticleSystem particleSystem(particleSystemConfig);
    particleSystem.initGraphicsResources(resourceManager);

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

    std::vector<gfx::DrawCall> drawCalls;

    int numFrames = 0;
    Win32Timer timer;
    MSG msg = {};
    bool done = false;
    while (!done) {
        keyboardInput.onFrameBegin();
        mouseInput.onFrameBegin();
        timer.onFrameBegin();
        drawCalls.clear();

        ++numFrames;

        while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT) {
                done = true;
            }
        }

        camera.processInput(keyboardInput, mouseInput, timer.getDeltaTime());
        particleSystem.update(timer.getDeltaTime());

        particleSystem.getDrawCalls(resourceManager, drawCalls);

        viewport.width = window.getClientWidth();
        viewport.height = window.getClientHeight();

        renderer.clear(clearOptions);
        renderer.setupCamera(camera, viewport);

        renderer.draw(drawCalls);

        glDevice.swapBuffers();
    }

    double avgFrameTime = timer.getTotalTime() / numFrames;

    return 0;
}