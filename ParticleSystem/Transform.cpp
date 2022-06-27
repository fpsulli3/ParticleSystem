#include "Transform.h"

const glm::mat4& Transform::getMatrix() const {
	if (dirty) {
		// Since the dirty flag is true, we 
		// need to recalculate the matrix 
		// before we return it.

		matrix = glm::mat4(1); // Start with an identity matrix 
		matrix = glm::translate(matrix, translation); // Translate it
		matrix *= glm::toMat4(rotation); // Rotate it
		matrix = glm::scale(matrix, scale); // Scale it

		// We are now in sync
		dirty = false;
	}

	return matrix;
}

glm::vec3 Transform::right() const {
	// Don't get the matrix directly; it may 
	// need to be recalculated;
	const glm::mat4& matrix = getMatrix();
	return matrix[0];
}

glm::vec3 Transform::up() const {
	// Don't get the matrix directly; it may 
	// need to be recalculated;
	const glm::mat4& matrix = getMatrix();
	return matrix[1];
}

glm::vec3 Transform::at() const {
	// Don't get the matrix directly; it may 
	// need to be recalculated;
	const glm::mat4& matrix = getMatrix();
	return -matrix[2];
}

void Transform::translate(float x, float y, float z) {
	translation += glm::vec3(x, y, z);
	dirty = true;
}

void Transform::translate(const glm::vec3 offset) {
	translation += offset;
	dirty = true;
}

void Transform::setRotation(const glm::quat& rotation) {
	this->rotation = rotation;
	dirty = true;
}
