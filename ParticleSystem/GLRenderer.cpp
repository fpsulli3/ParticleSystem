#include <glm/ext.hpp>
#include "GLRenderer.h"

namespace gfx {

	const GLuint CAMERA_UNIFORM_BLOCK_INDEX = 0;

	void GLRenderer::clear(const ClearOptions& clearOptions) {
		GLbitfield clearMask = 0;
		if (clearOptions.clearColor) {
			clearMask |= GL_COLOR_BUFFER_BIT;
			glClearColor(clearOptions.r, clearOptions.g, clearOptions.b, clearOptions.a);
		}

		if (clearOptions.clearDepth) {
			clearMask |= GL_DEPTH_BUFFER_BIT;
			glClearDepth(clearOptions.clearDepth);
		}

		if (clearOptions.clearStencil) {
			clearMask |= GL_STENCIL_BUFFER_BIT;
			glClearStencil(clearOptions.clearStencil);
		}

		if (clearMask != 0) {
			glClear(clearMask);
		}
	}

	void GLRenderer::setupCamera(GLuint ubo, const Camera& camera, const Viewport& viewport) {

		// We need to inform the shaders of the camera's properties 
		// so that they can correctly transform the vertices, etc. 
		// We are choosing to do this via a mechanism called a Uniform 
		// Buffer Object. 

		// The first step is to calculate all of the data  here on the 
		// CPU side; then, we can transfer the data to a Uniform Buffer, 
		// which is basically just a fast block of GPU memory. We then need 
		// to tell OpenGL to plug that Uniform buffer into our shaders.

		glViewport(viewport.x, viewport.y, viewport.width, viewport.height);

		// This CameraUBOData struct has the exact memory layout 
		// that our shaders expect.
		CameraUBOData uboData;
		uboData.worldMat = camera.getTransform().getMatrix();
		uboData.viewMat = glm::inverse(uboData.worldMat);
		uboData.projMat = glm::perspectiveFov(
			camera.getFovy(), 
			(float) viewport.width,
			(float) viewport.height,
			camera.getNear(), 
			camera.getFar());
		uboData.viewProjMat = uboData.projMat * uboData.viewMat;

		// Next, we are transfering the camera data we set in uboData 
		// to the graphics card via something called a Uniform 
		// Buffer Object (UBO).

		// First, we tell OpenGL that our UBO is the current UBO.
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);

		// Next, we tell OpenGL to allocate us the right 
		// amount of GPU memory to hold the uboData.
		int size = sizeof(CameraUBOData);
		glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STREAM_DRAW);

		// Next, we actually copy the uboData to this GPU memory.
		void* buffer = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
		memcpy(buffer, &uboData, size);
		glUnmapBuffer(GL_UNIFORM_BUFFER);

		// Lastly, we tell OpenGL that we need to match this UBO 
		// to the correct interface block in the shader.
		glBindBufferBase(GL_UNIFORM_BUFFER, CAMERA_UNIFORM_BLOCK_INDEX, ubo);
	}
}
