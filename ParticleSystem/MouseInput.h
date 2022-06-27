#pragma once

namespace input {
	class MouseInput {
	public:
		virtual int getDeltaX() const = 0;
		virtual int getDeltaY() const = 0;
		virtual void onFrameBegin() = 0;
	};
}