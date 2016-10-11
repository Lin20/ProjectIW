#include "Window.h"
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <direct.h>



#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

Engine* engine;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = WINDOW_CLASS;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL, "Error registering class.", "Error", MB_ICONERROR);
		return 1;
	}

	RECT clientSize = { 0, 0, BUFFER_WIDTH, BUFFER_HEIGHT };
	AdjustWindowRect(&clientSize, WS_OVERLAPPEDWINDOW, false);
	hwnd = CreateWindow(WINDOW_CLASS, WINDOW_TITLE, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, clientSize.right - clientSize.left, clientSize.bottom - clientSize.top, NULL, NULL, hInstance, NULL);
	if (!hwnd)
	{
		MessageBox(NULL, "Error creating window.", "Error", MB_ICONERROR);
		return 2;
	}


	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	engine = new Engine(hwnd, hInstance);
	if (!engine->Setup())
		return 1;

	engine->Loop();
	delete engine;


#ifdef _DEBUG
	//Print out any memory leaks
	_CrtDumpMemoryLeaks();

	CHAR s[500];
	GetCurrentDirectory(500, s);
	OutputDebugString(s);
	OutputDebugString("\nShutdown success.\n");
#endif

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			PostQuitMessage(0);
		break;
	case WM_EXITSIZEMOVE:
	case WM_SIZE:
		if (engine && engine->rendering)
		{
			RECT rect;
			GetClientRect(hwnd, &rect);
			BUFFER_WIDTH = (int)rect.right - rect.left;
			BUFFER_HEIGHT = (int)rect.bottom - rect.top;
			engine->rendering->UpdateViewport();
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:

		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

int HandleMessages()
{
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}