#include "IBO.h"

IBO::IBO(void* data, unsigned int size, bool modifiable)
{
	this->buffer = 0;
	this->buffer_size = size;
	CreateBuffer(data, modifiable);
}

IBO::~IBO()
{
	Dispose();
}

void IBO::Dispose()
{
	if (buffer)
	{
		buffer->Release();
		buffer = 0;
	}
}

bool IBO::CreateBuffer(void* data, bool modifyable)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = (!modifyable ? D3D11_USAGE_DEFAULT : D3D11_USAGE_DYNAMIC);
	desc.ByteWidth = this->buffer_size;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = (!modifyable ? 0 : D3D11_CPU_ACCESS_WRITE);
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA resource;
	ZeroMemory(&resource, sizeof(D3D11_SUBRESOURCE_DATA));
	resource.pSysMem = data;

	HRESULT hr = engine->rendering->Device->CreateBuffer(&desc, &resource, &buffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, "Failed to create index buffer.", "Error", MB_ICONERROR);
		return false;
	}

	return true;
}

void IBO::SetData(void* data, UINT offset, UINT length)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	engine->rendering->ImmediateContext->Map(buffer, 0, D3D11_MAP_WRITE, 0, &resource);
	memcpy((void*)((char*)resource.pData + offset), data, length);
	engine->rendering->ImmediateContext->Unmap(buffer, 0);
}

void IBO::Bind()
{
	const UINT offset = 0;
	engine->rendering->ImmediateContext->IASetIndexBuffer(buffer, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
}