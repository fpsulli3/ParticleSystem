#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <GL/glew.h>
#include <vector>
#include <map>

using namespace std;

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
	class GLResourceManager {
	public:

		~GLResourceManager();

		struct ShaderSourceDesc {
			GLenum type;
			const GLchar* source;
		};

		GLuint createProgramFromSource(const vector<ShaderSourceDesc>& desc);
		void deleteProgram(GLuint programHandle);

		GLuint createStreamingUniformBuffer(int size, unsigned char* initialData);

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

		map<GLuint, ProgramDesc> programs;

		void deleteProgram(const ProgramDesc& program);
		void setLastError(const GLchar* error);
	};
}