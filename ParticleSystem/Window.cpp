#include <map>
#include <Windowsx.h>

#include "Window.h"

// We declared this static property already in the
// Window class. However, this line of code actually 
// defines and initializes it.
std::map<HWND, Window*> Window::windows;

// Constructor
Window::Window(const WindowParams& params)
    : nCmdShow(params.nCmdShow),
    clientWidth(0), 
    clientHeight(0),
    hwnd(NULL) {

    // This is a bit confusing, but Win32 has this notion of
    // "Window Class", which is just a template of source that 
    // defines how the window will behave.
    //
    // This is not to be confused with our custom Window class, 
    // which is a C++ class that we're using to encapulate a window.
    // 
    // The Win32 "Window Class" allows us to make multiple windows 
    // that each share these properties and thererfore have similar 
    // behavior. We have to create this "Window Class" and register 
    // it with the Win32 system. Then, when we as Win32 to create a 
    // window for us, we need to specify which registered "Window Class"
    // to use.
    const wchar_t CLASS_NAME[] = L"Particle System Window Class";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc; // The Window procedure. See definition below for more.
    wc.hInstance = params.hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    DWORD dwStyle = params.fullScreen ? WS_POPUP : WS_OVERLAPPEDWINDOW;

    // Now we can actually create the window. The HWND that is returned 
    // is the Win32 "Window Handle". It is basically just a number that 
    // is used by Win32 to uniquely identify the window. When we want to 
    // tell Win32 to do something with our window, we have to give it 
    // this HWND handle so that it knows what window we're talking about.
    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles (none)
        CLASS_NAME,                     // The name of our Window Class
        params.title,                   // The window's title text
        dwStyle | CS_OWNDC,             // The window's style.
        CW_USEDEFAULT, CW_USEDEFAULT,   // The window's position (we're telling Win32 to put the window wherever it wants)
        params.width, params.height,    // Window width/height
        NULL,                           // Parent window    
        NULL,                           // Menu
        params.hInstance,               // Instance handle
        NULL                            // Additional application data
    );

    if (hwnd != NULL) {
        // Window creation worked.
       
        // The client rect is the drawable rectangle of 
        // the window (not including the title bar, etc).
        RECT rect;
        GetClientRect(hwnd, &rect);

        this->clientWidth = rect.right - rect.left;
        this->clientHeight = rect.bottom - rect.top;
        this->hwnd = hwnd;

        // We do this to keep track of all of the windows 
        // we create (just one right now, but could be more
        // later). 
        windows.emplace(hwnd, this);
    }
}

// Destructor
Window::~Window() {
    if (this->hwnd != NULL) {
        // Remove this Window instance from 
        // our static lookup.
        windows.erase(this->hwnd);

        // Tell Win32 that we don't need this 
        // window anymore.
        DestroyWindow(this->hwnd);
    }
}

void Window::show() {
    ShowWindow(hwnd, nCmdShow);
}

LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    // The HWND (Window Handle) param tells us which of our 
    // windows received the event. Here, we are using a static 
    // Window lookup to find out which of our Window instances 
    // this HWND is associated with. We only have one Window 
    // for this program, but it was easy to support multiple 
    // windows, so I just did it.
    std::map<HWND, Window*>::iterator windowItr = windows.find(hwnd);
    if (windowItr == windows.end()) {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    Window& window = *(windowItr->second);

    switch (uMsg) {
    case WM_CLOSE :
        // The user closed the window. We post a Quit Message to 
        // tell the program to quit. If you look at the main loop
        // in main.cpp, you can see we're looking for this Quit 
        // Message (called WM_QUIT).
        PostQuitMessage(0);
        break;
    case WM_SIZE:
        // The user resized the window. 
        // In this case, the width and height of the window are 
        // BOTH stored in the lParam. The lower 16 bits contains 
        // the width and we can use the LOWORD macro to pick these
        // bits out. The upper 16 bits contains the height, and we 
        // can use the HIWORD macro to pick those bits out.
        window.clientWidth = LOWORD(lParam);
        window.clientHeight = HIWORD(lParam);
    break;
    case WM_KEYUP:
        // The user released a key that was pressed. The wParam 
        // contains a code indicating which key was pressed. We 
        // pass the wParam to the Window's onKeyUp function. Our 
        // input system is watching calls to this function to stay 
        // informed as to which keyboard keys are being pressed.
        if (window.keyboardEventHandler != nullptr) {
            window.keyboardEventHandler->onKeyUp(wParam);
        }
        break;
    case WM_KEYDOWN:
        // The user pressed a key. The wParam contains a code 
        // indicating which key was pressed. We pass the wParam 
        // to the Window's onKeyUp function. Our input system is 
        // watching calls to this function to stay informed as 
        // to which keyboard keys are being pressed.
        if (window.keyboardEventHandler != nullptr) {
            window.keyboardEventHandler->onKeyDown(wParam);
        }
        break;

    case WM_INPUT: 
        UINT dwSize = sizeof(RAWINPUT);
        RAWINPUT raw;

        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &raw, &dwSize, sizeof(RAWINPUTHEADER));

        if (raw.header.dwType == RIM_TYPEMOUSE)
        {
            LONG xPosRelative = raw.data.mouse.lLastX;
            LONG yPosRelative = raw.data.mouse.lLastY;

            if (window.mouseEventHandler != nullptr) {
                window.mouseEventHandler->onMouseMoved(xPosRelative, yPosRelative);
            }
        }
        break;
    }

    // Whether or not we handled the message, we have to 
    // pass it to the "default Window Procedure" and return 
    // the result.
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}