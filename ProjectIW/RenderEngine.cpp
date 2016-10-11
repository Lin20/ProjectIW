#include "RenderEngine.h"
#include "Scene.h"
#include <string>

int BUFFER_WIDTH = 1280;
int BUFFER_HEIGHT = 720;

RenderEngine::RenderEngine(HWND handle)
{
	this->windowHandle = handle;
}

RenderEngine::~RenderEngine(void)
{
	Dispose();
}

bool RenderEngine::Setup()
{
	this->scene = 0;
	if (!SetupDevice())
		return false;
	if (!SetupBuffer())
		return false;
	if (!SetupDepthBuffer())
		return false;
	if (!SetupRasterizerState())
		return false;
	void* v = _aligned_malloc(sizeof(Scene), 16);
	this->scene = new (v)Scene();
	if (!this->scene->Setup())
		return false;

	return true;
}

bool RenderEngine::SetupDevice()
{
	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BufferCount = 1;
	desc.BufferDesc.Width = BUFFER_WIDTH;
	desc.BufferDesc.Height = BUFFER_HEIGHT;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferDesc.RefreshRate.Numerator = REFRESH_RATE;
	desc.BufferDesc.RefreshRate.Denominator = 1;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.OutputWindow = this->windowHandle;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Windowed = true;

	D3D_FEATURE_LEVEL  FeatureLevelsRequested[3] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 };
	UINT               numLevelsRequested = 1;
	D3D_FEATURE_LEVEL  FeatureLevelsSupported;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, FeatureLevelsRequested, 3, D3D11_SDK_VERSION, &desc, &SwapChain, &Device, &FeatureLevelsSupported, &ImmediateContext);
	if (FAILED(hr))
	{
		MessageBox(0, std::to_string(hr).c_str(), "DIR", MB_OK);
		MessageBox(NULL, "Failed to create device.", "Error", 0);
		return false;
	}

	return true;
}

bool RenderEngine::SetupBuffer()
{
	if (FAILED(SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&BackBuffer)))
	{
		MessageBox(NULL, "Failed to create back buffer.", "Error", 0);
		return false;
	}
	HRESULT hr = Device->CreateRenderTargetView(BackBuffer, NULL, &RenderTargetView);
	BackBuffer->Release();
	if (FAILED(hr))
	{
		MessageBox(NULL, "Failed to create render target view.", "Error", 0);
		return false;
	}

	ZeroMemory(&Viewport, sizeof(Viewport));
	UpdateViewport();

	return true;
}

bool RenderEngine::SetupDepthBuffer()
{
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = BUFFER_WIDTH;
	desc.Height = BUFFER_HEIGHT;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	if (FAILED(Device->CreateTexture2D(&desc, NULL, &DepthBuffer)))
	{
		MessageBox(0, "Failed to create depth buffer.", "Error", MB_ICONERROR);
		return false;
	}
	if (FAILED(Device->CreateDepthStencilView(DepthBuffer, NULL, &DepthStencilView)))
	{
		MessageBox(0, "Failed to create depth stencil view.", "Error", MB_ICONERROR);
		return false;
	}

	return true;
}

bool RenderEngine::SetupRasterizerState()
{
	D3D11_RASTERIZER_DESC rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(rasterDesc));
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = true;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	if (FAILED(Device->CreateRasterizerState(&rasterDesc, &Rasterizer)))
	{
		MessageBox(0, "Failed to create rasterizer.", "Error", MB_ICONERROR);
		return false;
	}

	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterDesc.CullMode = D3D11_CULL_NONE;

	if (FAILED(Device->CreateRasterizerState(&rasterDesc, &WireframeRasterizer)))
	{
		MessageBox(0, "Failed to create rasterizer.", "Error", MB_ICONERROR);
		return false;
	}

	return true;
}

void RenderEngine::UpdateViewport()
{
	Viewport.TopLeftX = 0;
	Viewport.TopLeftY = 0;
	Viewport.Width = (float)BUFFER_WIDTH;
	Viewport.Height = (float)BUFFER_HEIGHT;
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;
	ImmediateContext->RSSetViewports(1, &Viewport);
	//if (scene)
	//	scene->UpdateCamera();
}

void RenderEngine::Render()
{
	//float color[4] = { 0.0f, 0.6f, 0.9f, 1.0f };
	float color[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	ImmediateContext->ClearRenderTargetView(RenderTargetView, color);
	ImmediateContext->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	ImmediateContext->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);

	ImmediateContext->RSSetState(Rasterizer);

	if (scene)
	{
		scene->Update();
		scene->Render();
	}

	SwapChain->Present(1, 0);
}

void RenderEngine::Dispose()
{
	if (scene)
	{
		//delete scene;
		scene->~Scene();
		_aligned_free(scene);
	}
	if (SwapChain)
		SwapChain->Release();
	if (Device)
		Device->Release();
	if (ImmediateContext)
		ImmediateContext->Release();
	if (BackBuffer)
		BackBuffer->Release();
	if (RenderTargetView)
		RenderTargetView->Release();
	if (DepthBuffer)
		DepthBuffer->Release();
	if (DepthStencilView)
		DepthStencilView->Release();
	if (Rasterizer)
		Rasterizer->Release();
	if (WireframeRasterizer)
		WireframeRasterizer->Release();
}
