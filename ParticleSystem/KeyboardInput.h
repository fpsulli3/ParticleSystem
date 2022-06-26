#pragma once

namespace input {

	class KeyboardInput {
	public:
		enum KeyCode : int;

		// Returns true if the key is currently being pressed
		virtual bool isKeyDown(KeyCode code) const = 0;

		// Returns true if the key is currently NOT being pressed
		virtual bool isKeyUp(KeyCode code) const = 0;

		// Returns true if the key was newly pressed this frame
		virtual bool isKeyDownEdge(KeyCode code) const = 0;

		// Returns true if the key was newly released this frame
		virtual bool isKeyUpEdge(KeyCode code) const = 0;

		// Informs the keyboard input that the next frame is about to begin.
		virtual void onFrameBegin() = 0;

		// The codes representing the keys on the keyboard.
		// I could have just used the Win32 Vk Codes, but I 
		// wanted to keep this class platform-independent. So, 
		// you'll see there is a Win32KeyboardInput subclass that 
		// does the conversion.
		enum KeyCode : int {
			KC_UNKNOWN = -1,
			KC_SPACE,
			KC_RETURN,
			KC_LSHIFT,
			KC_RSHIFT,
			KC_LCTRL,
			KC_RCTRL,
			KC_LALT,
			KC_RALT,
			KC_TAB,
			KC_ESC,
			KC_UP,
			KC_DOWN,
			KC_LEFT,
			KC_RIGHT,
			KC_INSERT,
			KC_DELETE,
			KC_HOME,
			KC_END,
			KC_0 = 0x31,
			KC_1,
			KC_2,
			KC_3,
			KC_4,
			KC_5,
			KC_6,
			KC_7,
			KC_8,
			KC_9,
			KC_A = 0x41,
			KC_B,
			KC_C,
			KC_D,
			KC_E,
			KC_F,
			KC_G,
			KC_H,
			KC_I,
			KC_J,
			KC_K,
			KC_L,
			KC_M,
			KC_N,
			KC_O,
			KC_P,
			KC_Q,
			KC_R,
			KC_S,
			KC_T,
			KC_U,
			KC_V,
			KC_W,
			KC_X,
			KC_Y,
			KC_Z,
			KC_NUMPAD0,
			KC_NUMPAD1,
			KC_NUMPAD2,
			KC_NUMPAD3,
			KC_NUMPAD4,
			KC_NUMPAD5,
			KC_NUMPAD6,
			KC_NUMPAD7,
			KC_NUMPAD8,
			KC_NUMPAD9,
			KC_F1,
			KC_F2,
			KC_F3,
			KC_F4,
			KC_F5,
			KC_F6,
			KC_F7,
			KC_F8,
			KC_F9,
			KC_F10,
			KC_F11,
			KC_F12,
			MAX_KEYS
		};
	};
}