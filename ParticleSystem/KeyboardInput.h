#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace input {
	class KeyboardInput {
	public:
		bool isKeyUp(UINT vkCode);
		bool isKeyDown(UINT vkCode);
		bool isKeyUpEdge(UINT vkCode);
		bool isKeyDownEdge(UINT vkCode);

		void notifyKeyUpEvent(UINT vkCode);
		void notifyKeyDownEvent(UINT vkCode);

		void swapBuffers();

	private:
		static const unsigned int NUM_KEYS = 256;

		bool previous[NUM_KEYS] = { false };
		bool current[NUM_KEYS] = { false };
	};
}