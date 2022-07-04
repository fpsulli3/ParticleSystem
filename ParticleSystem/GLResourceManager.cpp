#include "GLResourceManager.h"


namespace gfx {

	std::map<ResourceManager::ShaderType, GLenum> shaderTypeLookup;
	GLenum lookUpShaderType(ResourceManager::ShaderType type) {
		static bool initialized = false;
		if (!initialized) {
			shaderTypeLookup.emplace(ResourceManager::ShaderType::COMPUTE_SHADER, GL_COMPUTE_SHADER);
			shaderTypeLookup.emplace(ResourceManager::ShaderType::VERTEX_SHADER, GL_VERTEX_SHADER);
			shaderTypeLookup.emplace(ResourceManager::ShaderType::FRAGMENT_SHADER, GL_FRAGMENT_SHADER);
			initialized = true;
		}

		std::map<ResourceManager::ShaderType, GLenum>::const_iterator itr = shaderTypeLookup.find(type);
		if (itr == shaderTypeLookup.end()) {
			return -1;
		}

		return itr->second;
	}

	GLResourceManager::~GLResourceManager() {
		// Clean up remaining Programs
		std::map<HPROGRAM, ProgramDesc>::const_iterator programItr = programs.begin();
		while (programItr != programs.end()) {
			const ProgramDesc& programDescription = programItr->second;
			deleteProgram(programDescription);
			programs.erase(programItr++);
		}

		// Clean u p remaining Buffers
		std::map<HBUFFER, BufferDesc>::const_iterator bufferItr = buffers.begin();
		while (bufferItr != buffers.end()) {
			HBUFFER handle = bufferItr->first;
			glDeleteBuffers(1, &handle);
			buffers.erase(bufferItr++);
		}
	}

	ResourceManager::HPROGRAM GLResourceManager::createProgramFromSource(const ShaderSource* shaders, unsigned int numShaders) {

		ProgramDesc program;
		program.handle = glCreateProgram();
		program.numShaders = numShaders;
		program.shaderHandles = new GLuint[numShaders];

		for (size_t i = 0; i < numShaders; ++i) {
			const ShaderSource& d = shaders[i];
			GLenum shaderType = lookUpShaderType(d.type);
			GLuint shader = glCreateShader(shaderType);

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

	void GLResourceManager::deleteProgram(HPROGRAM programName) {
		std::map<HPROGRAM, ProgramDesc>::iterator itr = programs.find(programName);
		if (itr != programs.end()) {
			deleteProgram(itr->second);
			programs.erase(itr);
		}
	}

	void GLResourceManager::useProgram(HPROGRAM programHandle) {
		glUseProgram(programHandle);
	}

	GLResourceManager::HBUFFER GLResourceManager::createStreamingUniformBuffer(unsigned int initialDataSize, unsigned char* initialData) {
		return createStreamingBuffer(GL_UNIFORM_BUFFER, initialDataSize, initialData);
	}

	void GLResourceManager::streamDataToUniformBuffer(HBUFFER bufferHandle, unsigned int dataSize, const void* data) {
		streamDataToBuffer(GL_UNIFORM_BUFFER, bufferHandle, dataSize, data);
	}

	void GLResourceManager::bindUniformBufferBase(HBUFFER handle, unsigned int index) {
		glBindBufferBase(GL_UNIFORM_BUFFER, index, handle);
	}

	GLResourceManager::HBUFFER GLResourceManager::createStreamingStorageBuffer(unsigned int initialDataSize, unsigned char* initialData) {
		return createStreamingBuffer(GL_SHADER_STORAGE_BUFFER, initialDataSize, initialData);
	}

	void GLResourceManager::streamDataToStorageBuffer(HBUFFER bufferHandle, unsigned int dataSize, const void* data) {
		streamDataToBuffer(GL_SHADER_STORAGE_BUFFER, bufferHandle, dataSize, data);
	}

	void GLResourceManager::bindStorageBufferBase(HBUFFER handle, unsigned int index) {
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, handle);
	}

	void GLResourceManager::deleteBuffer(HBUFFER bufferHandle) {
		std::map<HBUFFER, BufferDesc>::const_iterator itr = buffers.find(bufferHandle);
		if (itr != buffers.end()) {
			buffers.erase(itr);
			glDeleteBuffers(1, &bufferHandle);
		}
	}

	void GLResourceManager::bindUniformBuffer(HBUFFER buffer) {
		if (buffer != curUniformBuffer) {
			glBindBuffer(GL_UNIFORM_BUFFER, buffer);
			curUniformBuffer = buffer;
		}
	}

	void GLResourceManager::bindStorageBuffer(HBUFFER buffer) {
		if (buffer != curStorageBuffer) {
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
			curStorageBuffer = buffer;
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

	GLuint GLResourceManager::createStreamingBuffer(GLenum target, unsigned int initialDataSize, unsigned char* initialData) {
		GLuint buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(target, buffer);
		glBufferData(target, initialDataSize, initialData, GL_STREAM_DRAW);
		BufferDesc bufferDescription;
		bufferDescription.bufferHandle = buffer;
		bufferDescription.initialSize = initialDataSize;
		buffers.emplace(buffer, bufferDescription);
		return buffer;
	}

	void GLResourceManager::streamDataToBuffer(GLenum target, HBUFFER bufferHandle, unsigned int dataSize, const void* data) {
		std::map<HBUFFER, BufferDesc>::const_iterator itr = buffers.find(bufferHandle);

		if (itr != buffers.end()) {
			const BufferDesc& buffer = itr->second;
			// First, we bind our buffer to the correct target.
			glBindBuffer(target, bufferHandle);

			// Next, we tell OpenGL to allocate us the right 
			// amount of GPU memory to hold the buffer data. With Buffer 
			// Streaming, we target to always use the INITIAL amount.
			glBufferData(target, buffer.initialSize, NULL, GL_STREAM_DRAW);

			// Next, we actually copy the data to this GPU memory.
			void* bufferMem = glMapBuffer(target, GL_WRITE_ONLY);
			memcpy(bufferMem, data, dataSize);
			glUnmapBuffer(target);
		}
	}

	ResourceManager::HVAO GLResourceManager::createVAO(const VAOConfig& config) {
		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(1);

		if (config.indexData != NULL) {
			GLuint indexBuffer;
			glGenBuffers(1, &indexBuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, config.indexBufferSizeBytes, config.indexData, GL_STATIC_DRAW);
		}

		return vao;
	}

	void GLResourceManager::deleteVAO(HVAO vaoHandle) {
		glDeleteVertexArrays(1, &vaoHandle);		
	}

	void GLResourceManager::bindVAO(HVAO vaoHandle) {
		glBindVertexArray(vaoHandle);
		curVao = vaoHandle;
	}
}