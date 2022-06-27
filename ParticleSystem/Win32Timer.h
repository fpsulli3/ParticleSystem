#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "Timer.h"

class Win32Timer: public Timer {
public:
	Win32Timer();

	void onFrameBegin();
	double getTotalTime() const { return totalTime;  }
	double getDeltaTime() const { return deltaTime;  }
private:
	LARGE_INTEGER startTime, prevTime, frequency;
	double totalTime = 0.0, deltaTime = 0.0;
};