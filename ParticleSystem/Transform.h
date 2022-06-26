#pragma once

#include<glm/glm.hpp>

class Transform {
public:
	// Gets the direction pointed to the right
	// from the point of view of this Transform.
	glm::vec3 right();

	// Gets the direction pointed upwards
	// from the point of view of this Transform.
	glm::vec3 up();

	// Gets the direction pointed forward (a.k.a. "looking at")
	// from the point of view of this Transform.
	glm::vec3 at();

	// Moves the Transform by the specified 
	// x, y, and z offsets.
	void translate(float x, float y, float z);
	
	// Moves the Transform by the specified 
	// 
	void translate(const glm::vec3 offset);
};