#pragma once

#include <DirectXMath.h>

using namespace DirectX;

__declspec(align(16))
struct BasicShaderConstants
{
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX projection;
};
