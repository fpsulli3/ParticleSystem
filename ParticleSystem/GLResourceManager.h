#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <GL/glew.h>
#include <vector>
#include <map>
#include <list>
#include "ResourceManager.h"

namespace gfx {
	const GLuint RESOURCE_CREATION_FAILED = -1;

	/**
	 * Handles the creation and deletion of OpenGL resources.
	 * 
	 * OpenGL Resources include things like:
	 *  - Textures/Images
	 *  - Shaders/Programs
	 *  - Buffer Objects, such as Vertex Buffers (VBOs), Uniform Buffers (UBOs), etc.
	 *  
	 * The methods for creating and deleting these resources are generally 
	 * easier to use than their OpenGL counterparts. 
	 * 
	 * For convenience, the GLResourceManager will also keep track of any 
	 * resources that have been created but not yet destroyed. That way, 
	 * if there are any resources that are still alive when the 
	 * GLResourceManager is destroyed, its destructor will delete those 
	 * resources for you.
	 */
	class GLResourceManager : public ResourceManager {
	public:

		~GLResourceManager();

		// Shader Programs
		HPROGRAM createProgramFromSource(const ShaderSource* shaders, unsigned int numShaders);
		void deleteProgram(HPROGRAM programHandle);

		// Buffers
		HBUFFER createStreamingUniformBuffer(unsigned int initialDataSize, unsigned char* initialData);
		void streamDataToUniformBuffer(HBUFFER bufferHandle, unsigned int dataSize, const void* data);
		void bindUniformBufferBase(HBUFFER handle, unsigned int index);
		HBUFFER createStreamingStorageBuffer(unsigned int initialDataSize, unsigned char* initialData);
		void streamDataToStorageBuffer(HBUFFER bufferHandle, unsigned int dataSize, const void* data);

		void deleteBuffer(HBUFFER bufferHandle);

		/**
		 * Returns the last error that occurred while managing a resource. I don't 
		 * usually like to do error handling this way, but OpenGL makes it a 
		 * pain sometimes to do things in a better way.
		 */
		const GLchar* getLastError() { return lastError; }

	private:

		const GLchar* lastError = NULL;

		struct ProgramDesc {
			GLuint handle;
			unsigned int numShaders;
			GLuint* shaderHandles;
		};

		std::map<HPROGRAM, ProgramDesc> programs;

		struct BufferDesc {
			HBUFFER bufferHandle;
			unsigned int initialSize;
		};

		std::map<HBUFFER, BufferDesc> buffers;
		HBUFFER curUniformBuffer = 0;
		HBUFFER curStorageBuffer = 0;

		void bindUniformBuffer(HBUFFER buffer);
		void bindStorageBuffer(HBUFFER buffer);

		void deleteProgram(const ProgramDesc& program);
		void setLastError(const GLchar* error);

		GLuint createStreamingBuffer(GLenum target, unsigned int initialDataSize, unsigned char* initialData);
		void streamDataToBuffer(GLenum target, HBUFFER bufferHandle, unsigned int dataSize, const void* data);
	};
}