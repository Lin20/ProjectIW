#pragma once

#include <d3d11.h>
#include "General.h"

#define REFRESH_RATE 60

class RenderEngine
{
public:
	ID3D11Device* Device = 0;
	IDXGISwapChain* SwapChain = 0;
	ID3D11DeviceContext* ImmediateContext = 0;
	ID3D11RasterizerState* Rasterizer = 0;
	ID3D11RasterizerState* WireframeRasterizer = 0;

	ID3D11Texture2D* BackBuffer = 0;
	ID3D11RenderTargetView* RenderTargetView = 0;
	ID3D11Texture2D* DepthBuffer = 0;
	ID3D11DepthStencilView* DepthStencilView = 0;

	D3D11_VIEWPORT Viewport;

	RenderEngine(HWND);
	~RenderEngine(void);

	bool Setup();
	void UpdateViewport();
	void Render();
	void Dispose();

private:
	HWND windowHandle;
	Scene* scene = 0;

	bool SetupDevice();
	bool SetupBuffer();
	bool SetupDepthBuffer();
	bool SetupRasterizerState();
};
