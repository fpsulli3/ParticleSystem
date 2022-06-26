#include "Win32MouseInput.h"
#include <stdio.h>

namespace input {

	void Win32MouseInput::onFrameBegin() {
		prevFrameX = curFrameX;
		prevFrameY = curFrameY;
		deltaX = deltaY = 0;
	}

	void Win32MouseInput::onMouseMoved(UINT deltaX, UINT deltaY) {
		this->deltaX = deltaX;
		this->deltaY = deltaY;

		curFrameX += deltaX;
		curFrameY += deltaY;

		firstInputReceived = true;
	}
}