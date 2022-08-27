#pragma once

namespace gfx {
	// Represents the graphics device, such as the GPU.
	// 
	// This is an Abstract Base Class (ABC) with no knowledge 
	// or dependency on any particular graphics API.
	// 
	// I've created a subclass called GLDevice that contains 
	// the actual OpenGL implementation. In the future, I may 
	// want to add other implementations, such as VulkanDevice 
	// or DX11Device.
	class Device {
	public:
		// Tells the underlying graphics API that 
		// this is the device we're currently using.
		virtual void makeCurrent()=0;

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
		virtual void swapBuffers()=0;
	};
}
