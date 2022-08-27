#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Utils.h"
#include "Camera.h"

#include "Window.h"
#include "Win32KeyboardInput.h"
#include "Win32MouseInput.h"
#include "Win32Timer.h"

#include "Device.h"
#include "ResourceManager.h"
#include "Renderer.h"

#include "ParticleSystem.h"
#include "GraphicsSystem.h"

using namespace glm;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    // Create window
    WindowParams params;
    params.hInstance = hInstance;
    params.nCmdShow = nCmdShow;
    params.title = L"Particle System";
    params.width = 3440;
    params.height = 1440;
    params.fullScreen = true;
    Window window = Window(params);

    // Init graphics
    gfx::GraphicsSystem gfx(gfx::GraphicsSystem::OpenGL, window);
    gfx.device().makeCurrent();

    // Init input
    input::Win32KeyboardInput keyboardInput;
    input::Win32MouseInput mouseInput(window.getHandle());
    window.setKeyboardEventHandler(&keyboardInput);
    window.setOnMouseMovedHandler(&mouseInput);

    // Init scene
    ParticleSystem::Config particleSystemConfig;
    particleSystemConfig.maxParticles = 100000;
    ParticleSystem particleSystem(particleSystemConfig);
    particleSystem.initGraphicsResources(gfx.resourceManager());

    Camera camera;

    Viewport viewport;
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = window.getClientWidth();
    viewport.height = window.getClientHeight();

    ClearOptions clearOptions;
    clearOptions.clearColor = true;
    clearOptions.r = 0.1f;
    clearOptions.g = 0.05f;
    clearOptions.b = 0.1f;
    clearOptions.a = 1.0f;
    clearOptions.clearDepth = true;
    clearOptions.depth = 1.0;
    clearOptions.clearStencil = true;
    clearOptions.stencilValue = 0;

    std::vector<gfx::DrawCall> drawCalls;

    window.show();

    int numFrames = 0;
    Win32Timer timer;
    MSG msg = {};
    bool done = false;
    while (!done) {
        // Reset
        keyboardInput.onFrameBegin();
        mouseInput.onFrameBegin();
        timer.onFrameBegin();
        drawCalls.clear();

        // Update
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

        // Cull
        particleSystem.getDrawCalls(gfx.resourceManager(), drawCalls);

        // Render
        viewport.width = window.getClientWidth();
        viewport.height = window.getClientHeight();
        gfx.renderer().clear(clearOptions);
        gfx.renderer().setupCamera(camera, viewport);

        gfx.renderer().draw(drawCalls);

        gfx.device().swapBuffers();
    }

    double avgFrameTime = timer.getTotalTime() / numFrames;

    return 0;
}