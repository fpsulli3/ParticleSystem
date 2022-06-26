#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "ClearOptions.h"
#include "Viewport.h"
#include "Camera.h"

namespace gfx {

	class GLRenderer {
	public:

		// Clears the screen entirely, according to the ClearOptions.
		void clear(const ClearOptions& clearOptions);

		// Sets up the Viewport. The Viewport is the rectangular 
		// section of the screen that we're going to draw the 
		// scene to. Usually this is the full window, but if we 
		// wanted to do some kind of split-screen or picture-in-picture 
		// thing, we could makae the rectangle smaller than the full 
		// window if we want.
		void setupViewport(const Viewport& viewport);

		// Takes the properties of the Camera and informs the 
		// shaders about the Camera's properties so that everything 
		// is rendered in the right place, with the right field of 
		// view and perspective distortion, etc.
		void setupCamera(GLuint ubo, const Camera& camera);

	private:

		// We need to know the aspect ratio of the Viewport 
		// in order to render things correctly, so that they 
		// don't look squished or squeezed.
		float currentViewportAspectRatio = 1.0f;

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
	};

}