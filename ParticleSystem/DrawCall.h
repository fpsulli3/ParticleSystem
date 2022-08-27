#pragma once

#include "ResourceManager.h"

namespace gfx {

	// Represents a single draw call
	// Notice that this is api-agnostic; it does not 
	// know anything about OpenGL per se. This means 
	// hopefully that we can use the same DrawCall class
	// with other graphics APIs.
	struct DrawCall {

		enum class Mode {
			TRIANGLES, 
			TRIANGLE_STRIP,
			TRIANGLE_FAN,
			POINTS,
			LINES
		};

		enum class IndexType {
			USHORT,
			UINT
		};

		Mode mode;
		IndexType indexType;
		int numIndices;
		const void* indices;

		ResourceManager::HPROGRAM programHandle;
		ResourceManager::HVAO vaoHandle;
		ResourceManager::HBUFFER storageBuffer;
		unsigned int storageBufferBaseIndex;
	};
}