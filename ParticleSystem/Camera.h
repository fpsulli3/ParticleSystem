#pragma once

#define PI 3.14159265358979f

class Camera {
public:
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

private:

	float fovy = PI / 2.0f; // half-pi radians is 90 degrees.
	float nearPlane = 1.0f;
	float farPlane = 1000.0f;
};