#include "Win32Timer.h"

Win32Timer::Win32Timer(): totalTime(0.0), deltaTime(0.001) {
	QueryPerformanceCounter(&prevTime);
	QueryPerformanceFrequency(&frequency);
}

void Win32Timer::onFrameBegin() {
	LARGE_INTEGER curTime;
	QueryPerformanceCounter(&curTime);
	LARGE_INTEGER dt;
	dt.QuadPart = curTime.QuadPart - prevTime.QuadPart;

	deltaTime = (double)dt.QuadPart / (double)frequency.QuadPart;
	totalTime += deltaTime;
	prevTime = curTime;
}