#pragma once

#include <dinput.h>
#include "General.h"
#include "Window.h"

class DirectInput
{
public:

	IDirectInputDevice8* Mouse;
	IDirectInputDevice8* Keyboard;

	LPDIRECTINPUT8 DIController;

	DirectInput(HINSTANCE hInstance);
	~DirectInput();

	void MouseState(DIMOUSESTATE* dest);
	void KeyboardState(BYTE* dest);

	bool Setup();
	void Dispose();

private:
	HINSTANCE hInstance;
};
