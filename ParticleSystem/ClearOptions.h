#pragma once

struct ClearOptions {
	bool clearColor;
	bool clearDepth;
	bool clearStencil;
	float r;
	float g;
	float b;
	float a;
	float depth;
	int stencilValue;
};