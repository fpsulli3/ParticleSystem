#include "Win32Timer.h"

Win32Timer::Win32Timer() {
	QueryPerformanceCounter(&startTime);
	QueryPerformanceFrequency(&frequency);
}

void Win32Timer::onFrameBegin() {
	LARGE_INTEGER curTime;
	QueryPerformanceCounter(&curTime);
	LARGE_INTEGER totalEllapsed;
	totalEllapsed.QuadPart = curTime.QuadPart - startTime.QuadPart;
	double time = (double)totalEllapsed.QuadPart / (double)frequency.QuadPart;

	deltaTime = time - totalTime;
	totalTime = time;
}