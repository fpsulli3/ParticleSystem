namespace gfx {

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
		typedef unsigned int HBUFFER;

		virtual HBUFFER createStreamingUniformBuffer(unsigned int initialDataSize, unsigned char* initialData) = 0;
		virtual void streamDataToUniformBuffer(HBUFFER bufferHandle, unsigned int dataSize, const void* data) = 0;
		virtual HBUFFER createStreamingStorageBuffer(unsigned int initialDataSize, unsigned char* initialData) = 0;
		virtual void streamDataToStorageBuffer(HBUFFER bufferHandle, unsigned int dataSize, const void* data) = 0;
		virtual void deleteBuffer(HBUFFER bufferHandle) = 0;
	};
}