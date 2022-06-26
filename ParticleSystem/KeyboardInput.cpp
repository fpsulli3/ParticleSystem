
#include "KeyboardInput.h"

namespace input {
	bool KeyboardInput::isKeyUp(UINT vkCode) {
		return !current[vkCode];
	}

	bool KeyboardInput::isKeyDown(UINT vkCode) {
		return current[vkCode];
	}

	bool KeyboardInput::isKeyUpEdge(UINT vkCode) {
		return !current[vkCode] && previous[vkCode];
	}

	bool KeyboardInput::isKeyDownEdge(UINT vkCode) {
		return current[vkCode] && !previous[vkCode];
	}

	void KeyboardInput::notifyKeyUpEvent(UINT vkCode) {
		current[vkCode] = false;
	}

	void KeyboardInput::notifyKeyDownEvent(UINT vkCode) {
		current[vkCode] = true;
	}

	void KeyboardInput::swapBuffers() {
		for (int i = 0; i < NUM_KEYS; i++) {
			previous[i] = current[i];
		}
	}
}