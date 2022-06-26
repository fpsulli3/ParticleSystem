#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "MouseInput.h"
#include "Win32InputHandlers.h"

namespace input {
	class Win32MouseInput : public MouseInput, public Win32MouseEventHandler {
	public:
		int getPosX() const { return curFrameX; }
		int getPosY() const { return curFrameY; }
		int getDeltaX() const { return deltaX; }
		int getDeltaY() const { return deltaY; }
		void onFrameBegin();

		void onMouseMoved(UINT xPos, UINT yPos);

	private:
		bool firstInputReceived = false;
		int curFrameX = 0, curFrameY = 0;
		int prevFrameX = 0, prevFrameY = 0;
		int deltaX = 0, deltaY = 0;
	};
}