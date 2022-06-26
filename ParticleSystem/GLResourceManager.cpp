#include "GLResourceManager.h"

namespace gfx {

	GLResourceManager::~GLResourceManager() {
		map<GLuint, ProgramDesc>::iterator itr = programs.begin(), progEnd = programs.end();
		while (itr != programs.end()) {
			deleteProgram(itr->second);
			programs.erase(itr++);
		}
	}

	GLuint GLResourceManager::createProgramFromSource(const vector<ShaderSourceDesc>& desc) {
		unsigned int numShaders = (unsigned int)desc.size();

		ProgramDesc program;
		program.handle = glCreateProgram();
		program.numShaders = numShaders;
		program.shaderHandles = new GLuint[numShaders];

		for (size_t i = 0, iCount = desc.size(); i < iCount; ++i) {
			const ShaderSourceDesc& d = desc[i];
			GLuint shader = glCreateShader(d.type);

			glShaderSource(shader, 1, &(d.source), NULL);
			glCompileShader(shader);

			GLint success;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				GLint logLength;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
				char* infoLog = new char[logLength];
				glGetShaderInfoLog(shader, logLength, NULL, infoLog);
				setLastError(infoLog);

				return RESOURCE_CREATION_FAILED;
			}

			glAttachShader(program.handle, shader);
			program.shaderHandles[i] = shader;
		}

		glLinkProgram(program.handle);
		GLint success;
		glGetProgramiv(program.handle, GL_LINK_STATUS, &success);
		if (!success) {
			GLint logLength;
			glGetProgramiv(program.handle, GL_INFO_LOG_LENGTH, &logLength);
			char* infoLog = new char[logLength];
			glGetProgramInfoLog(program.handle, logLength, NULL, infoLog);
			setLastError(infoLog);
			
			return RESOURCE_CREATION_FAILED;
		}

		programs.emplace(program.handle, program);

		return program.handle;
	}

	void GLResourceManager::deleteProgram(GLuint programName) {
		map<GLuint, ProgramDesc>::iterator itr = programs.find(programName);
		if (itr != programs.end()) {
			deleteProgram(itr->second);
			programs.erase(itr);
		}
	}

	void GLResourceManager::deleteProgram(const ProgramDesc& program) {
		if (program.shaderHandles != nullptr) {
			for (size_t shaderIndex = 0; shaderIndex < program.numShaders; ++shaderIndex) {
				GLuint shader = program.shaderHandles[shaderIndex];
				glDeleteShader(shader);
			}

			delete[] program.shaderHandles;
		}

		glDeleteProgram(program.handle);
	}

	void GLResourceManager::setLastError(const GLchar* error) {
		if (lastError != NULL) {
			delete[] lastError;
			lastError = NULL;
		}
	}
}