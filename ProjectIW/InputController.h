#pragma once

#include "General.h"
#include "DirectInput.h"

class InputController
{
public:
	InputController(HINSTANCE, HWND hwnd);
	~InputController();
	bool Setup();
	void UpdateMousePosition();
	void Update();

	//for these two functions, they originally just returned v & 0x80, but the compiler complained about
	//a performance hit due to implicitly casting an int to bool. now it returns v & 0x80 != 0
	inline bool CurrentKey(int k) { return (current_keys[k] & 0x80) != 0; }
	inline bool LastKey(int k) { return (last_keys[k] & 0x80) != 0; }
	inline bool CurrentKeyOnly(int k) { return CurrentKey(k) && !LastKey(k); }

	inline int CurrentDeltaX() { return mouse_pos.lX; }
	inline int CurrentDeltaY() { return mouse_pos.lY; }
	inline int LastDeltaX() { return last_mouse.lX; }
	inline int LastDeltaY() { return last_mouse.lY; }
	inline int CurrentX() { return current_pos.x; }
	inline int CurrentY() { return current_pos.y; }
	inline int LastX() { return last_pos.x; }
	inline int LastY() { return last_pos.y; }
	inline bool LeftButton() { return current_mouse.rgbButtons[0] != 0; }
	inline bool RightButton() { return current_mouse.rgbButtons[1] != 0; }
	inline bool MiddleButton() { return current_mouse.rgbButtons[2] != 0; }
	inline bool LeftButtonOnly() { return current_mouse.rgbButtons[0] != 0 && last_mouse.rgbButtons[0] == 0; }
	inline bool RightButtonOnly() { return current_mouse.rgbButtons[1] != 0 && last_mouse.rgbButtons[1] == 0; }
	inline int GetScroll() { return current_scroll; }

	inline void LockCursor()
	{
		if (lock_cursor)
			return;
		lock_cursor = true;
		ShowCursor(false);
		GetCursorPos(&lock_point);
	}
	inline void UnlockCursor()
	{
		if (!lock_cursor)
			return;
		lock_cursor = false;
		ShowCursor(true);
		SetCursorPos(lock_point.x, lock_point.y);
	}

private:
	HWND _hwnd;
	DirectInput* direct_input;
	BYTE current_keys[256];
	BYTE last_keys[256];
	DIMOUSESTATE current_mouse;
	DIMOUSESTATE last_mouse;
	DIMOUSESTATE mouse_pos;
	bool lock_cursor;
	POINT current_pos;
	POINT last_pos;

	int current_scroll;
	int total_scroll;
	POINT lock_point;
};
