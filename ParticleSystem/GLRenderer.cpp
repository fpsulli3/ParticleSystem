#include <glm/ext.hpp>
#include "GLRenderer.h"

namespace gfx {

	const GLuint CAMERA_UNIFORM_BLOCK_INDEX = 0;

	GLRenderer::GLRenderer(GLResourceManager& resourceManager) : resourceManager(resourceManager) {
		cameraUniformBuffer = resourceManager.createStreamingUniformBuffer(sizeof(CameraUBOData), NULL);
	}

	GLRenderer::~GLRenderer() {
		resourceManager.deleteBuffer(cameraUniformBuffer);
	}

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

	void GLRenderer::setupCamera(const Camera& camera, const Viewport& viewport) {

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

		// Stream the data to video memory buffer
		resourceManager.streamDataToUniformBuffer(cameraUniformBuffer, [uboData](void* buffer) {
				memcpy_s(buffer, sizeof(CameraUBOData), (const void*)&uboData, sizeof(CameraUBOData));
			});

		// Lastly, we tell OpenGL that we need to match this UBO 
		// to the correct interface block in the shader.
		resourceManager.bindUniformBufferBase(cameraUniformBuffer, CAMERA_UNIFORM_BLOCK_INDEX);
	}

	void GLRenderer::draw(const std::vector<DrawCall> drawCalls) {

		for (size_t i = 0; i < drawCalls.size(); i++) {
			const DrawCall& drawCall = drawCalls[i];
			bool done = false;
			if (!done) {
				resourceManager.useProgram(drawCall.programHandle);
				resourceManager.bindStorageBufferBase(drawCall.storageBuffer, drawCall.storageBufferBaseIndex);
				resourceManager.bindVAO(drawCall.vaoHandle);
				done = true;
			}
			glDrawElements(GL_TRIANGLES, drawCall.numIndices, GL_UNSIGNED_INT, NULL);
		}
	}
}
