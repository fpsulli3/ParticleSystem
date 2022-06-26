#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "KeyboardInput.h"
#include "Win32InputHandlers.h"

namespace input {
	class Win32KeyboardInput : public KeyboardInput, public Win32KeyboardEventHandler {
	public:
		bool isKeyUp(KeyCode code) const;
		bool isKeyDown(KeyCode code) const;
		bool isKeyUpEdge(KeyCode code) const;
		bool isKeyDownEdge(KeyCode code) const;

		void onFrameBegin();

		void onKeyDown(UINT vkCode);
		void onKeyUp(UINT vkCode);

	private:
		// Key states. true = key is down, false = key is up
		bool previousKeyStates[MAX_KEYS] = { false };
		bool currentKeyStates[MAX_KEYS] = { false };
	};
}