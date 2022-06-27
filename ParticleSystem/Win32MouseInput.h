#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "MouseInput.h"
#include "Win32InputHandlers.h"

namespace input {
	class Win32MouseInput : public MouseInput, public Win32MouseEventHandler {
	public:
		Win32MouseInput(HWND hwnd);

		int getDeltaX() const { return deltaX; }
		int getDeltaY() const { return deltaY; }
		void onFrameBegin();

		void onMouseMoved(LONG xPos, LONG yPos);

	private:
		bool firstInputReceived = false;
		int deltaX = 0, deltaY = 0;
	};
}