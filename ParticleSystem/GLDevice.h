#pragma once

#define	WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <gl/glew.h>

namespace gfx {

	// Represents the 'device' itself; sort of 
	// an abstract representation of the graphics 
	// card and the window surface that we're 
	// drawing to.
	class GLDevice {
	public:
		// Constructor
		GLDevice(HWND hwnd);

		// Destructor
		~GLDevice();

		// Tells OpenGL that future OpenGL calls 
		// will apply to this GLDevice.
		void makeCurrent();

		// Swaps the front and back screen buffers.
		// Pretty much all video games have at least two 
		// screen buffers, known as the front buffer and 
		// the back buffer. The back buffer is what we 
		// draw to. The front buffer is what is currently 
		// being displayed on the screen. This makes sure 
		// that the user doesn't see the frame as it's 
		// being drawn. Once we're finished drawing to 
		// the back buffer, we swap the buffers so that 
		// the back buffer becomes the front buffer and 
		// vice-versa, and the user will see the newly-
		// drawn frame while we begin to draw the next 
		// frame. Some games have longer swap chains with 
		// more than two buffers (you might've seen triple 
		// buffering as an option in some games).
		void swapBuffers();

	private:
		// A handle to a Win32 Device Context.
		// In Win32, the Device Context gives us 
		// a way of drawing to the Window. We 
		// give this Device Context handle to 
		// OpenGL so that it draw to our window.
		HDC deviceContext;

		// A handle to an OpenGL Render Context.
		// This sort of represents the graphics 
		// hardware itself 
		HGLRC renderContext;
	};
}