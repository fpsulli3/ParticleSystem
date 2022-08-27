#include "GraphicsSystem.h"
#include "GLDevice.h"
#include "GLResourceManager.h"
#include "GLRenderer.h"

namespace gfx {
	GraphicsSystem::GraphicsSystem(API api, const Window& window) {
		switch (api) {
		case OpenGL:
			_device = new GLDevice(window.getHandle());
			GLResourceManager* glResourceManager = new GLResourceManager();
			_resourceManager = glResourceManager;
			_renderer = new GLRenderer(*glResourceManager);
			break;
		}
	}

	GraphicsSystem::~GraphicsSystem() {
		if (_device != nullptr) {
			delete _device;
			_device = nullptr;		
		}

		if (_resourceManager != nullptr) {
			delete _resourceManager;
			_resourceManager = nullptr;
		}

		if (_renderer != nullptr) {
			delete _renderer;
			_renderer = nullptr;
		}
	}
}