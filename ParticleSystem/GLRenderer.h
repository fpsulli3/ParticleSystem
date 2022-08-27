#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

#include "Camera.h"
#include "ClearOptions.h"
#include "DrawCall.h"
#include "GLResourceManager.h"
#include "Renderer.h"
#include "Viewport.h"

namespace gfx {

	// An OpenGL implementation of the Renderer class
	class GLRenderer: public Renderer {
	public:
		GLRenderer(GLResourceManager& resourceManager);
		~GLRenderer();

		// Clears the screen entirely, according to the ClearOptions.
		void clear(const ClearOptions& clearOptions);


		// Takes the properties of the Camera and informs the 
		// shaders about the Camera's properties so that everything 
		// is rendered in the right place, with the right field of 
		// view and perspective distortion, etc.
		void setupCamera(const Camera& camera, const Viewport& viewport);

		// Takes a list of draw calls and draws them!
		void draw(const std::vector<DrawCall> drawCalls);

	private:

		// This struct has the exact same memory 
		// layout as the Camera interface block that 
		// is found in the shaders. So, we can create 
		// and instance of this, and set the values 
		// for each field, and then directly transfer 
		// the data the shaders via something called 
		// a Uniform Buffer Object.
		struct CameraUBOData {
			glm::mat4x4 worldMat;
			glm::mat4x4 viewMat;
			glm::mat4x4 projMat;
			glm::mat4x4 viewProjMat;
		};

		GLResourceManager& resourceManager;
		ResourceManager::HBUFFER cameraUniformBuffer;
	};

}