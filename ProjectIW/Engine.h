#pragma once

#include "General.h"
#include "RenderEngine.h"
#include "InputController.h"
#include <Windows.h>

class Engine
{
public:
	RenderEngine* rendering;
	InputController* input;

	Engine(HWND handle, HINSTANCE hInstance);
	~Engine(void);

	bool Setup();
	void Loop();

private:
};
