#pragma once
#include <functional>

namespace gfx {

	// A ResourceManager handles the creation and destruction 
	// of graphics resources. 
	//
	// A "resource" is anything that we must specifically request 
	// access to, which we must also carefully release when we're 
	// finished with it. Some good examples of resources are memory, 
	// files on disk, network connections, etc.
	// 
	// But this class is specifically for graphics resources, which 
	// includes things like GPU memory buffers, shaders, images, 
	// textures, and so on.
	//
	// This is an Abstract Base Class (ABC) that is api-agnostic.
	// That is, it has no dependence on OpenGL or any other graphics 
	// api. There is a subclass called GLResourceManager, where all 
	// of the OpenGL-specific code lives.
	class ResourceManager {
	public:

		// Shader Programs
		typedef unsigned int HPROGRAM;

		enum class ShaderType {
			COMPUTE_SHADER,
			VERTEX_SHADER,
			FRAGMENT_SHADER
		};

		struct ShaderSource {
			ShaderType type;
			const char* source;
		};

		virtual HPROGRAM createProgramFromSource(const ShaderSource* shaders, unsigned int numShaders) = 0;
		virtual void deleteProgram(HPROGRAM programHandle) = 0;

		// Buffers
		typedef std::function<void(void* buffer)> BufferCallback;
		typedef unsigned int HBUFFER;

		virtual HBUFFER createStreamingUniformBuffer(unsigned int initialDataSize, unsigned char* initialData) = 0;
		virtual void streamDataToUniformBuffer(HBUFFER bufferHandle, const BufferCallback &bufferCallback) = 0;

		virtual HBUFFER createStreamingStorageBuffer(unsigned int initialDataSize, unsigned char* initialData) = 0;
		virtual void streamDataToStorageBuffer(HBUFFER bufferHandle, const BufferCallback &bufferCallback) = 0;

		virtual void deleteBuffer(HBUFFER bufferHandle) = 0;

		// VAO
		typedef unsigned int HVAO;

		struct VAOConfig {
			int indexBufferSizeBytes;
			const void* indexData;
		};

		virtual HVAO createVAO(const VAOConfig& config) = 0;
		virtual void deleteVAO(HVAO vaoHandle) = 0;
	};
}