#include "GLDevice.h"
#include <gl/wglew.h>

namespace gfx {
	GLDevice::GLDevice(HWND hwnd) {
		
		deviceContext = GetDC(hwnd);

		// First we have to choose a pixel format for our 
		// Win32 Device Context. In order to do that, we 
		// need to fill out this Pixel Format Descriptor so
		// that we can let Win32 know what this pixel format 
		// must support.
		// Here, we're telling Win32 that we need a pixel format 
		// that supports 32-bit color in RGBA format, with 24 bits 
		// for depth, and 8 bits for stencil. We're also telling 
		// Win32 that we want the pixel format to support OpenGL, 
		// and we need it to have a double-buffered frame buffer.
		PIXELFORMATDESCRIPTOR pfd = {
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA,		// The kind of framebuffer.
			32,					// Colordepth of the framebuffer.
			0, 0, 0, 0, 0, 0,
			0,
			0,
			0,
			0, 0, 0, 0,
			24,					// Number of bits for the depthbuffer
			8,					// Number of bits for the stencilbuffer
			0,					// Number of Aux buffers in the framebuffer.
			PFD_MAIN_PLANE,
			0,
			0, 0, 0
		};

		// Now we pass the Pixel Format Descriptor to Win32 so it 
		// can choose a pixel format for us that supports all of the 
		// things we need. This returns an integer id that identifies 
		// the pixel format the Win32 selected for us.
		int format = ChoosePixelFormat(deviceContext, &pfd);

		// Now, we actually tell Win32 to use the pixel format that 
		// it selected for us.
		SetPixelFormat(deviceContext, format, &pfd);

		// Now we tell the Win32 OpenGL implementation to give us 
		// a TEMPORARY render context. We won't want to use this 
		// context because we haven't had a chance to define the 
		// attributes we need, such as the OpenGL version. We're 
		// just going to use this temporary rendering context to 
		// initialize the OpenGL extensions. This is kind of a silly 
		// bootstrapping process, but it's necessary in this case.
		HGLRC tempRenderContext = wglCreateContext(deviceContext);
		wglMakeCurrent(deviceContext, tempRenderContext);

		// Init the OpenGL extensions.
		glewInit();

		wglMakeCurrent(deviceContext, 0);

		// Here we're creating a list of attributes that 
		// specifies that we want OpenGL 4.6 Core Profile.
		// Core Profile means we don't want access to any 
		// antiquated OpenGL functionality (some programs 
		// will want this for backward compatibility reasons, 
		// but we just want the modern stuff).
		static const int attributes[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
			WGL_CONTEXT_MINOR_VERSION_ARB, 6,
			WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			NULL
		};

		// Now we create a rendering context using these attributes.
		// Notice that we do that with an ARB extension. That's why we 
		// needed the temporary context; so we can initialize the extensions.
		// Stupid, but it is what it is.
		renderContext = wglCreateContextAttribsARB(deviceContext, 0, attributes);
	}

	GLDevice::~GLDevice() {
		// Gotta clean up
		if (renderContext != NULL) {
			wglDeleteContext(renderContext);
		}
	}

	void GLDevice::makeCurrent() {
		wglMakeCurrent(deviceContext, renderContext);
	}

	void GLDevice::swapBuffers() {
		SwapBuffers(deviceContext);
	}
}
