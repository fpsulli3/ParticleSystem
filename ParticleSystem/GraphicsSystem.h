#pragma once

#include "Device.h"
#include "ResourceManager.h"
#include "Renderer.h"
#include "Window.h"

namespace gfx {

	class GraphicsSystem {
	public:
		enum API {
			OpenGL // The only supported API at the moment.
		};

		GraphicsSystem(API api, const Window& window);
		~GraphicsSystem();

		Device& device() { return *_device; }
		ResourceManager& resourceManager() { return *_resourceManager; }
		Renderer& renderer() { return *_renderer; }

	private:
		Device* _device;
		ResourceManager* _resourceManager;
		Renderer* _renderer;
	};
}