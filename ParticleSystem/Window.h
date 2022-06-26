#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <map>
#include <functional>

// Typedef this because it's ugly.
typedef std::function<void(UINT vkCode)> OnKeyEventHandler;

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

	// Allows the caller to set an OnKeyEventHandler for "Key Up" 
	// events, which are raised whenever the user presses a key.
	// This uses a mechanism called a "Handler" a.k.a. a "Callback".
	// The OnKeyEventHandler is a function that we can pass in like 
	// a variable. Whenever a key is pressed, the Window class will 
	// call this function. 
	void setOnKeyDownHandler(OnKeyEventHandler handler) { onKeyDown = handler; }

	// Allows the caller to set an OnKeyEventHandler for "Key Up" 
	// events, which are raised whenever the user releases a key 
	// that was pressed.
	void setOnKeyUpHandler(OnKeyEventHandler handler) { onKeyUp = handler; }

	// When a Window is first created, it is typically not visible. Calling 
	// this function will make it visible.
	void show();

private:
	int clientWidth, clientHeight, nCmdShow;
	HWND hwnd;
	OnKeyEventHandler onKeyUp;
	OnKeyEventHandler onKeyDown;

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