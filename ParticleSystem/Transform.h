#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

// A Transform represents the position, rotation, and scale of an object.
class Transform {
public:
	// Gets the 4x4 matrix for this transform
	const glm::mat4& getMatrix() const;

	// Gets the direction pointed to the right
	// from the point of view of this Transform.
	glm::vec3 right() const;

	// Gets the direction pointed upwards
	// from the point of view of this Transform.
	glm::vec3 up() const;

	// Gets the direction pointed forward (a.k.a. "looking at")
	// from the point of view of this Transform.
	glm::vec3 at() const;

	// Moves the Transform by the specified 
	// x, y, and z offsets.
	void translate(float x, float y, float z);
	
	// Moves the Transform by the specified vector offset.
	void translate(const glm::vec3 offset);

	// Sets the Transform's rotation directly
	void setRotation(const glm::quat& rotation);

private:

	glm::vec3 translation = glm::vec3(0,0,0);
	glm::quat rotation = glm::quat(1,0,0,0);
	glm::vec3 scale = glm::vec3(1,1,1);

	// The entire Transform can be represented by a 
	// single 4x4 matrix. 
	mutable glm::mat4 matrix = glm::mat4(1);

	// This the dirty flag is true, this tells us that the 
	// matrix is out-of-sync with the translation, rotation, 
	// and scale properties and therefore needs to be 
	// recalculated.
	mutable bool dirty = false;
};