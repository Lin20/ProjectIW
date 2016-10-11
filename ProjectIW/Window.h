#pragma once

#include <windows.h>
#include <iostream>
#include <sstream>
#include <tchar.h>
#include "General.h"
#include "Engine.h"

#define winstring LPCTSTR

const TCHAR WINDOW_TITLE[] = _T("ProjectIW");
static TCHAR WINDOW_CLASS[] = _T("win32app");
static HWND hwnd;
static HINSTANCE windowHInstance;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int HandleMessages();
