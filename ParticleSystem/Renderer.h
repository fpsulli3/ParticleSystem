#pragma once

#include <vector>
#include "Camera.h"
#include "ClearOptions.h"
#include "DrawCall.h"
#include "Viewport.h"

namespace gfx {
	// This is the class that is responsible for actually rendering 
	// things to the screen. This is an Abstract Base Class (ABC) that 
	// is totally agnostic of which graphics API we're using. Notice 
	// that there is nothing OpenGL-related in its public interface, 
	// or in our includes above. 
	// 
	// I then created a concrete class called GLRenderer that is a 
	// subclass of Renderer. GLRenderer is where all of the actual 
	// OpenGL code lives.
	// 
	// By doing it this way, I can create other subclasses of Renderer, 
	// such as VulkanRenderer, DX11Renderer, etc., and if I'm careful 
	// to make sure that most code depends on the Renderer ABC instead 
	// of the concerete api-specific subclass, then I can switch which 
	// API I'm using very easily. The same game can even ship with 
	// support from multiple APIs!
	class Renderer {
	public:
		// Clears the screen entirely, according to the ClearOptions.
		virtual void clear(const ClearOptions& clearOptions)=0;

		// Takes the properties of the Camera and informs the 
		// shaders about the Camera's properties so that everything 
		// is rendered in the right place, with the right field of 
		// view and perspective distortion, etc.
		virtual void setupCamera(const Camera& camera, const Viewport& viewport)=0;

		// Takes a list of draw calls and draws them!
		virtual void draw(const std::vector<DrawCall> drawCalls)=0;
	};
}