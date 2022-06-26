#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <map>
#include "Win32InputHandlers.h"

// Gives us some options in terms 
// of what kind of window we want 
// to create.
struct WindowParams {
	HINSTANCE hInstance;
	int nCmdShow;
	const wchar_t* title;
	int width, height;
};

// Our custom Window class.
// This is basically just a wrapper around a Win32 window.
// Dealing directly with the Win32 API is a bit of a pain, 
// and so this makes it a little easier. But it takes a 
// but of "glue".
class Window {
public:
	// Constructor
	Window(const WindowParams& params);

	// Destructor
	~Window();

	// The Client Width is the width of the drawable area 
	// of the window, excluding the window's title bar, status
	// bar, etc.
	int getClientWidth() { return this->clientWidth;  }

	// The Client Height is the width of the drawable area 
	// of the window, excluding the window's title bar, status
	// bar, etc.
	int getClientHeight() { return this->clientHeight;  }

	// The Win32 Window Handle (HWND). An HWND is basically just 
	// an integer that Win32 uses to uniquely identify a window.
	HWND getHandle() { return this->hwnd; }

	// Allows an outside object to "register" to be informed of 
	// any important keyboard events, such as when a key is pressed 
	// or when a key is released.
	void setKeyboardEventHandler(input::Win32KeyboardEventHandler*handler) { keyboardEventHandler = handler; }

	// Allows an outside object to "register" to be informed of  
	// any important mouse events, such as when the mouse moves, 
	// or a mouse button is pressed.
	void setOnMouseMovedHandler(input::Win32MouseEventHandler *handler) { mouseEventHandler = handler;  }

	// When a Window is first created, it is typically not visible. Calling 
	// this function will make it visible.
	void show();

private:
	int clientWidth, clientHeight, nCmdShow;
	HWND hwnd;
	input::Win32KeyboardEventHandler *keyboardEventHandler = nullptr;
	input::Win32MouseEventHandler *mouseEventHandler = nullptr;

	// The Window Procedure is a function that defines how we're 
	// going to deal with Win32 events. Events include things like 
	// keyboard presses, mouse clicks, the user resizing the window, 
	// and the user closing the window.
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// This is a static Window lookup that we can use to 
	// keep track of every Window we create. The HWND is 
	// Win32 window handle -- basically a numeric id that 
	// Windows uses to identify the window. This map allows 
	// us to look up which instance of our Window class is 
	// assocated with a particular HWND.
	static std::map<HWND, Window*> windows;
};