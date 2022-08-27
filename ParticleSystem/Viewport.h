#pragma once

// The Viewport tells us which rectangle on the 
// screen we'd like to draw to. Typically, this 
// would be the full screen, but sometimes we 
// just want to draw to part of it, for example 
// in a split-screen view, we'd use two viewports, 
// each taking up half the screen.
struct Viewport {
	int x;
	int y;
	int width;
	int height;
};