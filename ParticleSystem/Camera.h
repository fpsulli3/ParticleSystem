#pragma once

#include "Transform.h"
#include "KeyboardInput.h"
#include "MouseInput.h"

class Camera {
public:
	// Constructor
	Camera();

	// fovy is a common abbreviation
	// for "Field of View Angle in the Vertical (y) Direction"
	float getFovy() const { return fovy;  }

	// The near plane clipping distance.
	// Anything that is closer to the camera than this
	// will be clipped (i.e. won't be drawn).
	float getNear() const { return nearPlane;  }

	// The far plane clipping distance.
	// This defines the draw distance. Anything 
	// further away from the camera than this 
	// will be clipped.
	float getFar() const { return farPlane; }

	// The Transform represents the camera's position 
	// and orientation (a.k.a rotation) in 3D space.
	const Transform& getTransform() const { return transform; }

	// Processes keyboard input so that we can 
	// control the camera with the keyboard
	void processInput(const input::KeyboardInput& kb, const input::MouseInput& mouse, float deltaT);

private:

	float fovy = glm::pi<float>() / 3.0f; // 60 degrees.
	float nearPlane = 0.2f;
	float farPlane = 1000.0f;
	Transform transform;
	float pitch = 0.0f, yaw = 0.0f;
};