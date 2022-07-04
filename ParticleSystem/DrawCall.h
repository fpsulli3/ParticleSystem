#pragma once

#include "ResourceManager.h"

namespace gfx {

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