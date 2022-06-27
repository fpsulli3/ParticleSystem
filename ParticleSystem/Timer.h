#pragma once

class Timer {
public:
	virtual void onFrameBegin() = 0;
	virtual double getTotalTime() const = 0;
	virtual double getDeltaTime() const = 0;
};