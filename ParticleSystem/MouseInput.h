#pragma once

namespace input {
	class MouseInput {
	public:
		virtual int getPosX() const = 0;
		virtual int getPosY() const = 0;
		virtual int getDeltaX() const = 0;
		virtual int getDeltaY() const = 0;
		virtual void onFrameBegin() = 0;
	};
}