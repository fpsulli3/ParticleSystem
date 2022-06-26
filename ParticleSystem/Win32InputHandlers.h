#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace input {

	// Represents a class that can handle Win32 Keyboard events.
	class Win32KeyboardEventHandler {
	public:
		virtual void onKeyDown(UINT vkCode) = 0;
		virtual void onKeyUp(UINT vkCode) = 0;
	};

	// Represents a class that can handle Win32 Mouse Events
	class Win32MouseEventHandler {
	public:
		virtual void onMouseMoved(UINT xPos, UINT yPos) = 0;
	};
}