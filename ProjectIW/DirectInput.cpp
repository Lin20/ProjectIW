#include "DirectInput.h"

DirectInput::DirectInput(HINSTANCE hInstance)
{
	this->hInstance = hInstance;
	Mouse = 0;
	Keyboard = 0;
	DIController = 0;
}

DirectInput::~DirectInput()
{
	Dispose();
}

bool DirectInput::Setup()
{
	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&DIController, NULL)))
	{
		MessageBox(0, "Failed to initialize direct input.", "Error", MB_ICONERROR);
		return false;
	}

	if (FAILED(DIController->CreateDevice(GUID_SysMouse, &Mouse, NULL)))
	{
		MessageBox(0, "Failed to create mouse.", "Error", MB_ICONERROR);
		return false;
	}
	Mouse->SetDataFormat(&c_dfDIMouse);
	Mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	if (FAILED(DIController->CreateDevice(GUID_SysKeyboard, &Keyboard, NULL)))
	{
		MessageBox(0, "Failed to create keyboard.", "Error", MB_ICONERROR);
		return false;
	}
	Keyboard->SetDataFormat(&c_dfDIKeyboard);
	Keyboard->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);

	return true;
}

void DirectInput::MouseState(DIMOUSESTATE* dest)
{
	if (!Mouse)
		return;
	Mouse->Acquire();
	if (FAILED(Mouse->GetDeviceState(sizeof(DIMOUSESTATE), dest)))
		MessageBox(0, "Failed to retrieve mouse state.", "Error", MB_ICONERROR);
}

void DirectInput::KeyboardState(BYTE* dest)
{
	if (!Keyboard)
		return;
	Keyboard->Acquire();
	if (FAILED(Keyboard->GetDeviceState(256, dest)))
		MessageBox(0, "Failed to retrieve keyboard state.", "Error", MB_ICONERROR);
}

void DirectInput::Dispose()
{
	if (Mouse)
		Mouse->Unacquire();
	if (Keyboard)
		Keyboard->Unacquire();
	if (DIController)
		DIController->Release();
}
