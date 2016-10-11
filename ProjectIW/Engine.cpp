#include "Engine.h"


Engine::Engine(HWND handle, HINSTANCE hInstance)
{
	rendering = new RenderEngine(handle);
	input = new InputController(hInstance, handle);
}


Engine::~Engine(void)
{
	delete input;
	delete rendering;
}

bool Engine::Setup()
{
	if (!input->Setup())
		return false;
	if (!rendering->Setup())
		return false;
	return true;
}

void Engine::Loop()
{
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			SetCursorPos(400, 400);
			input->UpdateMousePosition();
			input->Update();
			rendering->Render();
		}
	}
}