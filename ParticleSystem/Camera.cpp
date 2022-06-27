#include "Camera.h"

#define TRANSLATION_UNITS_PER_SECOND 8
#define ROTATION_RADIANS_PER_PIXEL 0.05
#define PITCH_LIMIT 1.55f // A little less than 90 degrees


Camera::Camera() {
	transform.translate(0, 1, 0);
}

void Camera::processInput(const input::KeyboardInput& kb, const input::MouseInput& mouse, float deltaT) {
	bool doTranslate = false;
	glm::vec3 translationOffset(0, 0, 0);

	if (kb.isKeyDown(input::KeyboardInput::KC_W)) {
		// Move toward where the camera is looking "at" (flat ground)
		glm::vec3 dir = transform.at();
		dir.y = 0;
		dir = glm::normalize(dir);
		translationOffset += (deltaT * TRANSLATION_UNITS_PER_SECOND) * dir;
		doTranslate = true;
	}

	if (kb.isKeyDown(input::KeyboardInput::KC_S)) {
		// Move away from where the camera is looking "at" (flat ground)
		glm::vec3 dir = transform.at();
		dir.y = 0;
		dir = glm::normalize(dir);
		translationOffset -= (deltaT * TRANSLATION_UNITS_PER_SECOND) * dir;
		doTranslate = true;
	}

	if (kb.isKeyDown(input::KeyboardInput::KC_D)) {
		// Move toward the camera's right
		translationOffset += (deltaT * TRANSLATION_UNITS_PER_SECOND) * transform.right();
		doTranslate = true;
	}

	if (kb.isKeyDown(input::KeyboardInput::KC_A)) {
		// Move toward the camera's left
		translationOffset -= (deltaT * TRANSLATION_UNITS_PER_SECOND) * transform.right();
		doTranslate = true;
	}

	if (doTranslate) {
		transform.translate(translationOffset);
	}

	bool doRotate = false;
	int mouseDeltaX = mouse.getDeltaX(), mouseDeltaY = mouse.getDeltaY();
	if (mouseDeltaX != 0) {
		// Look up/down
		float yawOffset = deltaT * ROTATION_RADIANS_PER_PIXEL * -mouseDeltaX;
		yaw += yawOffset;
		doRotate = true;
	}

	if (mouseDeltaY != 0) {
		// Look left/right
		float pitchOffset = deltaT * ROTATION_RADIANS_PER_PIXEL * -mouseDeltaY;
		pitch += pitchOffset;
		pitch = glm::clamp(pitch, -PITCH_LIMIT, PITCH_LIMIT);
		doRotate = true;
	}

	if (doRotate) {
		glm::quat rotation(glm::vec3(pitch, yaw, 0.0f));
		transform.setRotation(rotation);
	}
}
