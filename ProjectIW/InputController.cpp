#include "InputController.h"

InputController::InputController(HINSTANCE h, HWND hwnd)
{
	direct_input = new DirectInput(h);
	lock_cursor = false;
	_hwnd = hwnd;
}

InputController::~InputController()
{
	if (direct_input)
		delete direct_input;
}

bool InputController::Setup()
{
	ZeroMemory(current_keys, sizeof(current_keys));
	ZeroMemory(last_keys, sizeof(last_keys));
	ZeroMemory(&current_mouse, sizeof(current_mouse));
	ZeroMemory(&last_mouse, sizeof(last_mouse));
	ZeroMemory(&mouse_pos, sizeof(mouse_pos));
	current_pos.x = current_pos.y = last_pos.x = last_pos.y = 0;
	return direct_input->Setup();
}

void InputController::Update()
{
	if (!direct_input)
		return;

	memcpy(last_keys, current_keys, sizeof(last_keys));
	memcpy(&last_mouse, &current_mouse, sizeof(last_mouse));
	direct_input->KeyboardState(current_keys);
	direct_input->MouseState(&current_mouse);
	//if (CurrentKeyOnly(DIK_TAB))
	//	lock_cursor = !lock_cursor;


	current_scroll = total_scroll;
	total_scroll = 0;

	if (lock_cursor)
	{
		SetCursorPos(lock_point.x, lock_point.y);
	}
}

void InputController::UpdateMousePosition()
{
	if (!direct_input)
		return;

	direct_input->MouseState(&mouse_pos);
	total_scroll += mouse_pos.lZ;
	last_pos.x = current_pos.x;
	last_pos.y = current_pos.y;
	GetCursorPos(&current_pos);
	ScreenToClient(_hwnd, &current_pos);
}
