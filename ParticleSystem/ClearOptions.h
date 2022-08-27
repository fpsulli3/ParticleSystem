#pragma once

// Each frame, we want to clear the back 
// buffer before we start drawing again.
// The back buffer is more than just color data; 
// it also contains depth data, and stencil data.
// This allows us to choose which data we want to 
// clear, and to what values.
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