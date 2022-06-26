
#include "Win32KeyboardInput.h"
#include <algorithm>

namespace input {

	bool Win32KeyboardInput::isKeyUp(KeyCode code) const {
		return !currentKeyStates[code];
	}

	bool Win32KeyboardInput::isKeyDown(KeyCode code) const {
		return currentKeyStates[code];
	}

	bool Win32KeyboardInput::isKeyUpEdge(KeyCode code) const {
		return !currentKeyStates[code] && previousKeyStates[code];
	}

	bool Win32KeyboardInput::isKeyDownEdge(KeyCode code) const {
		return currentKeyStates[code] && !previousKeyStates[code];
	}

	void Win32KeyboardInput::onFrameBegin() {
		for (int i = 0; i < MAX_KEYS; i++) {
			previousKeyStates[i] = currentKeyStates[i];
		}
	}

	KeyboardInput::KeyCode lookupKeyCode(UINT vkCode) {
		static bool initialized = false;
		static KeyboardInput::KeyCode codes[256];
		if (!initialized) {
			std::fill_n(codes, 256, KeyboardInput::KC_UNKNOWN);
			codes[VK_SPACE] = KeyboardInput::KC_SPACE;
			codes[VK_RETURN] = KeyboardInput::KC_RETURN;
			codes[VK_LSHIFT] = KeyboardInput::KC_LSHIFT;
			codes[VK_RSHIFT] = KeyboardInput::KC_RSHIFT;
			codes[VK_LCONTROL] = KeyboardInput::KC_LCTRL;
			codes[VK_RCONTROL] = KeyboardInput::KC_RCTRL;
			codes[VK_LMENU] = KeyboardInput::KC_LALT;
			codes[VK_RMENU] = KeyboardInput::KC_RALT;
			codes[VK_TAB] = KeyboardInput::KC_TAB;
			codes[VK_ESCAPE] = KeyboardInput::KC_ESC;
			codes[VK_UP] = KeyboardInput::KC_UP;
			codes[VK_DOWN] = KeyboardInput::KC_DOWN;
			codes[VK_LEFT] = KeyboardInput::KC_LEFT;
			codes[VK_RIGHT] = KeyboardInput::KC_RIGHT,
			codes[VK_INSERT] = KeyboardInput::KC_INSERT,
			codes[VK_DELETE] = KeyboardInput::KC_DELETE,
			codes[VK_HOME] = KeyboardInput::KC_HOME,
			codes[VK_END] = KeyboardInput::KC_END,
			codes['0'] = KeyboardInput::KC_0,
			codes['1'] = KeyboardInput::KC_1,
			codes['2'] = KeyboardInput::KC_2,
			codes['3'] = KeyboardInput::KC_3,
			codes['4'] = KeyboardInput::KC_4,
			codes['5'] = KeyboardInput::KC_5,
			codes['6'] = KeyboardInput::KC_6,
			codes['7'] = KeyboardInput::KC_7,
			codes['8'] = KeyboardInput::KC_8,
			codes['9'] = KeyboardInput::KC_9,
			codes['A'] = KeyboardInput::KC_A,
			codes['B'] = KeyboardInput::KC_B,
			codes['C'] = KeyboardInput::KC_C,
			codes['D'] = KeyboardInput::KC_D,
			codes['E'] = KeyboardInput::KC_E,
			codes['F'] = KeyboardInput::KC_F,
			codes['G'] = KeyboardInput::KC_G,
			codes['H'] = KeyboardInput::KC_H,
			codes['I'] = KeyboardInput::KC_I,
			codes['J'] = KeyboardInput::KC_J,
			codes['K'] = KeyboardInput::KC_K,
			codes['L'] = KeyboardInput::KC_L,
			codes['M'] = KeyboardInput::KC_M,
			codes['N'] = KeyboardInput::KC_N,
			codes['O'] = KeyboardInput::KC_O,
			codes['P'] = KeyboardInput::KC_P,
			codes['Q'] = KeyboardInput::KC_Q,
			codes['R'] = KeyboardInput::KC_R,
			codes['S'] = KeyboardInput::KC_S,
			codes['T'] = KeyboardInput::KC_T,
			codes['U'] = KeyboardInput::KC_U,
			codes['V'] = KeyboardInput::KC_V,
			codes['W'] = KeyboardInput::KC_W,
			codes['X'] = KeyboardInput::KC_X,
			codes['Y'] = KeyboardInput::KC_Y,
			codes['Z'] = KeyboardInput::KC_Z,
			codes[VK_NUMPAD0] = KeyboardInput::KC_NUMPAD0,
			codes[VK_NUMPAD1] = KeyboardInput::KC_NUMPAD1,
			codes[VK_NUMPAD2] = KeyboardInput::KC_NUMPAD2,
			codes[VK_NUMPAD3] = KeyboardInput::KC_NUMPAD3,
			codes[VK_NUMPAD4] = KeyboardInput::KC_NUMPAD4,
			codes[VK_NUMPAD5] = KeyboardInput::KC_NUMPAD5,
			codes[VK_NUMPAD6] = KeyboardInput::KC_NUMPAD6,
			codes[VK_NUMPAD7] = KeyboardInput::KC_NUMPAD7,
			codes[VK_NUMPAD8] = KeyboardInput::KC_NUMPAD8,
			codes[VK_NUMPAD9] = KeyboardInput::KC_NUMPAD9,
			codes[VK_F1] = KeyboardInput::KC_F1,
			codes[VK_F2] = KeyboardInput::KC_F2,
			codes[VK_F3] = KeyboardInput::KC_F3,
			codes[VK_F4] = KeyboardInput::KC_F4,
			codes[VK_F5] = KeyboardInput::KC_F5,
			codes[VK_F6] = KeyboardInput::KC_F6,
			codes[VK_F7] = KeyboardInput::KC_F7,
			codes[VK_F8] = KeyboardInput::KC_F8,
			codes[VK_F9] = KeyboardInput::KC_F9,
			codes[VK_F10] = KeyboardInput::KC_F10,
			codes[VK_F11] = KeyboardInput::KC_F11,
			codes[VK_F12] = KeyboardInput::KC_F12,
			initialized = true;
		}	

		return codes[vkCode];
	}

	void Win32KeyboardInput::onKeyUp(UINT vkCode) {
		KeyboardInput::KeyCode kc = lookupKeyCode(vkCode);
		if (kc >= 0) {
			currentKeyStates[kc] = false;
		}
	}

	void Win32KeyboardInput::onKeyDown(UINT vkCode) {
		KeyboardInput::KeyCode kc = lookupKeyCode(vkCode);
		if (kc >= 0) {
			currentKeyStates[kc] = true;
		}
	}
}