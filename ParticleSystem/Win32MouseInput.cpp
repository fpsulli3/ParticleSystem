#include "Win32MouseInput.h"

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif

namespace input {
	Win32MouseInput::Win32MouseInput(HWND hwnd) {
		RAWINPUTDEVICE Rid[1];
		Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
		Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
		Rid[0].dwFlags = RIDEV_INPUTSINK;
		Rid[0].hwndTarget = hwnd;
		RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

		ShowCursor(FALSE);
	}

	void Win32MouseInput::onFrameBegin() {
		deltaX = deltaY = 0;
	}

	void Win32MouseInput::onMouseMoved(LONG deltaX, LONG deltaY) {
		this->deltaX += deltaX;
		this->deltaY += deltaY;

		firstInputReceived = true;
	}
}