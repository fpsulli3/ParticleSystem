#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <GL/GL.h>

struct GLDrawCall {
	GLuint vao;
	GLenum mode;
	GLsizei count;
	GLenum indexType;
	GLuint program;
};